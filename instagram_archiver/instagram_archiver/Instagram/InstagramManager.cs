using HtmlAgilityPack;
using Instagram_archiver.Http;
using Instagram_archiver.Util;
using Instagram_archiverML.Model;
using Sodium;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Net.NetworkInformation;
using System.Security.Cryptography;
using System.Security.Policy;
using System.Text;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Telegram.Bot.Types.InlineQueryResults;

namespace Instagram_archiver.Instagram
{
    public sealed class InstagramManager
    {
        private static Lazy<InstagramManager> InstanceHolder { get; } = new (() => new ());
        public static HttpManager HttpManager { get; set; }

        public static InstagramManager GetInstance(HttpManager httpManager)
        {
            HttpManager = httpManager;

            return InstanceHolder.Value;
        }
        /*private static CookieContainer CookieContainer { get; } = new ();
        private static HttpClientHandler HttpClientHandler { get; } = new()
        {
            AutomaticDecompression = DecompressionMethods.All,
            CookieContainer = CookieContainer,
        };
        private HttpClient HttpClient { get; } = new (HttpClientHandler) { Timeout = TimeSpan.FromSeconds(20) };*/

        private InstagramRequestHeaders InstagramRequestHeaders { get; } = new ();

        private InstagramManager() { }

        //https://www.instagram.com/explore/grid/?is_prefetch=false&omit_cover_media=false&module=explore_popular&use_sectional_payload=true&cluster_id=explore_all%3A0&include_fixed_destinations=true&max_id=9
        // 사진 받아오는거

        public async Task<bool> LoginAsync(string id, string password)
        {
            HttpManager.HttpClient.DefaultRequestHeaders.Clear();
            HttpManager.HttpClient.DefaultRequestHeaders.Add("Referer", "https://www.instagram.com/");
            HttpManager.HttpClient.DefaultRequestHeaders.Add("User-Agent", InstagramRequestHeaders.UserAgent);
            HttpManager.HttpClient.DefaultRequestHeaders.Add("X-IG-App-ID", InstagramRequestHeaders.XIGAppId); //tray 드갈때도 보냄
            HttpManager.HttpClient.DefaultRequestHeaders.Add("X-IG-WWW-Claim", "0"); //tray드갈때 hmac 보냄
            HttpManager.HttpClient.DefaultRequestHeaders.Add("X-Requested-With", InstagramRequestHeaders.XRequestedWith); //post에서 보냄

            string result;
            try
            {
                result = await HttpManager.HttpClient.GetStringAsync("https://www.instagram.com/data/shared_data/");
            }
            catch(TaskCanceledException)
            {
                return false;
            }

            var json = JsonDocument.Parse(result);
            
            var xCsrfToken = json.RootElement.GetProperty("config").GetProperty("csrf_token").GetString();
            var xInstagramAjax = json.RootElement.GetProperty("rollout_hash").GetString();
            var encryption = json.RootElement.GetProperty("encryption");
            var keyId = encryption.GetProperty("key_id").GetString();
            var publicKey = encryption.GetProperty("public_key").GetString();
            var version = encryption.GetProperty("version").GetString();

            HttpManager.HttpClient.DefaultRequestHeaders.Add("X-CSRFToken", xCsrfToken); //로그인시만
            HttpManager.HttpClient.DefaultRequestHeaders.Add("X-Instagram-AJAX", xInstagramAjax); //로그인시만


            var postData = new Dictionary<string, string>
            {
                ["username"] = id,
                ["enc_password"] = EncryptPassword(password, publicKey, keyId, version),
                ["queryParams"] = "{}",
                ["optIntoOneTap"] = "false",
            };


            using var formUrlEncodedContent = new FormUrlEncodedContent(postData);
            formUrlEncodedContent.Headers.ContentType = MediaTypeHeaderValue.Parse("application/x-www-form-urlencoded");

            HttpResponseMessage httpResponseMessage;
            try
            {
                httpResponseMessage = await HttpManager.HttpClient.PostAsync("https://www.instagram.com/accounts/login/ajax/", formUrlEncodedContent);
            }
            catch(TaskCanceledException)
            {
                return false;
            }
            result = await httpResponseMessage.Content.ReadAsStringAsync();
            
            json = JsonDocument.Parse(result);

            var status = json.RootElement.GetProperty("status").GetString();

            if (status == "ok")
            {
                InstagramRequestHeaders.XIGWwwClaim = httpResponseMessage.Headers.GetValues("x-ig-set-www-claim").FirstOrDefault();
                return true;
            }
            Console.WriteLine(result);
            return false;
        }
        private async Task<HttpResponseMessage> BufferGetAsync(string uri)
        {
            try
            {
                return await HttpManager.HttpClient.GetAsync(uri);
            }
            catch
            {
                return null;
            }
        }
        public async Task<List<InstagramUser>> GetUsersAsync(int page)
        {
            HttpManager.HttpClient.DefaultRequestHeaders.Clear();
            HttpManager.HttpClient.DefaultRequestHeaders.Add("Referer", "https://www.instagram.com/explore");
            HttpManager.HttpClient.DefaultRequestHeaders.Add("User-Agent", InstagramRequestHeaders.UserAgent);
            HttpManager.HttpClient.DefaultRequestHeaders.Add("X-IG-App-ID", InstagramRequestHeaders.XIGAppId);
            HttpManager.HttpClient.DefaultRequestHeaders.Add("X-IG-WWW-Claim", InstagramRequestHeaders.XIGWwwClaim);
            HttpManager.HttpClient.DefaultRequestHeaders.Add("X-Requested-With", InstagramRequestHeaders.XRequestedWith);

            string result = "";

            try
            {
                result = await HttpManager.HttpClient.GetStringAsync($"https://www.instagram.com/explore/grid/?is_prefetch=false&omit_cover_media=false&module=explore_popular&use_sectional_payload=true&cluster_id=explore_all%3A0&include_fixed_destinations=true&max_id={page}");
            }
            catch(TaskCanceledException)
            {
                return default;
            }

            var json = JsonDocument.Parse(result);
            var users = new List<InstagramUser>();

            foreach (var item in json.RootElement.GetProperty("sectional_items").EnumerateArray())
            {
                string s = item.GetProperty("layout_type").GetRawText();
                try
                {
                    IEnumerable<JsonElement> medias = default;

                    switch (item.GetProperty("layout_type").GetString())
                    {
                        case "two_by_two_left":
                        case "two_by_two_right":
                            medias = item.GetProperty("layout_content").GetProperty("fill_items").EnumerateArray().Select(element => element.GetProperty("media"));
                            break;
                        case "media_grid":
                            medias = item.GetProperty("layout_content").GetProperty("medias").EnumerateArray().Select(media => media.GetProperty("media"));
                            break;
                        default:
                            throw new Exception("레이아웃 타입: " + item.GetProperty("layout_type"));
                    }
                    foreach (var media in medias) //각 미디어가 하나의 글임
                    {
                        var userElement = media.GetProperty("user");
                        var userId = userElement.GetProperty("username").GetString();
                        var user = users.FirstOrDefault(user => user.Id == userId);
                        var userName = userElement.GetProperty("full_name").GetString().Normalize();
                        var postId = media.GetProperty("code").GetString();
                        var content = media.GetProperty("caption").GetProperty("text").GetString();
                        InstagramPost post;

                        if (user != default)
                        {
                            post = user.Posts.FirstOrDefault(post => post.Id == postId);
                            
                            if(post == default)
                            {
                                var images = new List<InstagramImage>();
                                user.Posts.Add(new() { Id = postId, Content = content, Images = images });
                            }
                        }
                        else
                        {
                            var images = new List<InstagramImage>();
                            post = new() { Id = postId, Content = content, Images = images };
                            var posts = new List<InstagramPost>() { post };

                            user = new() { Id = userId, Name = userName, Posts = posts };
                            users.Add(user);
                        }
                        
                        
                        

                        string url;
                        if (media.TryGetProperty("carousel_media", out var carouselMedias))
                        {
                            foreach (var carouselMedia in carouselMedias.EnumerateArray())
                            {
                                var imageId = carouselMedia.GetProperty("pk").GetString();

                                url = carouselMedia.GetProperty("image_versions2").GetProperty("candidates")[0].GetProperty("url").GetString();
                                post.Images.Add(new() { Id = imageId, Url = url});
                            }
                        }
                        else
                        {
                            var imageId = media.GetProperty("id").GetString();

                            url = media.GetProperty("image_versions2").GetProperty("candidates")[0].GetProperty("url").GetString();
                            post.Images.Add(new() { Id = imageId, Url = url});
                        }
                    }

                }
                catch (Exception e) 
                {
                    await Logger.LogAsync(e.ToString());
                    Console.WriteLine("예외뜸! " + e);
                    await File.WriteAllTextAsync("log.txt", s + result + e);
                }

            }

            foreach(var user in users)
            {
                foreach(var post in user.Posts)
                {
                    var httpResponseMessageTasks = new List<Task<HttpResponseMessage>>(post.Images.Select(image => BufferGetAsync(image.Url)));
                    await Task.WhenAll(httpResponseMessageTasks);

                    if (httpResponseMessageTasks.Count != 0)
                    { 
                        for (int i = httpResponseMessageTasks.Count - 1; i >= 0; --i)
                        {
                            if (httpResponseMessageTasks[i].IsFaulted)
                            {
                                post.Images.RemoveAt(i);
                                continue;
                            }
                            var message = await httpResponseMessageTasks[i];

                            post.Images[i].Stream = await message.Content.ReadAsStreamAsync();
                        }
                    }
                }
            }

            return users;
        }
        
