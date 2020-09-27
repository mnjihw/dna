using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Net.Http;
using System.IO;
using System.Text.RegularExpressions;
using HtmlAgilityPack;
using Newtonsoft.Json.Linq;
using System.Security.Cryptography;
using System.Diagnostics;
using System.Threading;

namespace 리듬_DC_콘솔_봇
{


    public sealed class DCManager
    {

        private static readonly Lazy<DCManager> instanceHolder = new Lazy<DCManager>(() => new DCManager());
        public static DCManager Instance => instanceHolder.Value;

        private static readonly CookieContainer cookieContainer = new CookieContainer();
        private static readonly HttpClientHandler httpClientHandler = new HttpClientHandler
        {
            CookieContainer = cookieContainer,
            //UseProxy = true,
            //Proxy = new WebProxy("158.69.59.171:3128")
        };
        private static readonly HttpClient httpClient = new HttpClient(httpClientHandler) { Timeout = TimeSpan.FromSeconds(10) };

        private bool? IsMinorGallery { get; set; }
        private readonly List<(string, int)> imageBlacklist = new List<(string, int)>();
        private readonly Dictionary<string, string> testValues = new Dictionary<string, string>();


        private DCManager()
        {
            try
            {
                foreach (var line in File.ReadAllLines("이미지 블랙리스트.txt"))
                {
                    var result = line.Split(' ');

                    if (result.Length != 1)
                        imageBlacklist.Add((result[0], int.Parse(result[1])));
                }

            }
            catch (IOException)
            {
                Console.WriteLine("이미지 블랙리스트.txt 없음");
            }
        }


        public async Task<int> GetArticleNumber(string galleryName, bool isLatest)
        {
            string result = default;

            if (!IsMinorGallery.HasValue)
                IsMinorGallery = false;

            while (true)
            {
                try
                {
                    result = await httpClient.GetStringAsync($"https://gall.dcinside.com/{(IsMinorGallery.Value ? "mgallery/" : "")}board/lists?id={galleryName}");

                    if (string.IsNullOrWhiteSpace(result))
                    {
                        Console.WriteLine("ㅁㅊ 빔");
                        await WaitFor(90);
                        continue;
                    }
                    else if (result.Length < 200 && result.Contains("location.replace"))
                    {
                        IsMinorGallery = !IsMinorGallery;
                        continue;
                    }
                    else if (result.Contains("알 수 없는 갤러리 페이지입니다"))
                    {
                        throw new Exception("갤러리 id 잘못됨");
                    }

                    break;
                }
                catch (Exception e)
                {
                    Console.WriteLine("머냐 ㄷㄷ" + e);
                }
            }

            

            HtmlDocument htmlDocument = new HtmlDocument();
            htmlDocument.LoadHtml(result);


            if (isLatest)
            {
                if (htmlDocument.DocumentNode.SelectSingleNode("//td[@class='gall_subject']") != null)
                    return int.Parse(htmlDocument.DocumentNode.SelectNodes("//td[@class='gall_subject']").First(node => node.InnerText != "공지" && node.InnerText != "이슈").SelectSingleNode("../td[@class='gall_num']").InnerText);
                else
                    return int.Parse(htmlDocument.DocumentNode.SelectNodes("//td[@class='gall_num']").First(node => int.TryParse(node.InnerText, out int parseResult)).InnerText);
            }
            else
                return int.Parse(htmlDocument.DocumentNode.SelectNodes("//td[@class='gall_num']").Last().InnerText);
        }

