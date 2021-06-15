using Sodium;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Security.Cryptography;
using System.Text;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace instagram
{
    public sealed class InstagramManager
    {
        private static Lazy<InstagramManager> InstanceHolder { get; } = new Lazy<InstagramManager>(() => new InstagramManager());
        public static InstagramManager Instance => InstanceHolder.Value;
        private static CookieContainer CookieContainer { get; } = new CookieContainer();
        private static HttpClientHandler HttpClientHandler { get; } = new HttpClientHandler
        {
            AutomaticDecompression = DecompressionMethods.All,
            CookieContainer = CookieContainer,
        };
        private HttpClient HttpClient { get; } = new HttpClient(HttpClientHandler) { Timeout = TimeSpan.FromSeconds(10) };

        private InstagramRequestHeaders InstagramRequestHeaders { get; } = new InstagramRequestHeaders();

        private InstagramManager() { }

        //https://www.instagram.com/explore/grid/?is_prefetch=false&omit_cover_media=false&module=explore_popular&use_sectional_payload=true&cluster_id=explore_all%3A0&include_fixed_destinations=true&max_id=9
        // 사진 받아오는거

        public async Task<bool> Login(string id, string password)
        {
            HttpClient.DefaultRequestHeaders.Clear();
            HttpClient.DefaultRequestHeaders.Add("Referer", "https://www.instagram.com/");
            HttpClient.DefaultRequestHeaders.Add("User-Agent", InstagramRequestHeaders.UserAgent);
            HttpClient.DefaultRequestHeaders.Add("X-IG-App-ID", InstagramRequestHeaders.XIGAppId); //tray 드갈때도 보냄
            HttpClient.DefaultRequestHeaders.Add("X-IG-WWW-Claim", "0"); //tray드갈때 hmac 보냄
            HttpClient.DefaultRequestHeaders.Add("X-Requested-With", InstagramRequestHeaders.XRequestedWith); //post에서 보냄

            var result = await HttpClient.GetStringAsync("https://www.instagram.com/data/shared_data/");

            var json = JsonDocument.Parse(result);
            var xCsrfToken = json.RootElement.GetProperty("config").GetProperty("csrf_token").GetString();
            var xInstagramAjax = json.RootElement.GetProperty("rollout_hash").GetString();
            var encryption = json.RootElement.GetProperty("encryption");
            var keyId = encryption.GetProperty("key_id").GetString();
            var publicKey = encryption.GetProperty("public_key").GetString();
            var version = encryption.GetProperty("version").GetString();

            HttpClient.DefaultRequestHeaders.Add("X-CSRFToken", xCsrfToken); //로그인시만
            HttpClient.DefaultRequestHeaders.Add("X-Instagram-AJAX", xInstagramAjax); //로그인시만


            var postData = new Dictionary<string, string>
            {
                ["username"] = id,
                ["enc_password"] = EncryptPassword(password, publicKey, keyId, version),
                ["queryParams"] = "{}",
                ["optIntoOneTap"] = "false",
            };


            using var formUrlEncodedContent = new FormUrlEncodedContent(postData);
            formUrlEncodedContent.Headers.ContentType = MediaTypeHeaderValue.Parse("application/x-www-form-urlencoded");
            var httpResponseMessage = await HttpClient.PostAsync("https://www.instagram.com/accounts/login/ajax/", formUrlEncodedContent);
            result = await httpResponseMessage.Content.ReadAsStringAsync();
            Console.WriteLine(result);
            json = JsonDocument.Parse(result);

            var status = json.RootElement.GetProperty("status").GetString();

            if (status == "ok")
            {
                InstagramRequestHeaders.XIGWwwClaim = httpResponseMessage.Headers.GetValues("x-ig-set-www-claim").FirstOrDefault();
                return true;
            }
            return false;
        }
        private async Task<HttpResponseMessage> BufferGetAsync(string uri)
        {
            try
            {
                return await HttpClient.GetAsync(uri);
            }
            catch
            {
                return null;
            }
        }
        public async Task<bool> ArchivePictures(int page, List<(string, InstagramPhoto)> list, string pathToSave = "")
        {
            var path = Path.Combine(pathToSave ?? string.Empty, "images");

            Directory.CreateDirectory(path);


            HttpClient.DefaultRequestHeaders.Clear();
            HttpClient.DefaultRequestHeaders.Add("Referer", "https://www.instagram.com/explore");
            HttpClient.DefaultRequestHeaders.Add("User-Agent", InstagramRequestHeaders.UserAgent);
            HttpClient.DefaultRequestHeaders.Add("X-IG-App-ID", InstagramRequestHeaders.XIGAppId);
            HttpClient.DefaultRequestHeaders.Add("X-IG-WWW-Claim", InstagramRequestHeaders.XIGWwwClaim);
            HttpClient.DefaultRequestHeaders.Add("X-Requested-With", InstagramRequestHeaders.XRequestedWith);

            var result = await HttpClient.GetStringAsync($"https://www.instagram.com/explore/grid/?is_prefetch=false&omit_cover_media=false&module=explore_popular&use_sectional_payload=true&cluster_id=explore_all%3A0&include_fixed_destinations=true&max_id={page}");

            var json = JsonDocument.Parse(result);

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
                            throw new Exception("레이아웃 타입" + item.GetProperty("layout_type"));
                            Console.WriteLine("레이아웃 타입 다른거: " + item.GetProperty("layout_type").GetString());
                            break;
                    }
                    foreach (var media in medias)
                    {
                        var caption = media.GetProperty("caption");
                        var id = caption.GetProperty("user").GetProperty("username").GetString();
                        var name = caption.GetProperty("user").GetProperty("full_name").GetString().Normalize();
                        var code = media.GetProperty("code").GetString();

                        var photo = new InstagramPhoto { Code = code, Id = id, Name = name };

                        if (media.TryGetProperty("carousel_media", out var carouselMedias))
                        {
                            var imageId = carouselMedias[0].GetProperty("pk").GetString();
                            
                            /*foreach (var carouselMedia in carouselMedias.EnumerateArray())
                                list.Add(carouselMedia.GetProperty("image_versions2").GetProperty("candidates")[0].GetProperty("url").GetString());
                            */ // 한 사람이 여러 장 올린 이미지는 가치가 없음
                            //캐러셀_미디어 안에 media_type이 1이면 사진 2면 동영상 썸네일
                            list.Add((carouselMedias[0].GetProperty("image_versions2").GetProperty("candidates")[0].GetProperty("url").GetString(), photo));
                        }
                        else
                        {
                            list.Add((media.GetProperty("image_versions2").GetProperty("candidates")[0].GetProperty("url").GetString(), photo));
                            
                        }
                    }

                }
                catch (Exception e)
                {
                    Console.WriteLine("예외뜸!");
                    await File.WriteAllTextAsync("log.txt", s);
                }

            }

            /*var httpResponseMessageTasks = new List<Task<HttpResponseMessage>>(list.Select(url => BufferGetAsync(url)));
            await Task.WhenAll(httpResponseMessageTasks);

            if (httpResponseMessageTasks.Count != 0)
            {

                for (int i = 0; i < httpResponseMessageTasks.Count; ++i)
                {
                    if (httpResponseMessageTasks[i].IsFaulted)
                        continue;
                    var message = await httpResponseMessageTasks[i];

                    var fileName = Path.GetFileNameWithoutExtension(Path.GetRandomFileName()) + ".jpg";

                    await using var ms = new MemoryStream();
                    await message.Content.CopyToAsync(ms);
                    ms.Seek(0, SeekOrigin.Begin);

                    await using var fs = new FileStream($@"{path}\{fileName}", FileMode.CreateNew, FileAccess.ReadWrite, FileShare.Read);
                    await ms.CopyToAsync(fs);


                }
            }

*/



            return false;
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
