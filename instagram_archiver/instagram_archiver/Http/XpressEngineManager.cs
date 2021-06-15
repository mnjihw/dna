using HtmlAgilityPack;
using Instagram_archiver.Instagram;
using Instagram_archiver.Util;
using Microsoft.ML.Transforms.Image;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Security.Policy;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace Instagram_archiver.Http
{
    public enum WritePostResult
    {
        Success, Error
    }
    public class XpressEngineManager
    {
        private static Lazy<XpressEngineManager> InstanceHolder { get; } = new (() => new ());
        public static HttpManager HttpManager { get; set; }
        private static string Domain { get; set; }
        private string CsrfToken { get; set; }
        private XpressEngineManager() { }
        public static XpressEngineManager GetInstance(HttpManager httpManager, string domain)
        {
            HttpManager = httpManager;
            Domain = domain;

            return InstanceHolder.Value;
        }
        public async Task<bool> LoginAsync(string id, string password)
        {
            var postData = new Dictionary<string, string>
            {
                ["error_return_url"] = "/",
                ["mid"] = "index",
                ["ruleset"] = "@login",
                ["act"] = "procMemberLogin",
                ["success_return_url"] = "/",
                ["xe_validator_id"] = "widgets/login_info/skins/default/login_form/1",
                ["user_id"] = id,
                ["password"] = password,
            };
            HttpManager.HttpClient.DefaultRequestHeaders.Clear();
            HttpManager.HttpClient.DefaultRequestHeaders.Add("Referer", $"https://{Domain}/");
            await HttpManager.HttpClient.PostAsync($"https://{Domain}/index.php?act=procMemberLogin", new FormUrlEncodedContent(postData));

            if (await HttpManager.GetHtmlFromAsync($"https://{Domain}/"))
                CsrfToken = HttpManager.HtmlNode.SelectSingleNode("//meta[@name='csrf-token']").Attributes["content"].Value;

            return !string.IsNullOrWhiteSpace(CsrfToken);
        }

        public async Task<WritePostResult> WritePostAsync(string boardName, string basePath, InstagramUser user, InstagramPost post, bool allowsComment)
        {
            post.Content += " "; //마지막 해시태그 읽기 위해 공백 추가함
            
            int error;
            HttpManager.HttpClient.DefaultRequestHeaders.Clear();
            HttpManager.HttpClient.DefaultRequestHeaders.Add("Referer", $"https://{Domain}/");
            if (!await HttpManager.GetHtmlFromAsync($"https://{Domain}/index.php?mid={boardName}&act=dispBoardWrite"))
                return WritePostResult.Error;
            var editorSequence = HttpManager.HtmlNode.SelectSingleNode("//input[@id='xe-fileupload']").Attributes["data-editor-sequence"].Value;

            
            var guid = Guid.NewGuid();

            int totalImagesSize = 0;
            int uploadTargetSrl = 0;
            HttpResponseMessage message;
            string result;
            JsonDocument json;


            HttpManager.HtmlDocument.LoadHtml(""); //이미지 html을 직접 세팅해줘야 해서 초기화해줌

            for (int i = 0; i < post.Images.Count; ++i)
            {

                await Logger.LogAsync($"이미지 {i + 1}/{post.Images.Count}개 업로드 중...");

                var imageDirectory = Path.GetDirectoryName(basePath);
                var imageFileName = $"{post.Images[i].Hash}.jpg";
                
                var imagePath = $@"{basePath}\{imageFileName}";

                if (!File.Exists(imagePath))
                    continue;

                totalImagesSize += (int)new FileInfo(imagePath).Length;
                if (totalImagesSize >= 1024 * 1024 * 50)
                {
                    await Logger.LogAsync($"용량 초과로 이미지 {post.Images.Count - i}장 포기함");
                    for (int j = post.Images.Count - 1; j >= i; --j)
                        post.Images.RemoveAt(j);
                    break;
                }

                var multipartFormDataContent = new MultipartFormDataContent($"----WebKitFormBoundary{guid}")
                {
                    { new StringContent(editorSequence), "editor_sequence" },
                    { new StringContent("undefined"), "upload_target_srl" },
                    { new StringContent(boardName), "mid" },
                    { new StringContent("procFileUpload"), "act" }
                };

                var fs = File.OpenRead(imagePath);
                var httpContent = new StreamContent(fs);

                httpContent.Headers.ContentType = new MediaTypeHeaderValue("image/jpeg");
                httpContent.Headers.ContentDisposition = new ContentDispositionHeaderValue("form-data") { Name = "Filedata", FileName = imageFileName };
                multipartFormDataContent.Add(httpContent);

                try
                {
                    message = await HttpManager.HttpClient.PostAsync($"https://{Domain}/", multipartFormDataContent);
                    if (message.StatusCode != HttpStatusCode.OK)
                    {
                        await Task.Delay(1000);
                        continue;
                    }
                    result = await message.Content.ReadAsStringAsync();
                }
                catch
                {
                    continue;
                }
                json = JsonDocument.Parse(result);


                uploadTargetSrl = json.RootElement.GetProperty("upload_target_srl").GetInt32();


                error = json.RootElement.GetProperty("error").GetInt32();
                if (error != 0)
                {
                    await Logger.SendTelegramMessageAsync("error 1 " + result);

                }

                var fileSrl = json.RootElement.GetProperty("file_srl").GetInt32().ToString();
                var downloadUrl = json.RootElement.GetProperty("download_url").GetString();

                HttpManager.HtmlDocument.DocumentNode.AppendChild(HtmlNode.CreateNode($@"<p><img alt=""{post.Images[i].Hash}.jpg"" data-file-srl=""{fileSrl}"" editor_component=""image_link"" src=""{downloadUrl}""/></p>"));

                await Task.Delay(1000);
            }
            

            await Logger.LogAsync($"이미지 {post.Images.Count}장 업로드 완료");

            
            var content = $@"{HttpManager.HtmlDocument.DocumentNode.OuterHtml}<p>{post.Content}</p><p style=""text-align: center;""><a href=""https://www.instagram.com/p/{post.Id}/"" style=""font-size: 24px; color: #000;"" rel=""nofollow"" target=""_blank""><strong>[인스타그램으로 이동]</strong></a></p>";
            var title = $"[{user.Name}({user.Id})] {post.Content[..(post.Content.Length >= 20 ? 20 : post.Content.Length)]}...";


            var postData = new Dictionary<string, string>
            {
                ["_filter"] = "insert",
                ["error_return_url"] = $"/index.php?mid={boardName}&act=dispBoardWrite",
                ["act"] = "procBoardInsertDocument",
                ["mid"] = boardName,
                ["content"] = $"{content}", 
                ["document_srl"] = uploadTargetSrl.ToString(),
                ["title"] = title,
                ["_saved_doc_message"] = "자동 저장된 글이 있습니다. 복구하시겠습니까? 글을 다 쓰신 후 저장하면 자동 저장 본은 사라집니다.",
                ["comment_status"] = allowsComment ? "ALLOW" : "DISALLOW",
                ["allow_trackback"] = "Y",
                ["status"] = "PUBLIC",
                ["_rx_csrf_token"] = CsrfToken,
                ["use_editor"] = "Y",
                ["use_html"] = "Y",
                ["module"] = "board",
                ["_rx_ajax_compat"] = "XMLRPC",
            };
            var matches = Regex.Matches(post.Content, @"#(.+?)\s");

            if(matches.Count != 0)
                postData["tags"] = string.Join(',', matches.Select(match => match.Groups[1].Value));

            await Task.Delay(500);
            try
            {
                message = await HttpManager.HttpClient.PostAsync($"https://{Domain}/", new FormUrlEncodedContent(postData));
                if (message.StatusCode != HttpStatusCode.OK)
                {
                    if (message.StatusCode == HttpStatusCode.Forbidden)
                    {
                        var text = $"403 떴다! {content}{Environment.NewLine}{await message.Content.ReadAsStringAsync()}";
                        await Logger.SendTelegramMessageAsync(text);
                    }
                    else
                    {
                        var text = $"{message.StatusCode}코드 뜸 {content}{Environment.NewLine}{await message.Content.ReadAsStringAsync()}";
                        await Logger.SendTelegramMessageAsync(text);
                    }
                    return WritePostResult.Error;
                }
                result = await message.Content.ReadAsStringAsync();
            }
            catch (Exception e)
            {
                await Logger.SendTelegramMessageAsync(e.ToString());
                return WritePostResult.Error;
            }
            json = JsonDocument.Parse(result);
            error = json.RootElement.GetProperty("error").GetInt32();
            if (error == 0)
            {
                return WritePostResult.Success;
            }
            await Logger.SendTelegramMessageAsync($"error 2 {result}");
            return WritePostResult.Error;
        }

    }
}
