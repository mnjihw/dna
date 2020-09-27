using System;
using System.Net;
using System.Net.Sockets;
using System.Net.Http;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.IO;
using System.Threading;

namespace 리듬_DC_맞춤법_봇
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

        public struct point_out_t
        {
            public string wrongstring;
            public string correctstring;
            public string explanation;
        }

        struct article_t
        {
            public string title;
            public string nick;
            public string content;
            public int recentnum;
        }

       

        static string[] nick = new string[1000];
        static string[] comment = new string[1000];
        public const int PS_MAX = 512; 
        public static point_out_t[] ps = new point_out_t[PS_MAX];
        static CookieContainer cookie = new CookieContainer();
        static int comment_idx, overlapped, list_idx, flip, flip2;
        static string ci_t = null;
        static StreamWriter sw;

        static char[] CHOSUNG_LIST = new char[] { 'ㄱ', 'ㄲ', 'ㄴ', 'ㄷ', 'ㄸ', 'ㄹ', 'ㅁ', 'ㅂ', 'ㅃ', 'ㅅ', 'ㅆ', 'ㅇ', 'ㅈ', 'ㅉ', 'ㅊ', 'ㅋ', 'ㅌ', 'ㅍ', 'ㅎ' };

        static char[] JUNGSUNG_LIST = new char[] { 'ㅏ', 'ㅐ', 'ㅑ', 'ㅒ', 'ㅓ', 'ㅔ', 'ㅕ', 'ㅖ', 'ㅗ', 'ㅘ', 'ㅙ', 'ㅚ', 'ㅛ', 'ㅜ', 'ㅝ', 'ㅞ', 'ㅟ', 'ㅠ', 'ㅡ', 'ㅢ', 'ㅣ' };

        static char[] JONGSUNG_LIST = new char[] { ' ', 'ㄱ', 'ㄲ', 'ㄳ', 'ㄴ', 'ㄵ', 'ㄶ', 'ㄷ', 'ㄹ', 'ㄺ', 'ㄻ', 'ㄼ', 'ㄽ', 'ㄾ', 'ㄿ', 'ㅀ', 'ㅁ', 'ㅂ', 'ㅄ', 'ㅅ', 'ㅆ', 'ㅇ', 'ㅈ', 'ㅊ', 'ㅋ', 'ㅌ', 'ㅍ', 'ㅎ' };

        static string[] gallid_list = new string[] { "programming", "pokemon", "nintendo", "iu_new", "twice", "produce101",
"baseball_new2", "lovelive", "drama_new", "etc_entertainment2", "d_fighter_new", "hanwhaeagles",
"game_classic", "giants_new1", "m_entertainer", "idolmaster", "stock_new1" };

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

 

        static async Task<string> Getci_t(string gallName)
        {
            HttpClient client;
            HttpResponseMessage response = null;
            string result;
            HttpClientHandler handler = new HttpClientHandler();
            handler.CookieContainer = cookie;
            
            client = new HttpClient(handler);
            client.Timeout = TimeSpan.FromMilliseconds(10000);

            start:
            try
            {
                response = await client.GetAsync($"http://gall.dcinside.com/board/view/?id={gallName}&no=1");
            }
            catch(Exception)
            {
                goto start;
            }
            result = response.Content.ReadAsStringAsync().Result;
            
            return result.Substring(result.IndexOf("ci_t\" value=\"") + 13, 32);
        }

        /*static void WriteComment(string gallName, string articlenum, string nick, string pw, string comment)
        {
     
            do
            {
                idx = random.Next(0, proxy_list.Length - 1);
                Console.Write("A");
            } while (!check_proxy(proxy_list[idx]));
            Console.Write(proxy_list[idx]);
            //proxy = new WebProxy(proxy_list[idx], false); //나중에 안되면 두번째 인자 포트로 바꾸기
            proxy = new WebProxy("124.88.67.21", false);


        }*/

       
        static void Connect(Socket socket, EndPoint endpoint, TimeSpan timeout)
        {
            var result = socket.BeginConnect(endpoint, null, null);

            bool success = result.AsyncWaitHandle.WaitOne(timeout, true);
            if (success)
            {
                socket.EndConnect(result);
            }
            else
            {
                socket.Close();
                throw new SocketException(10060); // Connection timed out.
            }
        }



        static bool aa(string host, int port)
        {
            var is_success = false;
            try
            {
                var connsock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                connsock.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout, 500);
                System.Threading.Thread.Sleep(500);
                var hip = IPAddress.Parse(host);
                var ipep = new IPEndPoint(hip, port);
                Connect(connsock, ipep, TimeSpan.FromMilliseconds(1000));
                if (connsock.Connected)
                    is_success = true;
                
                connsock.Close();
            }
            catch (Exception)
            {
                is_success = false;
            }
            return is_success;
        }




        static async void WriteComment(string gallName, string articlenum, string nick, string pw, string comment)
        {
            HttpContent querystring;
            HttpClientHandler handler = new HttpClientHandler();
            handler.CookieContainer = cookie;
            HttpClient client = new HttpClient(handler);

            client.DefaultRequestHeaders.Add("X-Requested-With", "XMLHttpRequest");
            client.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.93 Safari/537.36");
            client.DefaultRequestHeaders.TryAddWithoutValidation("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
            client.DefaultRequestHeaders.Add("Referer", $"http://gall.dcinside.com/board/view/?id={gallName}&no={articlenum}&page=1");
            client.Timeout = TimeSpan.FromMilliseconds(10000);

            querystring = new FormUrlEncodedContent(new Dictionary<string, string> { {"ci_t", ci_t }, {"name", nick }, {"password", pw }, {"memo", comment }, {"id", gallName }, {"no", articlenum }, {"best_orgin", "" } });
            
            start:
            try
            {
                await client.PostAsync("http://gall.dcinside.com/forms/comment_submit", querystring);
            }
            catch(Exception)
            {
                goto start;
            }   
        }

        static int GetComments(string gallName, string articlenum)
        {
            int currentPage = 1, start, end, tempidx;
            HttpContent querystring;
            HttpResponseMessage response;
            HttpClientHandler handler = new HttpClientHandler();
            string result;
            handler.CookieContainer = cookie;
            HttpClient client = new HttpClient(handler);
            client.Timeout = TimeSpan.FromMilliseconds(10000);


            comment_idx = 0;
            Array.Clear(comment, 0, comment.Length);
            // 콘앱에선 이앞에 GET으로 댓 볼 글 글삭됐는지 확인하는코드 있고 point_out에서도 호출해서 체크하는데 그거 필요하면 넣기
            while (true)
            {
                client.DefaultRequestHeaders.Clear();
                client.DefaultRequestHeaders.Add("Referer", $"http://gall.dcinside.com/board/comment_view/?id={gallName}&no={articlenum}&page=1");
                client.DefaultRequestHeaders.TryAddWithoutValidation("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
                client.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.93 Safari/537.36");
                client.DefaultRequestHeaders.Add("X-Requested-With", "XMLHttpRequest");

                querystring = new FormUrlEncodedContent(new Dictionary<string, string> { { "ci_t", ci_t }, { "id", gallName }, { "no", articlenum }, { "comment_page", currentPage++.ToString() } });

                start:
                try
                {
                    response = client.PostAsync("http://gall.dcinside.com/comment/view", querystring).Result;
                }
                catch (Exception)
                {
                    goto start;
                }
                result = response.Content.ReadAsStringAsync().Result;
                if (string.IsNullOrEmpty(result))
                    return currentPage - 1;
                
                
                start = result.IndexOf("user_name=\'");
                while(start != -1 && comment_idx < comment.Length)
                {
                    tempidx = start;
                    
                    end = result.IndexOf('\'', start + 11);
                    nick[comment_idx] = result.Substring(start + 11, end - start - 11);
                    start = result.IndexOf("<td class=\"reply\">", start);
                    end = result.IndexOf('<', start + 1);


                    comment[comment_idx] = result.Substring(start + 18, end - start - 18).Trim().Replace("&gt;", ">");

                    start = tempidx;
                    start = result.IndexOf("user_name=\'", start + 1);
                    ++comment_idx;
                }

            }
        }

        static bool GetArticleInfo(string gallName, string articlenum, ref article_t param, bool check)
        {
            int start, end;
            HttpClient client;
            HttpResponseMessage response = null;
            string result;
            HttpClientHandler handler = new HttpClientHandler();
            handler.CookieContainer = cookie;

            client = new HttpClient(handler);
            client.Timeout = TimeSpan.FromMilliseconds(10000);

            start:
            try
            {
                response = client.GetAsync($"http://gall.dcinside.com/board/view/?id={gallName}&no={articlenum}").Result;
            }
            catch(Exception)
            {
                goto start;
            }
            result = response.Content.ReadAsStringAsync().Result;
            if(result.Contains("error/deleted"))
                return false;
            if((start = result.IndexOf("<td class=\"t_notice\" >")) != -1)
            {
                start += 22;
                while ((result.Substring(start, 2) == "공지") || (result.Substring(start, 8) == "<img src"))
                {
                    start = result.IndexOf("<td class=\"t_notice\" >", start);
                    start += 22;
                }
                
                param.recentnum = Int32.Parse(result.Substring(start, result.IndexOf('<', start) - start));
            }

            if (check)
                return true;
            start = result.IndexOf("<dt>제 목</dt>");
            start = result.IndexOf('>', start + 12);
            end = result.IndexOf('<', start + 1);
            param.title = result.Substring(start + 1, end - start - 1);

            start = result.IndexOf("user_name=\"");
            end = result.IndexOf('\"', start + 11);
            param.nick = result.Substring(start + 11, end - start - 11);

            start = result.IndexOf("<div class=\"s_write\"");
            end = result.IndexOf("<!-- //con_substance -->", start);
            if(end == -1)
            {
                param.content = "";
                return false;
            }
            param.content = result.Substring(start, end - start);
            return true;
        }

        static bool IsImproperString(string wrongstring, string str, int pos)
        {
            bool ret = false;
            hangul_t hangul_comb = new hangul_t();

            if (wrongstring == "왠" && str.Length > 1 && (str.Substring(pos, 2) == "왠지" || str.Substring(pos, 2) == "왠만"))
                ret = true;
            else if (wrongstring == "되도" && (str.Length > pos + 2 && str[pos + 2] != '<' && str[pos + 2] != ' ') || (str.Length > pos + 4 && str[pos + 2] == ' ' && str.Substring(pos + 3, 2) == "않는"))
                ret = true;
            else if (wrongstring == "되라" && str.Length > pos + 2 && (str.Substring(pos + 2, 1) == "는" || str.Substring(pos + 2, 1) == "능" || str.Substring(pos + 2, 1) == "고" || str.Substring(pos + 2, 1) == "더" || str.Substring(pos + 2, 1) == "던" || str.Substring(pos + 2, 1) == "지" || str.Substring(pos + 2, 1) == "네" || str.Substring(pos + 2, 1) == "니" || str.Substring(pos + 2, 1) == "하"))
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

        static void _point_out(string gallName, string articlenum)
        {
            Point_out.pointed_out_t[] pointed_out = new Point_out.pointed_out_t[80];
            int i, j, k, idx = 0, pos;
            string str;
            
            for(i = 0; i < comment_idx; ++i)
            {
                if (comment[i].Contains("[리듬 맞춤법 봇♬]"))
                    continue;
                for(j = 0; j < Point_out.get_ps_idx(); ++j)
                {
                    if(comment[i].Contains(ps[j].wrongstring))
                    {
                        for(k = 0; k < comment_idx; ++k)
                        {
                            if(comment[k].Contains("[리듬 맞춤법 봇♬]"))
                            {
                                if (comment[k].Substring(nick[i].Length + 2, ps[j].wrongstring.Length) != ps[j].wrongstring)
                                    continue;
                                if(comment[k].Substring(0, nick[i].Length) == nick[i])
                                {
                                    k = 777;
                                    pointed_out[idx].idx = j;
                                    pointed_out[idx++].nick = nick[i];
                                }
                                if (k == 777)
                                    break;
                            }
                        }
                    }
                }
            }
            for(i = 0; i < comment_idx; ++i)
            {
                if (comment[i].Contains("[리듬 맞춤법 봇♬]"))
                    continue;
                for(j = 0; j < Point_out.get_ps_idx(); ++j)
                {
                    pos = comment[i].IndexOf(ps[j].wrongstring);
                    if(pos != -1)
                    {
                        if (IsImproperString(ps[j].wrongstring, comment[i], pos))
                            continue;
                        for(k = 0; k < idx; ++k)
                        {
                            if(j == pointed_out[k].idx && pointed_out[k].nick == nick[i])
                            {
                                k = 777;
                                ++overlapped;
                                if(overlapped >= 10)
                                {
                                    overlapped = 0;
                                    if (++list_idx == gallid_list.Length)
                                        list_idx = 0;
                                    flip = 1;
                                    Console.WriteLine($"\r                  \r갤 ID : {gallid_list[list_idx]}");
                                    return;
                                }
                                break;
                            }
                        }
                        if (k == 777)
                            continue;

                        str = $"{nick[i]}//{ps[j].wrongstring}->{ps[j].correctstring} {ps[j].explanation} [리듬 맞춤법 봇♬]";
                        //if (Encoding.Default.GetBytes(str).Length > 200)
                            //str = $"{nick[i]}//{ps[j].wrongstring}->{ps[j].correctstring} [리듬 맞춤법 봇♬]";
                        WriteComment(gallName, articlenum, "ㅇㅇ", "0288", str);
                        str = $"{gallName} {articlenum}에 {str}이라고 씀";
                        Console.WriteLine($"\r                  \r{str}");
                        sw.WriteLine(str);
                        pointed_out[idx].idx = j;
                        pointed_out[idx++].nick = nick[i];
                    }
                }
            }
        }

        static void point_out(string gallName, string articlenum)
        {
            Point_out.pointed_out_t[] pointed_out = new Point_out.pointed_out_t[80];
            int i, j, idx = 0, pos;
            string str;
            article_t param = new article_t();

            if (!GetArticleInfo(gallName, articlenum, ref param, false))
                return;

            GetComments(gallName, articlenum);

            for(i = 0; i < Point_out.get_ps_idx(); ++i)
            {
                pos = param.title.IndexOf(ps[i].wrongstring);
                if(pos != -1)
                {
                    if (IsImproperString(ps[i].wrongstring, param.title, pos))
                        continue;

                    for(j = 0; j < comment_idx; ++j)
                    {
                        if(comment[j] == $"{ps[i].wrongstring}->{ps[i].correctstring} {ps[i].explanation} [리듬 맞춤법 봇♬]")
                        {
                            j = 777;
                            ++overlapped;
                            if(overlapped >= 10)
                            {
                                overlapped = 0;
                                if (++list_idx == gallid_list.Length)
                                    list_idx = 0;
                                flip = 1;
                                Console.WriteLine($"\r                  \r갤 ID : {gallid_list[list_idx]}");
                                return;
                            }
                            break;
                        }
                    }
                    if (j == 777)
                        continue;

                    str = $"{ps[i].wrongstring}->{ps[i].correctstring} {ps[i].explanation} [리듬 맞춤법 봇♬]";
                    //if (Encoding.Default.GetBytes(str).Length > 200)
                        //str = $"{ps[i].wrongstring}->{ps[i].correctstring} [리듬 맞춤법 봇♬]";
                    WriteComment(gallName, articlenum, "ㅇㅇ", "0288", str);
                    str = $"{gallName} {articlenum}에 {str}이라고 씀";
                    Console.WriteLine($"\r                  \r{str}");
                    sw.WriteLine(str);
                    pointed_out[idx].idx = i;
                    pointed_out[idx++].nick = "";

                    //if (GetComments(gallName, articlenum) == 1)
                        //return;

                }

                pos = param.content.IndexOf(ps[i].wrongstring);
                if(pos != -1)
                {
                    if (IsImproperString(ps[i].wrongstring, param.content, pos))
                        continue;
                    for(j = 0; j < idx; ++j)
                    {
                        if(i == pointed_out[j].idx && pointed_out[j].nick == "")
                        {
                            j = 777;
                            ++overlapped;
                            if(overlapped >= 10)
                            {
                                overlapped = 0;
                                if (++list_idx == gallid_list.Length)
                                    list_idx = 0;
                                flip = 1;
                                Console.WriteLine($"\r                  \r갤 ID : {gallid_list[list_idx]}");
                                return;
                            }
                            break;
                        }
                    }
                    if (j == 777)
                        continue;
                    for(j = 0; j < comment_idx; ++j)
                    {
                        str = $"{ps[i].wrongstring}->{ps[i].correctstring}";

                        if (comment[j].Contains("[리듬 맞춤법 봇♬]") && comment[j].IndexOf('/') == -1 && comment[j].Substring(0, str.Length) == str)
                        {
                            j = 777;
                            ++overlapped;
                            if(overlapped >= 10)
                            {
                                overlapped = 0;
                                if (++list_idx == gallid_list.Length)
                                    list_idx = 0;
                                flip = 1;
                                Console.WriteLine($"\r                  \r갤 ID : {gallid_list[list_idx]}");
                                return;
                            }
                            break;
                        }
                    }
                    if (j == 777)
                        continue;

                    str = $"{ps[i].wrongstring}->{ps[i].correctstring} {ps[i].explanation} [리듬 맞춤법 봇♬]";
                    //if (Encoding.Default.GetBytes(str).Length > 200)
                        //str = $"{ps[i].wrongstring}->{ps[i].correctstring} [리듬 맞춤법 봇♬]";
                    WriteComment(gallName, articlenum, "ㅇㅇ", "0288", str);
                    str = $"{gallName} {articlenum}에 {str}이라고 씀";
                    Console.WriteLine($"\r                  \r{str}");
                    sw.WriteLine(str);
                    pointed_out[idx].idx = i;
                    pointed_out[idx++].nick = "";
                    //if (GetComments(gallName, articlenum) == 1)
                    //{
                        //Console.WriteLine("ㅁ?");
                        //return;
                    //}
                }
            }

            _point_out(gallName, articlenum);

        }

        static bool CtrlHandler(CtrlTypes CtrlType)
        {
            switch (CtrlType)
            {
                case CtrlTypes.CTRL_C_EVENT:
                    overlapped = 0;
                    if (++list_idx == gallid_list.Length)
                        list_idx = 0;
                    flip = 1;
                    Console.WriteLine($"\r                  \r갤 ID : {gallid_list[list_idx]}");

                    break;
                default:
                    sw.Flush();
                    sw.Close();
                    return false;
            }
            return true;
            
        }

        static void TimerCallback(Object o)
        {
            overlapped = 0;
            if (++list_idx == gallid_list.Length)
                list_idx = 0;
            flip = 1;
            flip2 = 1;
            sw.Flush();
            Console.WriteLine($"\r                  \r갤 ID : {gallid_list[list_idx]}");
        }

        struct pointing_out_t 
        {
            public int idx;
            public string nick;
            public string gallName;
            public string articlenum;


        }
        
        
        static void Main(string[] args)
        {
          
            int i, j, current_idx;
            Point_out.ps_init();
            HandlerRoutine hr = new HandlerRoutine(CtrlHandler);
            SetConsoleCtrlHandler(hr, true);
            article_t param = new article_t();
            Task<string> task;

            Console.Title = "리듬 DC 맞춤법 봇";
            Timer timer = new Timer(TimerCallback, null, 1000 * 60 * 5, 1000 * 60 * 5);
            task= Getci_t("watch");
            Console.Write(task.Result);
            return;
            ci_t = task.Result;
            sw = File.AppendText("C:\\Users\\지환\\Documents\\visual studio 2015\\Projects\\리듬 DC 맞춤법 봇_Console\\리듬 DC 맞춤법 봇\\bin\\Debug\\로그.txt");

            Console.WriteLine($"갤 ID : {gallid_list[list_idx]}");

            GetArticleInfo("watch", "", ref param, true);
            j = param.recentnum;
            GetArticleInfo(gallid_list[list_idx], "", ref param, true);

            for(i = param.recentnum; ; --i, --j)
            {
                Console.Write($"\r                  \r여기도는중 {i} 시갤은 {j}");

                current_idx = list_idx;
                if(flip == 1)
                {
                    GetArticleInfo(gallid_list[current_idx], "", ref param, true);
                    i = param.recentnum;
                    flip = 0;
                }
                if(flip2 == 1)
                {
                    GetArticleInfo("watch", "", ref param, true);
                    j = param.recentnum;
                    flip2 = 0;
                }

                if (GetArticleInfo(gallid_list[current_idx], i.ToString(), ref param, true))
                    point_out(gallid_list[current_idx], i.ToString());
                else
                {
                    while(!GetArticleInfo(gallid_list[current_idx], (--i).ToString(), ref param, true))
                        Thread.Sleep(100);
                    ++i;
                }

                if (GetArticleInfo("watch", j.ToString(), ref param, true))
                    point_out("watch", j.ToString());
                else
                {
                    while (!GetArticleInfo("watch", (--j).ToString(), ref param, true))
                        Thread.Sleep(100);
                    ++j;
                }
            }


        }
    }
}
