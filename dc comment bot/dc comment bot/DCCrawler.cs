using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Tesseract;

namespace dc_comment_bot
{
    public class DCCrawler
    {
        private static Lazy<DCCrawler> InstanceHolder { get; } = new Lazy<DCCrawler>(() => new DCCrawler());
        public static HttpManager HttpManager { get; set; }
        public string UserAgent => "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.121 Safari/537.36 Edg/85.0.564.63";
        //private Dictionary<string, CsrfTokens> Tokens { get; } = new Dictionary<string, CsrfTokens>();
        private Dictionary<(string galleryName, int postId), CsrfTokens> Tokens = new();
        private DCCrawler() { }

        public static DCCrawler GetInstance(HttpManager httpManager)
        {
            HttpManager = httpManager;
            return InstanceHolder.Value;
        }

        public async Task<List<int>> GetPostIdsAsync(IPostMetadata postMetadata, int page, string[] keywords)
        {
            var dcPostMetadata = postMetadata as DCPostMetadata;
            var list = new List<int>();

            if (!await HttpManager.GetHtmlFromAsync($"https://gall.dcinside.com/{(dcPostMetadata.IsMinor ? "mgallery/" : "")}board/lists?id={dcPostMetadata.GalleryName}&page={page}"))
                return new List<int>();

            if (HttpManager.HtmlDocument.DocumentNode.SelectSingleNode("//td[@class='gall_subject']") != null)
            {
                foreach (var node in HttpManager.HtmlDocument.DocumentNode.SelectNodes("//td[@class='gall_subject']").Where(node => new[] { "뉴스", "공지", "이슈", "설문", "AD" }.All(s => s != node.InnerText)))
                {
                    var postId = int.Parse(node.SelectSingleNode("../td[@class='gall_num']").InnerText);
                    var title = node.SelectSingleNode("../td[@class='gall_tit ub-word']/a").InnerText;
                    
                    if(keywords.Any(keyword => title.IndexOf(keyword, StringComparison.OrdinalIgnoreCase) != -1))
                        list.Add(postId);
                }
            }
            else
            {
                var nodes = HttpManager.HtmlDocument.DocumentNode.SelectNodes("//td[@class='gall_num']");

                if (nodes != null)
                {
                    foreach (var node in nodes)
                    {
                        if (!int.TryParse(node.InnerText, out int postId))
                            continue;
                        var title = node.SelectSingleNode("../td[@class='gall_tit ub-word']/a").InnerText;
                        
                        if (keywords.Any(keyword => title.IndexOf(keyword, StringComparison.OrdinalIgnoreCase) != -1))
                            list.Add(postId);
                    }
                }
            }
            return list;
        }
        
        public async Task<bool> WriteCommentAsync(IPostMetadata postMetadata, string nickname, string password, string content)
        {
            var dcPostMetadata = postMetadata as DCPostMetadata;

            if (!await GetTokensAsync(dcPostMetadata))
            {
                Console.WriteLine("not found");
                return false;
            }
            var galleryType = dcPostMetadata.IsMinor ? "M" : "G";

            if (HttpManager.HtmlNode.SelectSingleNode("//input[@id='kcaptcha_use']").Attributes["value"].Value == "Y")
            {
                await DownloadCaptchaImage(postMetadata, "captcha.png");
                using var ocr = new TesseractEngine("./tessdata", "eng", EngineMode.Default);
                //var text = ocr.Process(Pix.LoadFromFile("captcha.png")).GetText();
                var text = ocr.Process(Pix.LoadFromFile("captcha.png"));
                Console.WriteLine("이거 인식됨 " + text);
            }
            return false;

            HttpManager.HttpClient.DefaultRequestHeaders.Clear();
            HttpManager.HttpClient.DefaultRequestHeaders.Add("Referer", $"https://gall.dcinside.com/{(dcPostMetadata.IsMinor ? "mgallery/" : "")}board/view/?id={dcPostMetadata.GalleryName}&no={dcPostMetadata.PostId}&page=1");
            HttpManager.HttpClient.DefaultRequestHeaders.Add("User-Agent", UserAgent);
            HttpManager.HttpClient.DefaultRequestHeaders.Add("X-Requested-With", "XMLHttpRequest");


            var tuple = (dcPostMetadata.GalleryName, dcPostMetadata.PostId);

            
            var formData = new Dictionary<string, string>
            {
                ["id"] = dcPostMetadata.GalleryName,
                ["no"] = dcPostMetadata.PostId.ToString(),
                ["name"] = nickname,
                ["password"] = password,
                ["memo"] = content,
                ["cur_t"] = Tokens[tuple].CurT,
                ["check_6"] = Tokens[tuple].Checks[0],
                ["check_7"] = Tokens[tuple].Checks[1],
                ["check_8"] = Tokens[tuple].Checks[2],
                ["check_9"] = Tokens[tuple].Checks[3],
                ["recommend"] = Tokens[tuple].Recommend,
                ["user_ip"] = Tokens[tuple].UserIP,
                ["t_vch2"] = "",
                ["t_vch2_chk"] = "",
                ["c_gall_id"] = dcPostMetadata.GalleryName,
                ["c_gall_no"] = dcPostMetadata.PostId.ToString(),
                ["service_code"] = Tokens[tuple].ServiceCode,
                ["g-recaptcha-response"] = "",
                ["_GALLTYPE"] = galleryType,
            };

            using var formUrlEncodedContent = new FormUrlEncodedContent(formData);
            formUrlEncodedContent.Headers.ContentType = MediaTypeHeaderValue.Parse("application/x-www-form-urlencoded; charset=UTF-8");
            string result = default;

            try
            {
                var responseMessage = await HttpManager.HttpClient.PostAsync("https://gall.dcinside.com/board/forms/comment_submit", formUrlEncodedContent);
                result = await responseMessage.Content.ReadAsStringAsync();
            }
            catch (Exception e)
            {
                await Logger.SendTelegramMessageAsync(e.ToString());
                return false;
            }
            Console.WriteLine(result);
            if (result.Contains("False"))
                return false;

            return true;
        }

