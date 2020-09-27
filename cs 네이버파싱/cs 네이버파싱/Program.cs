using MSScriptControl;
using System;
using System.IO;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Net.Http;
using System.Collections.Generic;
using System.Globalization;

namespace cs_네이버파싱
{


    class Program
    {

        [DllImport("Kernel32")]
        static extern bool SetConsoleCtrlHandler(HandlerRoutine handler, bool add);
        delegate bool HandlerRoutine(CtrlTypes CtrlTypes);


        enum CtrlTypes
        {
            CTRL_C_EVENT = 0,
            CTRL_BREAK_EVENT = 1,
            CTRL_CLOSE_EVENT = 2,
            CTRL_LOGOFF_EVENT = 5,
            CTRL_SHUTDOWN_EVENT = 6
        }


        struct foo_t
        {
            public string blogId;
            public string logNo;
        }

        public struct point_out_t
        {
            public string wrongstring;
            public string correctstring;
            public string explanation;
        }


     
        static string title, content;
        public const int PS_MAX = 512;
        public static point_out_t[] ps = new point_out_t[PS_MAX];
        static foo_t[] foo = new foo_t[10];
        static HttpWebRequest request;
        static CookieContainer cookie = new CookieContainer();
        static int comment_idx, overlapped;
        static StreamWriter sw;

        static char[] CHOSUNG_LIST = new char[] { 'ㄱ', 'ㄲ', 'ㄴ', 'ㄷ', 'ㄸ', 'ㄹ', 'ㅁ', 'ㅂ', 'ㅃ', 'ㅅ', 'ㅆ', 'ㅇ', 'ㅈ', 'ㅉ', 'ㅊ', 'ㅋ', 'ㅌ', 'ㅍ', 'ㅎ' };

        static char[] JUNGSUNG_LIST = new char[] { 'ㅏ', 'ㅐ', 'ㅑ', 'ㅒ', 'ㅓ', 'ㅔ', 'ㅕ', 'ㅖ', 'ㅗ', 'ㅘ', 'ㅙ', 'ㅚ', 'ㅛ', 'ㅜ', 'ㅝ', 'ㅞ', 'ㅟ', 'ㅠ', 'ㅡ', 'ㅢ', 'ㅣ' };

        static char[] JONGSUNG_LIST = new char[] { ' ', 'ㄱ', 'ㄲ', 'ㄳ', 'ㄴ', 'ㄵ', 'ㄶ', 'ㄷ', 'ㄹ', 'ㄺ', 'ㄻ', 'ㄼ', 'ㄽ', 'ㄾ', 'ㄿ', 'ㅀ', 'ㅁ', 'ㅂ', 'ㅄ', 'ㅅ', 'ㅆ', 'ㅇ', 'ㅈ', 'ㅊ', 'ㅋ', 'ㅌ', 'ㅍ', 'ㅎ' };


        struct hangul_t
        {
            public int f;
            public int m;
            public int l;
        }

        static void hangul_disassemble(char ch, ref hangul_t param)
        {
            if (ch >= 0xAC00 && ch <= 0xD7A3)
            {
                int ch2 = (int)ch - 0xAC00;
                param.l = ch2 % 28;
                ch2 /= 28;
                param.m = ch2 % 21;
                param.f = ch2 / 21;
            }
            else
            {
                param.f = -1;
                param.m = -1;
                param.l = -1;
            }
        }

        static char hangul_assemble(hangul_t param)
        {
            return (char)(0xAC00 + ((param.f * 21) + param.m) * 28 + param.l);
        }