        private string EncryptPassword(string password, string publicKey, string keyId, string version)
        {
            var time = ((DateTimeOffset)DateTime.UtcNow).ToUnixTimeSeconds();
            var keyBytes = HexStringToBytes(publicKey);
            var key = new byte[32];
            new Random().NextBytes(key);
            var iv = new byte[12];
            var tag = new byte[16];

            var plainText = Encoding.UTF8.GetBytes(password);
            var cipherText = new byte[plainText.Length];
            using var cipher = new AesGcm(key);

            cipher.Encrypt(iv, plainText, cipherText, tag, Encoding.UTF8.GetBytes(time.ToString()));

            var encryptedKey = SealedPublicKeyBox.Create(key, keyBytes);
            var bytesOfLen = BitConverter.GetBytes((short)encryptedKey.Length);
            var info = new byte[] { 1, byte.Parse(keyId) };
            var bytes = CombineArrays(info, bytesOfLen, encryptedKey, tag, cipherText);

            return $"#PWD_INSTAGRAM_BROWSER:{version}:{time}:{Convert.ToBase64String(bytes)}";
        }

        private T[] CombineArrays<T>(params T[][] arrays)
        {
            T[] rv = new T[arrays.Sum(a => a.Length)];
            int offset = 0;

            foreach (T[] array in arrays)
            {
                Buffer.BlockCopy(array, 0, rv, offset, array.Length);
                offset += array.Length;
            }
            return rv;
        }

        private byte[] HexStringToBytes(string hex) => Enumerable.Range(0, hex.Length / 2).Select(x => Convert.ToByte(hex[(x * 2)..(x * 2 + 2)], 16)).ToArray();


    }
}
