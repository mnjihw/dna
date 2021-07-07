using CrawlerHelper.Http;
using CrawlerHelper.Util;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Web;

namespace TodayHumorCrawler
{
    public class TodayHumorCrawler
    {
        private static Lazy<TodayHumorCrawler> InstanceHolder { get; } = new Lazy<TodayHumorCrawler>(() => new TodayHumorCrawler());
        public static HttpManager HttpManager { get; set; }
        public string UserAgent => "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.121 Safari/537.36 Edg/85.0.564.63";
        private TodayHumorCrawler() { }
        public static TodayHumorCrawler GetInstance(HttpManager httpManager)
        {
            HttpManager = httpManager;
            return InstanceHolder.Value;
        }

        private async Task<HttpResponseMessage> BufferGetAsync(string uri)
        {
            try
            {
                return await HttpManager.HttpClient.GetAsync(HttpUtility.HtmlDecode(uri));
            }
            catch
            {
                return null;
            }
        }
        public async Task<(bool succeeded, string title, string content)> GetPost(long postId, List<(string fileName, Stream stream)> imageDatas)
        {
            if (!await HttpManager.GetHtmlFromAsync($"http://www.todayhumor.co.kr/board/view.php?table=bestofbest&no={postId}")) 
                return (false, "1", "");

            var match = Regex.Match(HttpManager.HtmlNode.SelectSingleNode("//div[@class='writerInfoContents']/div[7]").InnerText, "베오베  등록시간 : (.+)");

            if ((DateTime.Parse(match.Groups[1].Value) - DateTime.Now).Duration().Hours >= 24)
                return (false, "2", "");

            var title = HttpManager.HtmlNode.SelectSingleNode("//div[@class='viewSubjectDiv']/div").InnerText.Trim();
            var contentNode = HttpManager.HtmlNode.SelectSingleNode("//div[@class='viewContent']");
            if (contentNode == null)
                return (false, "1", ""); //블라인드 처리된 글

            if (ContainsForbiddenWords(title) || ContainsForbiddenWords(contentNode.InnerText))
                return (false, "3", "");

            var nodes = contentNode.SelectNodes(".//embed");
            if (nodes != null)
            {
                for (int i = 0; i < nodes.Count; ++i)
                {
                    var newNode = nodes[i].CloneNode("iframe");
                    nodes[i].ParentNode.ReplaceChild(newNode, nodes[i]);
                }
            }

            nodes = contentNode.SelectNodes(".//script");
            if (nodes != null)
            {
                for (int i = nodes.Count - 1; i >= 0; --i)
                {
                    nodes[i].Remove(); //원본 HtmlNode에서 사라짐
                    nodes.RemoveAt(i); //nodes에서 사라짐
                }
            }

            var imageNodes = contentNode.SelectNodes(".//img"); //미리보기용 이미지 2개만 가져옴

            if (imageNodes == null)
                return (false, "4", "");

            for (int i = imageNodes.Count - 1; i >= 0; --i)
            {
                if (i >= 2) //미리보기용 이미지 2개만 가져옴
                {
                    imageNodes[i].Remove();
                    imageNodes.RemoveAt(i);
                }
            }

            HttpManager.HttpClient.DefaultRequestHeaders.Clear();
            HttpManager.HttpClient.DefaultRequestHeaders.Add("Referer", "http://www.todayhumor.co.kr/");

            var httpResponseMessagesTasks = new List<Task<HttpResponseMessage>>(imageNodes.Select(node => BufferGetAsync(node.Attributes["src"].Value)));


            await Task.WhenAll(httpResponseMessagesTasks);


            if (httpResponseMessagesTasks.Count != 0)
            {
                for (int i = httpResponseMessagesTasks.Count - 1; i >= 0; --i)
                {
                    HttpResponseMessage message;
                    try
                    {
                        message = await httpResponseMessagesTasks[i];
                    }
                    catch (TaskCanceledException)
                    {
                        imageNodes[i].Remove();
                        imageNodes.RemoveAt(i);
                        continue;
                    }
                    if (message == null || message?.StatusCode != HttpStatusCode.OK) //이미지 읽어올 때 예외 생겼으면 이미지 포기함
                    {
                        imageNodes[i].Remove();
                        imageNodes.RemoveAt(i);
                        continue;
                    }


                    string extension;

                    var mediaType = message.Content.Headers.ContentType.MediaType;
                    if (Regex.IsMatch(mediaType, "(jpg|jpeg)", RegexOptions.IgnoreCase))
                        extension = ".jpg";
                    else if (Regex.IsMatch(mediaType, "png", RegexOptions.IgnoreCase))
                        extension = ".png";
                    else if (Regex.IsMatch(mediaType, "gif", RegexOptions.IgnoreCase))
                        extension = ".gif";
                    else if (Regex.IsMatch(mediaType, "webp", RegexOptions.IgnoreCase))
                        extension = ".webp";
                    else if (Regex.IsMatch(mediaType, "bmp", RegexOptions.IgnoreCase))
                    {
                        imageNodes[i].Remove();
                        imageNodes.RemoveAt(i);
                        continue;
                    }
                    else
                    {
                        await Logger.SendTelegramMessageAsync($"{postId}에서 {mediaType} 나옴");

                        return default;
                    }


                    var stream = await message.Content.ReadAsStreamAsync();


                    imageNodes[i].Attributes["src"].Value = $"{i}{extension}";
                    imageNodes[i].Attributes.Remove("onclick");
                    imageNodes[i].Attributes.Remove("alt");
                    imageDatas.Add(($"{i}{extension}", stream));
                }
            }

            int contentLength = 0;
            const int maxContentLength = 400;
            bool skip = false;

            nodes = contentNode.SelectNodes("*");

            for (int i = nodes.Count - 1; i >= 0; --i)
            {
                var textNode = nodes[i].SelectSingleNode(".//text()");

                if (!skip && textNode != null && contentLength + textNode.InnerHtml.Length > maxContentLength)
                {
                    textNode.InnerHtml = textNode.InnerHtml[..(maxContentLength - contentLength)];
                    skip = true;
                }
                else if (skip && !nodes[i].OuterHtml.Contains("img"))
                {
                    nodes[i].Remove();
                    nodes.RemoveAt(i);
                }

                if (textNode != null)
                    contentLength += textNode.InnerHtml.Trim().Length;

            }

            return (true, title, contentNode.InnerHtml);
        }
        public async Task<IEnumerable<long>> GetPostIdsAsync(int page)
        {
            var list = new List<(long postId, int viewCount, int recommendationCount)>();
            if (!await HttpManager.GetHtmlFromAsync($"http://www.todayhumor.co.kr/board/list.php?table=bestofbest&page={page}"))
                return new List<long>();

            foreach (var node in HttpManager.HtmlNode.SelectNodes("//table[@class='table_list']/tbody/tr[@class='view list_tr_humordata']"))
            {
                var subjectNode = node.SelectSingleNode("td[@class='subject']");

                if (subjectNode.SelectSingleNode("span/img[contains(@src, 'list_icon_photo')]") == null)
                    continue;


                var match = Regex.Match(subjectNode.SelectSingleNode("a").Attributes["href"].Value, @"s_no=(\d+)");
                var postId = long.Parse(match.Groups[1].Value);
                var recommendationCount = int.Parse(node.SelectSingleNode("td[@class='oknok']").InnerText);
                var viewCount = int.Parse(node.SelectSingleNode("td[@class='hits']").InnerText);

                if (viewCount < 2000)
                    continue;

                list.Add((postId, viewCount, recommendationCount));
            }

            return list.OrderByDescending(t => t.viewCount).ThenByDescending(t => t.recommendationCount).Take(list.Count / 3).Select(t => t.postId);
        }

        private bool ContainsForbiddenWords(string str)
        {
            var forbiddenWords = new[] { "에미", "애미", "새끼", "세끼", "새기", "게이", "존나", "졷나", "좆", "졷", "시발", "씨발", "파딱", "죽었", "뒤졌", "뒤진", "디진", "디졌", "병신", "븅신", "자살", "붕이", "이기" };

            return forbiddenWords.Any(word => str.Contains(word));
        }
    }
}