        static bool _NaverLogin(string id, string password)
        {
            ScriptControl sc = new ScriptControl();
            string[] buf;
            string SessionKey, keyName, eValue, nValue, data, result;
            byte[] sendData;
            HttpClient client;
            HttpClientHandler handler = new HttpClientHandler();
            HttpContent querystring;
            handler.CookieContainer = cookie;

            client = new HttpClient(handler);

            result = client.GetAsync("http://static.nid.naver.com/enclogin/keys.nhn").Result.Content.ReadAsStringAsync().Result;


            buf = result.Split(',');
            SessionKey = buf[0];
            keyName = buf[1];
            eValue = buf[2];
            nValue = buf[3];


            sc.Language = "JavaScript";
            sc.Reset();
            sc.AddCode(File.ReadAllText("C:\\Users\\지환\\Documents\\visual studio 2015\\Projects\\cs 네이버파싱\\cs 네이버파싱\\rsa.js"));
            //data = "encpt=1&encpw=" + sc.Run("createRsaKey", id, password, SessionKey, keyName, eValue, nValue) + "&encnm=" + keyName + "&svctype=0&url=http://naver.com&enc_url=http%3A%2F%2Fwww.naver.com%2F&smart_level=-1";


            client.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.84 Safari/537.36");
            client.DefaultRequestHeaders.TryAddWithoutValidation("Content-Type", "application/x-www-form-urlencoded");
            querystring = new FormUrlEncodedContent(new Dictionary<string, string> { { "encpt", "1" }, { "encpw", sc.Run("createRsaKey", id, password, SessionKey, keyName, eValue, nValue) }, { "encnm", keyName }, { "svctype", "0" }, { "url", "http://naver.com" }, { "enc_url", "http%3A%2F%2Fwww.naver.com%2F" }, { "smart_level", "-1" } });

            result = client.PostAsync("https://nid.naver.com/nidlogin.login", querystring).Result.Content.ReadAsStringAsync().Result;


            if (result.Contains("https://nid.naver.com/login/sso/finalize.nhn?url"))
                return true;
            else
                return false;
        }

        static bool NaverLogin(string id, string password)
        {
            string[] buf;
            string SessionKey, keyName, eValue, nValue, data, result;
            byte[] sendData;
            ScriptControl sc = new ScriptControl();
            HttpWebResponse response;
            Stream requestStream;
            StreamReader reader;

            request = (HttpWebRequest)WebRequest.Create("http://static.nid.naver.com/enclogin/keys.nhn");
            request.Method = "GET";
            request.CookieContainer = cookie;

            response = (HttpWebResponse)request.GetResponse();
            requestStream = response.GetResponseStream();
            reader = new StreamReader(requestStream);
            result = reader.ReadToEnd();
            reader.Close();
            requestStream.Close();
            response.Close();
            
            if(response.StatusCode != HttpStatusCode.OK)
            {
                Console.WriteLine("status is not OK");
                return false;
            }


            buf = result.Split(',');
            SessionKey = buf[0];
            keyName = buf[1];
            eValue = buf[2];
            nValue = buf[3];
            

            sc.Language = "JavaScript";
            sc.Reset();
            sc.AddCode(File.ReadAllText("C:\\Users\\지환\\Documents\\visual studio 2015\\Projects\\cs 네이버파싱\\cs 네이버파싱\\rsa.js"));

            data = "encpt=1&encpw=" + sc.Run("createRsaKey", id, password, SessionKey, keyName, eValue, nValue) + "&encnm=" + keyName + "&svctype=0&url=http://naver.com&enc_url=http%3A%2F%2Fwww.naver.com%2F&smart_level=-1";

            sendData = Encoding.Default.GetBytes(data);

            request = (HttpWebRequest)WebRequest.Create("https://nid.naver.com/nidlogin.login");
            request.Method = "POST";
            request.CookieContainer = cookie;
            request.UserAgent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.84 Safari/537.36";
            request.ContentType = "application/x-www-form-urlencoded";
            request.ContentLength = sendData.Length;
            

            requestStream = request.GetRequestStream();
            requestStream.Write(sendData, 0, sendData.Length);
            requestStream.Close(); 

            response = (HttpWebResponse)request.GetResponse();
            requestStream = response.GetResponseStream();
            reader = new StreamReader(requestStream);

            result = reader.ReadToEnd();
            
            reader.Close();
            requestStream.Close();
            response.Close();


            if (result.Contains("https://nid.naver.com/login/sso/finalize.nhn?url"))
                return true;
            else
                return false;

        }