        public async Task WriteComment(string galleryName, string articleNumber, string nickname, string password, string content)
        {
            string result = default;
            var htmlDocument = new HtmlDocument();

            httpClient.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.110 Whale/1.4.64.6 Safari/537.36");

            if (!IsMinorGallery.HasValue)
                IsMinorGallery = false;

            for(int i = 0; i < 2; ++i)
            {
                var responseMessage = await httpClient.GetAsync($"https://gall.dcinside.com/{(IsMinorGallery.Value ? "mgallery/" : "")}board/view?id={galleryName}&no={articleNumber}");
                result = await responseMessage.Content.ReadAsStringAsync();

                if (responseMessage.StatusCode == HttpStatusCode.NotFound)
                {
                    Console.WriteLine("article deleted");
                    return;
                }

                if (result.Length < 200 && result.Contains("location.replace"))
                {
                    if (i == 1)
                        throw new Exception("먼가 이상함");

                    IsMinorGallery = !IsMinorGallery;
                    await Task.Delay(1500);
                    continue;
                }
                else
                    break;
            } 

            htmlDocument.LoadHtml(result);
            string serviceCode = htmlDocument.DocumentNode.SelectSingleNode("//input[@name='service_code']").Attributes["value"].Value;

            var match = Regex.Match(result, @"_d\('(.+)'\)");
            if (!match.Success)
                throw new Exception("service_code failed");

            string[] key = Decode(match.Groups[1].Value, true).Split(',');

            string code = default;

            for (int i = 0; i < key.Length; ++i)
                code += Encoding.Default.GetString(new[] { (byte)(2 * (Convert.ToDouble(key[i]) - i - 1) / (12 - i)) });

            for (int i = 6; i <= 9; ++i)
                testValues[$"check_{i}"] = htmlDocument.DocumentNode.SelectSingleNode($"//input[@name='check_{i}']").Attributes["value"].Value;
            testValues["cur_t"] = htmlDocument.DocumentNode.SelectSingleNode("//input[@name='cur_t']").Attributes["value"].Value;
            testValues["service_code"] = Regex.Replace(serviceCode, ".{10}$", code);

            httpClient.DefaultRequestHeaders.TryAddWithoutValidation("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
            httpClient.DefaultRequestHeaders.Add("Referer", $"https://gall.dcinside.com/{(IsMinorGallery.Value ? "mgallery/" : "")}board/view/?id={galleryName}&no={articleNumber}&page=1");
            httpClient.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.110 Whale/1.4.64.6 Safari/537.36");
            httpClient.DefaultRequestHeaders.Add("X-Requested-With", "XMLHttpRequest");


            var formData = new Dictionary<string, string>
            {
                ["id"] = galleryName,
                ["no"] = articleNumber,
                ["name"] = nickname,
                ["password"] = password,
                ["memo"] = content,
                ["cur_t"] = testValues["cur_t"],
                ["check_6"] = testValues["check_6"],
                ["check_7"] = testValues["check_7"],
                ["check_8"] = testValues["check_8"],
                ["check_9"] = testValues["check_9"],
                ["recommend"] = "K",
                ["user_ip"] = "59.8.178.110",
                ["t_vch2"] = "",
                ["service_code"] = testValues["service_code"]
            };


            result = await (await httpClient.PostAsync("https://gall.dcinside.com/board/forms/comment_submit", new FormUrlEncodedContent(formData))).Content.ReadAsStringAsync();
            
            if (result.Contains("False"))
                throw new Exception("commenting failed");

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
                    int ch = int.Parse(result.Substring(0, 1));
                    result = Regex.Replace(result, "^.", (ch > 5 ? ch - 5 : ch + 4).ToString());
                }
                return result;
            }
        }


        public async Task<string[]> GetComments(string galleryName, int articleNumber)
        {
            HttpResponseMessage responseMessage = null;

            httpClient.DefaultRequestHeaders.TryAddWithoutValidation("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"); //컨텐츠타입세팅이안됨
            httpClient.DefaultRequestHeaders.Add("Host", "gall.dcinside.com");
            httpClient.DefaultRequestHeaders.Add("Origin", "https://gall.dcinside.com");
            httpClient.DefaultRequestHeaders.Add("Referer", $"https://gall.dcinside.com/{(IsMinorGallery.Value ? "mgallery/" : "")}board/view/?id={galleryName}&no={articleNumber}&page=1");
            httpClient.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.110 Whale/1.4.64.6 Safari/537.36");
            httpClient.DefaultRequestHeaders.Add("X-Requested-With", "XMLHttpRequest");

            string nickname = default, content = default;
            var comments = new List<string>();
            var formData = new Dictionary<string, string>
            {
                ["id"] = galleryName,
                ["no"] = articleNumber.ToString(),
                ["cmt_id"] = galleryName,
                ["cmt_no"] = articleNumber.ToString(),
                ["e_s_n_o"] = testValues["e_s_n_o"],
                ["comment_page"] = "1",
                ["sort"] = ""
            };

            try
            {
                responseMessage = await httpClient.PostAsync("https://gall.dcinside.com/board/comment/", new FormUrlEncodedContent(formData));
            }
            catch
            {
            }

            var result = await responseMessage.Content.ReadAsStringAsync();

            foreach (var commentsToken in JObject.Parse(result)["comments"].Children())
            {
                foreach (var commentElement in JObject.Parse(commentsToken.ToString()))
                {
                    if (commentElement.Key == "nicktype" && commentElement.Value.ToString() == "COMMENT_BOY")
                        break;
                    if (commentElement.Key == "name")
                        nickname = commentElement.Value.ToString();
                    else if (commentElement.Key == "memo")
                    {
                        if (commentElement.Value.ToString().Contains("<img") && commentElement.Value.ToString().Contains("src"))
                            goto EXIT;
                        content = commentElement.Value.ToString();
                    }

                }
                comments.Add($"{nickname}: {content}");
            EXIT:;
            }

            return comments.ToArray();
        }