        private async Task<bool> GetTokensAsync(IPostMetadata postMetadata)
        {
            var dcPostMetadata = postMetadata as DCPostMetadata;
            var tuple = (dcPostMetadata.GalleryName, dcPostMetadata.PostId);

            if (!Tokens.ContainsKey(tuple) || Tokens[tuple] == null)
            {
                var url = $"https://gall.dcinside.com/{(dcPostMetadata.IsMinor ? "mgallery/" : "")}board/view?id={dcPostMetadata.GalleryName}&no={dcPostMetadata.PostId}";
                if (!await HttpManager.GetHtmlFromAsync(url))
                    return false;
                var serviceCode = HttpManager.HtmlNode.SelectSingleNode("//input[@name='service_code']").Attributes["value"].Value;
                var match = Regex.Match(HttpManager.HtmlDocument.ParsedText, @"_d\('(.+)'\)");

                if (!match.Success)
                    throw new Exception("service_code failed");

                string[] key = Decode(match.Groups[1].Value, true).Split(',');
                string code = default;

                for (int i = 0; i < key.Length; ++i)
                    code += Encoding.Default.GetString(new[] { (byte)(2 * (Convert.ToDouble(key[i]) - i - 1) / (12 - i)) });

                Tokens[tuple] = new CsrfTokens
                {
                    CurT = HttpManager.HtmlNode.SelectSingleNode("//input[@id='cur_t']").Attributes["value"].Value,
                    ServiceCode = Regex.Replace(serviceCode, ".{10}$", code),
                    Esno = HttpManager.HtmlNode.SelectSingleNode("//input[@id='e_s_n_o']").Attributes["value"].Value,
                    Recommend = HttpManager.HtmlNode.SelectSingleNode("//input[@id='recommend']").Attributes["value"].Value,
                    UserIP = HttpManager.HtmlNode.SelectSingleNode("//input[@id='user_ip']").Attributes["value"].Value
                };

                for (int i = 0; i < 4; ++i)
                    Tokens[tuple].Checks[i] = HttpManager.HtmlNode.SelectSingleNode($"//input[@id='check_{i + 6}']").Attributes["value"].Value;
                await Task.Delay(500);
            }
            return true;

            static string Decode(string input, bool isDouble = false)
            {
                const string key = "yL/M=zNa0bcPQdReSfTgUhViWjXkYIZmnpo+qArOBs1Ct2D3uE4Fv5G6wHl78xJ9K";
                string result = default;
                StringBuilder sb = new StringBuilder(48);

                input = Regex.Replace(input, "[^A-Za-z0-9+/=]", string.Empty);

                int k1, k2, k3, k4, k1n2, k2n3, k3n4;

                for (int i = 0; i < input.Length;)
                {
                    k1 = key.IndexOf(input[i++]);
                    k2 = key.IndexOf(input[i++]);
                    k3 = key.IndexOf(input[i++]);
                    k4 = key.IndexOf(input[i++]);

                    k1n2 = (k1 << 2) | (k2 >> 4);
                    k2n3 = ((k2 & 15) << 4) | (k3 >> 2);
                    k3n4 = ((k3 & 3) << 6) | k4;

                    sb.Append(Encoding.Default.GetString(new[] { (byte)k1n2 }));

                    if (k3 != 64)
                        sb.Append(Encoding.Default.GetString(new[] { (byte)k2n3 }));

                    if (k4 != 64)
                        sb.Append(Encoding.Default.GetString(new[] { (byte)k3n4 }));
                }
                if (isDouble)
                {
                    result = sb.ToString();
                    int ch = int.Parse(result[..1]);
                    result = Regex.Replace(result, "^.", (ch > 5 ? ch - 5 : ch + 4).ToString());
                }
                return result;
            }
        }

        private async Task DownloadCaptchaImage(IPostMetadata postMetadata, string imagePath)
        {
            var dcPostMetadata = postMetadata as DCPostMetadata;

            await GetTokensAsync(new DCPostMetadata { GalleryName = dcPostMetadata.GalleryName, PostId = dcPostMetadata.PostId });

            var galleryType = dcPostMetadata.IsMinor ? "M" : "G";
            var postData = new Dictionary<string, string>
            {
                ["ci_t"] = HttpManager.CookieContainer.GetCookies(new Uri("https://www.gall.dcinside.com"))["ci_c"].Value,
                ["gall_id"] = dcPostMetadata.GalleryName,
                ["kcaptcha_type"] = "comment",
                ["_GALLTYPE_"] = galleryType,
            };

            try
            {
                await HttpManager.HttpClient.PostAsync("https://gall.dcinside.com/kcaptcha/session", new FormUrlEncodedContent(postData));
                await using var fs = new FileStream(imagePath, FileMode.Create);
                await (await HttpManager.HttpClient.GetStreamAsync($"https://gall.dcinside.com/kcaptcha/image/?gall_id={dcPostMetadata.GalleryName}&kcaptcha_type=comment&time={DateTimeOffset.Now.ToUnixTimeMilliseconds()}&_GALLTYPE_={galleryType}")).CopyToAsync(fs);
            }
            catch (Exception e)
            {
                await Logger.SendTelegramMessageAsync(e.ToString());
            }
        }
    }
}