        static void WriteComment(string blogId, string logNo, string commentNo, string id, string nick, string content)
        {
            string result, data;
            byte[] sendData;
            ScriptControl sc = new ScriptControl();
            HttpWebResponse response;
            Stream requestStream;
            StreamReader reader;
            
            data = "blogId=" + blogId + "&logNo=" + logNo + "&comment.emoticon=1045017&comment.parentCommentNo=" + commentNo + "&isMemolog=false&currenrPage=&commentProfileImageType=3&comment.imageType=1" + (commentNo == "0" ? "" : "&comment.toBlogId=" + id) + (commentNo == "0" ? "" : "&comment.toNickname=" + nick) + "&shortestContentAreaWidth=false&captchaKey=&captchaValue=&comment.stickerCode=&comment.contents=" + content + "&comment.secretYn=false";
            sendData = Encoding.Default.GetBytes(data);

            request = (HttpWebRequest)WebRequest.Create("http://blog.naver.com/CommentWrite.nhn");
            request.Method = "POST";
            request.CookieContainer = cookie;
            request.UserAgent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.84 Safari/537.36";
            request.ContentType = "application/x-www-form-urlencoded";
            request.ContentLength = sendData.Length;
            request.Referer = "http://blog.naver.com/CommentList.nhn?blogId=" + blogId + "&logNo=" + logNo + "&currentPage=&isMemolog=false&focusingCommentNo=&showLastPage=true&shortestContentAreaWidth=false";

            requestStream = request.GetRequestStream();
            requestStream.Write(sendData, 0, sendData.Length);
            requestStream.Close();

            response = (HttpWebResponse)request.GetResponse();
            requestStream = response.GetResponseStream();
            reader = new StreamReader(requestStream, Encoding.GetEncoding("ks_c_5601-1987"));

            result = reader.ReadToEnd();

            reader.Close();
            requestStream.Close();
            response.Close();

            if(result.Contains("댓글 등록 제한을 초과해"))
            {
                Console.WriteLine("댓글 등록 제한 초과함");
                Environment.Exit(0);
            }
        }


        static void Search(string keyword)
        {
            string result, buf = null;
            ScriptControl sc = new ScriptControl();
            HttpWebResponse response;
            Stream requestStream;
            StreamReader reader;
            int start, end;
            int idx;

            for (idx = 1;; idx += 10)
            {
                request = (HttpWebRequest)WebRequest.Create("https://search.naver.com/search.naver?sm=tab_hty.top&where=post&oquery=&ie=utf8&query=" + keyword + "&start=" + idx);

                request.Method = "GET";
                request.UserAgent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.84 Safari/537.36";
                request.CookieContainer = cookie;


                response = (HttpWebResponse)request.GetResponse();
                requestStream = response.GetResponseStream();
                reader = new StreamReader(requestStream);
                result = reader.ReadToEnd();
                reader.Close();
                requestStream.Close();
                response.Close();

                start = result.LastIndexOf("<div class=\"blog section _blogBase");
                end = result.LastIndexOf("aSlidingThumbnailData");
                result = result.Substring(start, end - start);

                
                start = 0;

                for (int i = 0;; ++i)
                {
                    for (int j = 0; j < 1; ++j)
                    {
                        if ((start = result.IndexOf("<a href=\"http://", start + 1)) == -1)
                        {
                            i = 777;
                            break;
                        }
                        end = result.IndexOf("\"", start + 9);

                        buf = result.Substring(start, end - start);

                        if (buf.Contains("section.blog.naver.com") || (!buf.Contains("blog.naver.com") && !buf.Contains("blog.me")))
                        {
                            --j;
                            continue;
                        }

                    }
                    if (i == 777)
                        break;
                   

                    if (buf.Contains("blog.me"))
                    {
                        buf = result.Substring(start + 16, end - start - 16);
                        
                        if (!buf.Contains("/") || i >= 1 && buf.Substring(0, buf.IndexOf(".blog.me")) == foo[i - 1].blogId)
                        {
                            --i;
                            continue;
                        }
                        
                        foo[i].blogId = buf.Substring(0, buf.IndexOf(".blog.me"));
                        foo[i].logNo = buf.Substring(buf.IndexOf("blog.me") + 8, buf.Length - buf.IndexOf("blog.me") - 8);
                    }
                    else
                    {
                        buf = result.Substring(start + 31, end - start - 31);
                        
                        if (!buf.Contains("?") || (i >= 1 && buf.Substring(0, buf.IndexOf("?")) == foo[i - 1].blogId))
                        {
                            --i;
                            continue;
                        }

                        foo[i].blogId = buf.Substring(0, buf.IndexOf('?'));
                        foo[i].logNo = buf.Substring(buf.IndexOf("logNo=") + 6, buf.Length - buf.IndexOf("logNo=") - 6);

                    }
                }

                for (int i = 0; i < foo.Length && !string.IsNullOrEmpty(foo[i].logNo); ++i)
                    point_out(foo[i].blogId, foo[i].logNo);
            }
        }

        