        public async Task<(string nickname, string title, string content)> ParseArticle(string galleryName, int articleNumber, bool savesImages)
        {
            string nickname, title, content, result = default;
            HttpResponseMessage responseMessage = default;

            while (true)
            {

                try
                {
                    responseMessage = await httpClient.GetAsync($"https://gall.dcinside.com/{(IsMinorGallery.Value ? "mgallery/" : "")}board/view/?id={galleryName}&no={articleNumber}");
                    result = await responseMessage.Content.ReadAsStringAsync();

                    if (string.IsNullOrWhiteSpace(result))
                    {
                        Console.WriteLine("빔" + articleNumber.ToString());
                        await WaitFor(90);
                        continue;
                    }
                    if (responseMessage.StatusCode == HttpStatusCode.NotFound)
                        return default; //if (result == default)
                    else
                        break;
                }
                catch (TaskCanceledException e)
                {
                    Console.WriteLine("타임아웃ㄷ" + e);
                    return default;
                }
                catch (Exception)
                {
                    throw;
                }
            }


            HtmlDocument htmlDocument = new HtmlDocument();

            htmlDocument.LoadHtml(result);

            title = htmlDocument.DocumentNode.SelectSingleNode(@"//span[@class='title_subject']").InnerText;
            nickname = htmlDocument.DocumentNode.SelectSingleNode("//div[@class='gall_writer ub-writer']").Attributes["data-nick"].Value;

            testValues["e_s_n_o"] = htmlDocument.DocumentNode.SelectSingleNode("//input[@id='e_s_n_o']").Attributes["value"].Value;

            HtmlNode htmlNode = htmlDocument.DocumentNode.SelectSingleNode("//div[@class='writing_view_box']/div[@style='overflow:hidden;']");

            content = htmlNode.InnerHtml;
            var imageNodes = htmlDocument.DocumentNode.SelectNodes("//div[@class='appending_file_box']/ul[@class='appending_file']//a");

            if (savesImages && imageNodes != null)
            {

                string[] array = { title, content };
                string[] keywords = { "퍄", "오우야", "ㅗㅜㅑ", "야짤", "머꼴", "개꼴", "후방", "조공", "달림", "달린다" };

                /*bool abc = keywords.Any(array.Contains);
                if (abc)
                {
                    Console.WriteLine("응 맞음");
                }
                else
                    Console.WriteLine("아님");*/


                title = ReplaceInvalidFileNameCharacters(Path.GetInvalidPathChars(), title);



                if (string.IsNullOrWhiteSpace(title.ToString()))
                    title = Path.GetRandomFileName().Split('.')[0];

                Directory.CreateDirectory($@"D:\이미지\{DateTime.Now.ToShortDateString()}");
                
                _ = Task.Factory.StartNew(() => Parallel.ForEach(imageNodes, async (imageNode) =>
                {
                    HttpClientHandler imageDownloadClientHandler = new HttpClientHandler() { AutomaticDecompression = DecompressionMethods.GZip | DecompressionMethods.Deflate };
                    HttpClient imageDownloadClient = new HttpClient(imageDownloadClientHandler) { Timeout = TimeSpan.FromSeconds(10) };

                    imageDownloadClient.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.110 Whale/1.4.64.6 Safari/537.36");
                    imageDownloadClient.DefaultRequestHeaders.Add("Referer", $"https://gall.dcinside.com/{(IsMinorGallery.Value ? "mgallery/" : "")}board/view/?id={galleryName}&no={articleNumber}");
                    imageDownloadClient.DefaultRequestHeaders.Add("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8");
                    imageDownloadClient.DefaultRequestHeaders.Add("Accept-Encoding", "gzip, deflate, br");
                    imageDownloadClient.DefaultRequestHeaders.Add("Accept-Language", "ko-KR,ko;q=0.9,en-US;q=0.8,en;q=0.7");

                    try
                    {
                        responseMessage = await imageDownloadClient.GetAsync(imageNode.Attributes["href"].Value);

                        string fileName = responseMessage.Content.Headers.ContentDisposition?.FileName;

                        if (string.IsNullOrEmpty(fileName))
                        {
                            //파일명 받아올 때 피들러 켜져있으면 인코딩 깨져서 옴 주의!
                            if (responseMessage.Content.Headers.TryGetValues("Content-Disposition", out var headers))
                            {
                                fileName = headers.First().Split(new[] { "filename=" }, StringSplitOptions.None)[1];

                                if (string.IsNullOrEmpty(fileName))
                                    fileName = $"{Environment.TickCount}.png";
                                else
                                    fileName = Encoding.Default.GetString(Encoding.GetEncoding("ISO-8859-1").GetBytes(fileName));
                            }
                            else if (string.IsNullOrEmpty(fileName))
                                fileName = $"{Environment.TickCount}.png";
                        }
                        if (fileName.Length > 120)
                        {
                            int startIndex = fileName.LastIndexOf('.');
                            string fileExtension = fileName.Substring(startIndex, fileName.Length - startIndex);
                            fileName = fileName.Substring(0, 120) + fileExtension;
                        }

                        fileName = ReplaceInvalidFileNameCharacters(Path.GetInvalidFileNameChars(), fileName);

                        string path = $@"D:\이미지\{DateTime.Now.ToShortDateString()}\{fileName}";

                        using var fs = new FileStream(path, FileMode.Create, FileAccess.ReadWrite, FileShare.Read);
                        await responseMessage.Content.CopyToAsync(fs);
                        fs.Position = 0;
                        int length = (int)new FileInfo(path).Length;
                        var tuple = (ComputeSHA256Hash(fs), length);
                        int idx = imageBlacklist.BinarySearch(tuple, new MyComparer());

                        if (idx >= 0)
                        {
                            fs.Close();
                            File.Delete(path);
                            return;
                        }
                        else
                            Console.WriteLine("blocked");
                    }
                    catch (TaskCanceledException e)
                    {
                        Console.WriteLine("타임아웃" + e);
                        return;
                    }
                    catch (IOException)
                    {

                    }
                    catch (HttpRequestException)
                    {

                    }


                }));
            }


            content = Regex.Replace(htmlNode.InnerHtml.Replace("<br>", "\r\n"), "<.*?>", string.Empty).Trim();

            return (nickname, title, content);

            static string ComputeSHA256Hash(FileStream fileStream)
            {
                using SHA256 sha256 = SHA256.Create();
                return Convert.ToBase64String(sha256.ComputeHash(fileStream));
            }
            string ReplaceInvalidFileNameCharacters(char[] invalidCharacters, string folderName)
            {
                Array.Resize(ref invalidCharacters, invalidCharacters.Length + 3);
                invalidCharacters[invalidCharacters.Length - 3] = '?';
                invalidCharacters[invalidCharacters.Length - 2] = ':';
                invalidCharacters[invalidCharacters.Length - 1] = '\\';
                Array.ForEach(invalidCharacters, c => folderName = folderName.Replace(c, '_'));

                return folderName;
            }
        }



        private async Task WaitFor(int second)
        {
            for (int i = 1; i <= second; ++i)
            {
                await Task.Delay(1000);
                Console.WriteLine($"대기중... ({(int)((double)i / second * 100)}%/100%)");

            }
        }

        private class MyComparer : IComparer<(string Hash, int Size)>
        {
            public int Compare((string Hash, int Size) t1, (string Hash, int Size) t2)
            {
                if (t1.Size == t2.Size)
                    return t1.Hash.CompareTo(t2.Hash);

                return t1.Size.CompareTo(t2.Size);
            }
        }
    }
}