        static void GetArticleInfo(string blogId, string logNo)
        {
            int start, end;
            string result;
            HttpWebResponse response;
            Stream requestStream;
            StreamReader reader;

            
            request = (HttpWebRequest)WebRequest.Create("http://blog.naver.com/PostView.nhn?blogId=" + blogId + "&logNo=" + logNo + "&beginTime=0&jumpingVid=&from=search&redirect=Log&widgetTypeCall=true");

            request.Method = "GET";
            request.UserAgent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.84 Safari/537.36";
            request.CookieContainer = cookie;

            response = (HttpWebResponse)request.GetResponse();
            requestStream = response.GetResponseStream();
            reader = new StreamReader(requestStream, Encoding.GetEncoding("ks_c_5601-1987"));


            result = reader.ReadToEnd();

            reader.Close();
            requestStream.Close();
            response.Close();

            if (result.Contains("삭제되었거나 존재하지 않는 게시물입니다.") || result.Contains("글쓰기 제한기능을 사용하고 있습니다") || result.Substring(result.IndexOf("forwardWriteForm = \"") + 20, 4) == "none")
                return;

            start = result.IndexOf("og:title\" content=\"");
            end = result.IndexOf("\"", start + 19);
            title = result.Substring(start + 19, end - start - 19);


            start = result.LastIndexOf("post-view");
            end = result.LastIndexOf("post_footer_contents");
            content = result.Substring(start, end - start);
        }

        struct comment_t
        {
            public string comment;
            public string commentNo;
            public string id;
            public string nick;
            public string targetId;

        }

        static comment_t[] ct = new comment_t[1024];

        static void GetComments(string blogId, string logNo)
        {
            int start, end, tempidx, tempidx2, currentPage = 1, i;
            string result, temp;
            ScriptControl sc = new ScriptControl();
            HttpWebResponse response;
            Stream requestStream;
            StreamReader reader;

            comment_idx = 0;
            Array.Clear(ct, 0, ct.Length);
            i = 0;
            
            while (true)
            {
                request = (HttpWebRequest)WebRequest.Create("http://blog.naver.com/CommentList.nhn?blogId=" + blogId + "&logNo=" + logNo + "&currentPage=" + currentPage++ + "&isMemolog=false&focusingCommentNo=&shortestContentAreaWidth=false&isAfterWrite=true");
                request.Method = "GET";
                request.CookieContainer = cookie;

                response = (HttpWebResponse)request.GetResponse();
                requestStream = response.GetResponseStream();
                reader = new StreamReader(requestStream, Encoding.GetEncoding("ks_c_5601-1987"));
                result = reader.ReadToEnd();
                reader.Close();
                requestStream.Close();
                response.Close();

                if (response.StatusCode != HttpStatusCode.OK)
                {
                    Console.WriteLine("status is not OK");
                    return;
                }

                if (result.Contains("삭제되었거나 존재하지 않는 게시물입니다.") || result.Contains("글쓰기 제한기능을 사용하고 있습니다"))
                    return;
                if (result.Substring(result.IndexOf("forwardWriteForm = \"") + 20, 4) == "none")
                {
                    title = "";
                    content = "";
                    return;
                }

                start = result.IndexOf("postComment_");
                if (start == -1)
                    return;

                end = result.LastIndexOf("formCommentLoginUser");

                result = result.Substring(start, end - start);
                start = -1;

                for (; (start = result.IndexOf("postComment_", start + 1)) != -1 && i < ct.Length; ++i)
                {

                    end = result.IndexOf("\"", start);
                    ct[i].commentNo = result.Substring(start + 12, end - start - 12);

                    end = result.IndexOf("postComment_", start + 1);
                    if (end == -1)
                        end = result.Length;
                    temp = result.Substring(start, end - start);

                    tempidx = start;

                    start = temp.IndexOf("ainDispatcher.nhn?blogId=" + blogId + "&id=");

                    end = temp.IndexOf("&", start + blogId.Length + 29);
                    if (start != -1)
                        ct[i].targetId = temp.Substring(start + blogId.Length + 29, end - start - blogId.Length - 29);

                    start = temp.IndexOf("value=\"");
                    if (start == -1)
                    {
                        start = tempidx;
                        --i;
                        continue;
                    }

                    end = temp.IndexOf("\"", start + 7);


                    if (string.IsNullOrEmpty(temp.Substring(start + 7, end - start - 7)))
                    {
                        start = tempidx;
                        --i;
                        continue;
                    }


                    ct[i].comment = temp.Substring(start + 7, end - start - 7).Replace("&gt;", ">");

                    tempidx2 = start;
                    start = temp.IndexOf("writerBlogId : \"", start);
                    end = temp.IndexOf("\"", start + 16);
                    if (start == -1)
                    {
                        Console.WriteLine("여기 ㅋ{0}", ct[i].comment); //여긴 매크로가 써서 아무 정보도 없는 경우에 걸림
                        Console.WriteLine("{0} {1}", blogId, logNo);
                        Console.Read();
                        --i;
                        start = tempidx;
                        continue;
                        start = temp.LastIndexOf("<a href=\"http://blog.naver.com/", tempidx2);
                        end = temp.IndexOf("\"", start + 31);
                        if (start == -1)
                        {
                            start = temp.IndexOf("&id=", start);
                            end = temp.IndexOf("&", start);
                            ct[i].id = temp.Substring(start, end - start);
                        }
                        else
                        {
                            ct[i].id = temp.Substring(start + 31, end - start - 31);
                            Console.WriteLine(ct[i].id);
                        }
                    }
                    else
                        ct[i].id = temp.Substring(start + 16, end - start - 16);

                    tempidx2 = start;
                    start = temp.IndexOf("writerNickname : \"", start);
                    end = temp.IndexOf("\"", start + 18);
                    if (start == -1)
                    {
                        start = temp.LastIndexOf("id=\"nick", tempidx2);
                        start = temp.IndexOf(">", start);
                        end = temp.IndexOf("<", start);
                        ct[i].nick = temp.Substring(start, end - start);
                    }
                    else
                        ct[i].nick = temp.Substring(start + 18, end - start - 18);
                    ++comment_idx;
                    start = tempidx;
                }
            }

        }

        static bool IsImproperString(string wrongstring, string str, int pos)
        {
            bool ret = false;
            hangul_t hangul_comb = new hangul_t();

            if (wrongstring == "왠" && str.Length > 1 && (str.Substring(pos, 2) == "왠지" || str.Substring(pos, 2) == "왠만"))
                ret = true;
            else if(wrongstring == "되도" && (str.Length > pos + 2 && str[pos + 2] != '<' && str[pos + 2] != ' ') || (str.Length > pos + 4 && str[pos + 2] == ' ' && str.Substring(pos + 3, 2) == "않는"))
                ret = true;
            else if(wrongstring == "되라" && str.Length > pos + 2 && (str.Substring(pos + 2, 1) == "는" || str.Substring(pos + 2, 1) == "능" || str.Substring(pos + 2, 1) == "고" || str.Substring(pos + 2, 1) == "더" || str.Substring(pos + 2, 1) == "던" || str.Substring(pos + 2, 1) == "지" || str.Substring(pos + 2, 1) == "네" || str.Substring(pos + 2, 1) == "니" || str.Substring(pos + 2, 1) == "하"))
                ret = true;
            else if (wrongstring == "는대" && str.Length > pos + 2 && (str.Substring(pos + 2, 1) == "로" || str.Substring(pos + 2, 1) == "신" || str.Substring(pos + 2, 1) == "?"))
                ret = true;
            else if (wrongstring == "몇일" && str.Length > pos + 2 && (str.Substring(pos + 2, 1) == "까" || str.Length > pos + 3 && str.Substring(pos + 2, 2) == "텐데"))
                ret = true;
            else if (wrongstring == "뒷" || wrongstring == "윗" || wrongstring == "아랫")
            {
                if (str.Length > pos + wrongstring.Length)
                {
                    hangul_disassemble(str[pos + wrongstring.Length], ref hangul_comb);
                    if (hangul_comb.f == -1 || hangul_comb.m == -1 || hangul_comb.l == -1)
                        ret = true;
                    else if (CHOSUNG_LIST[hangul_comb.f] != 'ㅍ' && CHOSUNG_LIST[hangul_comb.f] != 'ㅌ' && CHOSUNG_LIST[hangul_comb.f] != 'ㅊ' &&
                        CHOSUNG_LIST[hangul_comb.f] != 'ㄲ' && CHOSUNG_LIST[hangul_comb.f] != 'ㄸ' && CHOSUNG_LIST[hangul_comb.f] != 'ㅃ' &&
                        CHOSUNG_LIST[hangul_comb.f] != 'ㅆ' && CHOSUNG_LIST[hangul_comb.f] != 'ㅉ' && CHOSUNG_LIST[hangul_comb.f] != 'ㅋ')
                        ret = true; //뒤 초성이 된소리나 거센소리가 아니면 넘어감
                }
            }
            else if (wrongstring == "폐쇠" && str.Length > pos + 4 && str.Substring(pos + 2, 3) == "공포증")
                ret = true;

            return ret;
        }

    



        static void point_out(string blogId, string logNo)
        {
            int i, j, idx = 0, pos, k;
            Point_out.pointed_out_t[] pointed_out = new Point_out.pointed_out_t[80];


            GetArticleInfo(blogId, logNo);
            
            GetComments(blogId, logNo);

            if (string.IsNullOrEmpty(title))
                return;

            for(i = 0; i < Point_out.get_ps_idx(); ++i)
            {
                pos = title.IndexOf(ps[i].wrongstring);
                if (pos != -1)
                {
                    if (IsImproperString(ps[i].wrongstring, title, pos))
                        continue;

                    for (j = 0; j < comment_idx; ++j)
                    {
                        if (string.IsNullOrEmpty(ct[j].targetId) && ct[j].comment == $"{ps[i].wrongstring}->{ps[i].correctstring} {ps[i].explanation} [리듬 맞춤법 봇♬]")
                        {
                            //Console.WriteLine("이미있2 {0}", ps[i].wrongstring);
                            j = 777;
                            ++overlapped;

                            if (overlapped >= 10)
                            {
                                overlapped = 0;
                                //Console.WriteLine("넘어가자");
                                //return;
                            }
                            break;
                        }
            
                    }
                    if (j == 777)
                        continue;

                    WriteComment(blogId, logNo, "0", "", "", $"{ps[i].wrongstring}->{ps[i].correctstring} {ps[i].explanation} [리듬 맞춤법 봇♬]");
                    Console.WriteLine("{0}/{1}에 {2}-{3}에다 {4} 지적함1", blogId, logNo, "블로그주인", title.Substring(pos, title.Length - pos >= 10 ? 10 : title.Length - pos), ps[i].wrongstring); //터짐
                    sw.WriteLine(string.Format("{0}/{1}에 {2}-{3}에다 {4} 지적함1", blogId, logNo, "블로그주인", title.Substring(pos, title.Length - pos >= 10 ? 10 : title.Length - pos), ps[i].wrongstring));
                    pointed_out[idx].idx = i;
                    pointed_out[idx++].id = "1";
                }

                pos = content.IndexOf(ps[i].wrongstring);
                
                if(pos != -1)
                {
                    if (IsImproperString(ps[i].wrongstring, content, pos))
                        continue;
                    for(j = 0; j < idx; ++j)
                    {
                        if (pointed_out[j].id == "1" && i == pointed_out[j].idx)
                        {
                            //Console.WriteLine("이미있3 {0}", ps[pointed_out[j].idx].wrongstring);

                            j = 777;
                            ++overlapped;

                            if (overlapped >= 10)
                            {
                                overlapped = 0;
                                // Console.WriteLine("넘어가자");
                                //return;
                                
                            }
                            break;
                        }

                    }
                    if (j == 777)
                        continue;
                    for(j = 0; j < comment_idx; ++j)
                    {
                        if(string.IsNullOrEmpty(ct[j].targetId) && ct[j].comment == $"{ps[i].wrongstring}->{ps[i].correctstring} {ps[i].explanation} [리듬 맞춤법 봇♬]")
                        {
                            //Console.WriteLine("이미있4 {0}", ps[i].wrongstring);

                            j = 777;
                            ++overlapped;

                            if (overlapped >= 10)
                            {
                                overlapped = 0;
                                //Console.WriteLine("넘어가자");
                                //return;
                            }
                            break;
                        }
                    }
                    if (j == 777)
                        continue;

                    WriteComment(blogId, logNo, "0", "", "", $"{ps[i].wrongstring}->{ps[i].correctstring} {ps[i].explanation} [리듬 맞춤법 봇♬]");
                    pointed_out[idx].idx = i;
                    pointed_out[idx++].id = "1";
                    Console.WriteLine("{0}/{1}에 {2}-{3}에다 {4} 지적함2", blogId, logNo, "블로그주인", content.Substring(pos, content.Length - pos >= 10 ? 10 : content.Length - pos), ps[i].wrongstring);
                    sw.WriteLine(string.Format("{0}/{1}에 {2}-{3}에다 {4} 지적함2", blogId, logNo, "블로그주인", content.Substring(pos, content.Length - pos >= 10 ? 10 : content.Length - pos), ps[i].wrongstring));

                }


            }
            
            for (i = 0; i < comment_idx; ++i)
            {
                if (!string.IsNullOrEmpty(ct[i].comment) && ct[i].comment.Contains("[리듬 맞춤법 봇♬]"))
                    continue;
                for (j = 0; j < Point_out.get_ps_idx(); ++j)
                {
                    if (!string.IsNullOrEmpty(ct[i].comment) && ct[i].comment.Contains(ps[j].wrongstring))
                    {
                        for (k = 0; k < comment_idx; ++k)
                        {
                            if (!ct[k].comment.Contains("[리듬 맞춤법 봇♬]"))
                                continue;

                            if (ct[k].targetId == ct[i].id && ct[i].comment.Contains(ct[k].comment.Substring(0, ps[j].wrongstring.Length)))
                            {
                                //Console.WriteLine("내가지적한놈이네!!");
                                k = 777;
                                pointed_out[idx].idx = j;
                                pointed_out[idx++].id = ct[i].id;
                            }
                            if (k == 777)
                                break;

                        }
                    }
                }
            }

            for (i = 0; i < comment_idx; ++i)
            {
                if (ct[i].comment.Contains("[리듬 맞춤법 봇♬]"))
                    continue;

                for (j = 0; j < Point_out.get_ps_idx(); ++j) 
                {
                    pos = ct[i].comment.IndexOf(ps[j].wrongstring);
                    if (pos != -1)
                    {
                        if (IsImproperString(ps[j].wrongstring, ct[i].comment, pos))
                            continue;
                       
                        for (k = 0; k < idx; ++k)
                        {
                            if (j == pointed_out[k].idx && pointed_out[k].id == ct[i].id)
                            {
                                k = 777;
                                ++overlapped;
                                if (overlapped >= 10)
                                {
                                    overlapped = 0;
                                    //Console.WriteLine("넘어가자");
                                    //return;
                                    
                                }
                                break;
                            }
                        }
                        if (k == 777)
                            continue;
                        WriteComment(blogId, logNo, ct[i].commentNo, ct[i].id, ct[i].nick, $"{ps[j].wrongstring}->{ps[j].correctstring} {ps[j].explanation} [리듬 맞춤법 봇♬]");
                        Console.WriteLine($"{blogId}/{logNo}에 {ct[i].nick}-{ct[i].comment}에다 {ps[j].wrongstring} 지적함3");
                        sw.WriteLine($"{blogId}/{logNo}에 {ct[i].nick}-{ct[i].comment}에다 {ps[j].wrongstring} 지적함3");
                        pointed_out[idx].idx = j;
                        pointed_out[idx++].id = ct[i].id;
                    }
                }
            }
        }



        static bool CtrlHandler(CtrlTypes CtrlType)
        {
            sw.Flush();
            sw.Close();
            Console.WriteLine("Flush 호출함");
            return false;
        }
        


        static void Main(string[] args)
        {
            {
                //string a = "리듀밍";
                char[] a = Encoding.Default.GetChars(Encoding.Default.GetBytes("리듀밍"));

                for(int i = 0; i < 3; ++i)
                {
                    Console.Write("{0:X} ", Convert.ToInt32(a[i]));
                }
                return;
            }
            Point_out.ps_init();
            HandlerRoutine hr = new HandlerRoutine(CtrlHandler);

            Console.Title = "cs 네이버파싱";
            SetConsoleCtrlHandler(hr, true);
            sw = File.AppendText("C:\\Users\\지환\\Documents\\visual studio 2015\\Projects\\cs 네이버파싱\\cs 네이버파싱\\bin\\Debug\\로그.txt");

            //NaverLogin("memscan", "zxc021");
            Console.Write(_NaverLogin("memscan", "zxc021"));

            //Search("카카오톡");
            //GetComments("nitemare", "80198247261");
            



        }
    }
}
