using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Http;
using System.Threading;
using System.Windows.Forms;


namespace 리듬_DC_맞춤법_봇
{

    public partial class Form1 : Form
    {
        static Thread thread;

        struct pointing_out_t
        {
            public int idx;
            public string nick;
            public string gallName;
            public string articlenum;
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

        LinkedList<pointing_out_t> pointing_out = new LinkedList<pointing_out_t>();

        static int selcount;
        static string[] nick = new string[1000];
        static string[] comment = new string[1000];
        public const int PS_MAX = 512;
        public const int PS2_MAX = 100;
        public const int OVERLAPPED_COUNT = 15;
        public static point_out_t[] ps = new point_out_t[PS_MAX];
        public static point_out_t[] ps2 = new point_out_t[PS2_MAX];
        static CookieContainer cookie = new CookieContainer();
        public static int comment_idx, overlapped, list_idx;
        public static int flip, flip2;
        static string ci_t;
        public static StreamWriter sw;

        static char[] CHOSUNG_LIST = new char[] { 'ㄱ', 'ㄲ', 'ㄴ', 'ㄷ', 'ㄸ', 'ㄹ', 'ㅁ', 'ㅂ', 'ㅃ', 'ㅅ', 'ㅆ', 'ㅇ', 'ㅈ', 'ㅉ', 'ㅊ', 'ㅋ', 'ㅌ', 'ㅍ', 'ㅎ' };

        static char[] JUNGSUNG_LIST = new char[] { 'ㅏ', 'ㅐ', 'ㅑ', 'ㅒ', 'ㅓ', 'ㅔ', 'ㅕ', 'ㅖ', 'ㅗ', 'ㅘ', 'ㅙ', 'ㅚ', 'ㅛ', 'ㅜ', 'ㅝ', 'ㅞ', 'ㅟ', 'ㅠ', 'ㅡ', 'ㅢ', 'ㅣ' };

        static char[] JONGSUNG_LIST = new char[] { ' ', 'ㄱ', 'ㄲ', 'ㄳ', 'ㄴ', 'ㄵ', 'ㄶ', 'ㄷ', 'ㄹ', 'ㄺ', 'ㄻ', 'ㄼ', 'ㄽ', 'ㄾ', 'ㄿ', 'ㅀ', 'ㅁ', 'ㅂ', 'ㅄ', 'ㅅ', 'ㅆ', 'ㅇ', 'ㅈ', 'ㅊ', 'ㅋ', 'ㅌ', 'ㅍ', 'ㅎ' };

        public static string[] gallid_list = new string[] { "programming", "overwatch", "pokemon", "nintendo", "iu_new", "twice", "produce101",
 "lovelive", "drama_new1", "etc_entertainment3", "d_fighter_new1", "hanwhaeagles",
"game_classic1", "giants_new1", "m_entertainer", "idolmaster", "stock_new2", "English" };

        struct hangul_t
        {
            public int f;
            public int m;
            public int l;
        }

        delegate void ListBoxInsertCallBack(ListBox list, int idx, string str);
        delegate void ListBoxClearCallBack(ListBox list);
        delegate void ListBoxAddCallBack(ListBox list, string str);
        delegate void ListViewAddCallBack(ListView list, ListViewItem item);
        delegate int ListBoxTopIndexCallBack(ListBox list);
        delegate int ListBoxItemCountCallBack(ListBox list);
        delegate int ListBoxSelectedIndexCallBack(ListBox list, int idx);
        delegate void TimerEventFIredDelegate();


        public void ListBoxInsert(ListBox list, int idx, string str)
        {
            if (list.InvokeRequired)
                list.Invoke(new ListBoxInsertCallBack(ListBoxInsert), new object[] { list, idx, str });
            else
                list.Items.Insert(idx, str);
        }

        public void ListBoxClear(ListBox list)
        {
            if (list.InvokeRequired)
                list.Invoke(new ListBoxClearCallBack(ListBoxClear), new object[] { list });
            else
                list.Items.Clear();
        }

        public void ListBoxAdd(ListBox list, string str)
        {
            if (list.InvokeRequired)
                list.Invoke(new ListBoxAddCallBack(ListBoxAdd), new object[] { list, str });
            else
                list.Items.Add(str);
        }

        public void ListViewAdd(ListView list, ListViewItem item)
        {
            if (list.InvokeRequired)
                list.Invoke(new ListViewAddCallBack(ListViewAdd), new object[] { list, item });
            else
                list.Items.Add(item);
        }

        public int ListBoxTopIndex(ListBox list)
        {
            if (list.InvokeRequired)
                return (int)list.Invoke(new ListBoxTopIndexCallBack(ListBoxTopIndex), new object[] { list });
            else
                return list.TopIndex;
        }

        public int ListBoxItemCount(ListBox list)
        {
            if (list.InvokeRequired)
                return (int)list.Invoke(new ListBoxItemCountCallBack(ListBoxItemCount), new object[] { list });
            else
                return list.Items.Count;
        }

        public int ListBoxSelectedIndex(ListBox list, int idx)
        {
            if (list.InvokeRequired)
                return (int)list.Invoke(new ListBoxSelectedIndexCallBack(ListBoxSelectedIndex), new object[] { list, idx });
            else
                return list.SelectedIndex = idx;
        }

        public void MainThread()
        {
            article_t param = new article_t();
            int i, j, current_idx, count = 0;

            ci_t = Getci_t("watch");

            GetArticleInfo("watch", "", ref param, true);
            j = param.recentnum;
            GetArticleInfo(gallid_list[list_idx], "", ref param, true);

            ListBoxInsert(listBox2, 0, gallid_list[list_idx]);

            
            for (i = param.recentnum; ; --i, --j)
            {
                ListBoxClear(listBox3);
                ListBoxClear(listBox4);
                ListBoxAdd(listBox3, i.ToString());
                ListBoxAdd(listBox4, j.ToString());

                if (flip == 1)
                {
                    current_idx = list_idx;
                    GetArticleInfo(gallid_list[current_idx], "", ref param, true);
                    i = param.recentnum;
                    flip = 0;
                }
                else
                    current_idx = list_idx;
                if (flip2 == 1)
                {
                    GetArticleInfo("watch", "", ref param, true);
                    j = param.recentnum;
                    flip2 = 0;
                }
                if (GetArticleInfo(gallid_list[current_idx], i.ToString(), ref param, true))
                    point_out(gallid_list[current_idx], i.ToString());
                else
                {
                    while (!GetArticleInfo(gallid_list[current_idx], (--i).ToString(), ref param, true))
                    {
                        Thread.Sleep(100);
                        ListBoxClear(listBox3);
                        ListBoxAdd(listBox3, i.ToString());
                        if (++count > 50)
                        {
                            if (++list_idx == gallid_list.Length)
                                list_idx = 0;
                            flip = 1;
                            ListBoxClear(listBox2);
                            ListBoxInsert(listBox2, 0, gallid_list[list_idx]);
                            break;
                        }
                    }
                    count = 0;
                    ++i;
                }
                if (GetArticleInfo("watch", j.ToString(), ref param, true))
                    point_out("watch", j.ToString());
                else
                {
                    while (!GetArticleInfo("watch", (--j).ToString(), ref param, true))
                    {
                        System.Threading.Thread.Sleep(100);
                        ListBoxClear(listBox4);
                        ListBoxAdd(listBox4, j.ToString());
                    }
                    ++j;
                }
            }
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



        static string Getci_t(string gallName)
        {
            HttpClient client;
            string result;
            HttpClientHandler handler = new HttpClientHandler();
            handler.CookieContainer = cookie;

            client = new HttpClient(handler);
            client.Timeout = TimeSpan.FromMilliseconds(10000);

            start:
            try
            {
                result = client.GetAsync($"http://gall.dcinside.com/board/view/?id={gallName}&no=1").Result.Content.ReadAsStringAsync().Result;
            }
            catch (Exception)
            {
                goto start;
            }

           
            return result.Substring(result.IndexOf("ci_t\" value=\"") + 13, 32);

        }

        async void WriteComment(string gallName, string articlenum, string nick, string pw, string content)
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

            start:
            querystring = new FormUrlEncodedContent(new Dictionary<string, string> { { "ci_t", ci_t }, { "name", nick }, { "password", pw }, { "memo", content }, { "id", gallName }, { "no", articlenum }, { "best_orgin", "" }, {"spam_key", "rhkdrhgkwlak!!" } });

            try
            {
                await client.PostAsync("http://gall.dcinside.com/forms/comment_submit", querystring);
            }
            catch (Exception)
            {
                goto start;
            }
        }

        int GetComments(string gallName, string articlenum)
        {
            int currentPage = 1, start, end, tempidx;
            HttpContent querystring;
            HttpClientHandler handler = new HttpClientHandler();
            string result;
            handler.CookieContainer = cookie;
            HttpClient client = new HttpClient(handler);
            client.Timeout = TimeSpan.FromMilliseconds(10000);


            comment_idx = 0;
            Array.Clear(comment, 0, comment.Length);

            while (true)
            {
                client.DefaultRequestHeaders.Clear();
                client.DefaultRequestHeaders.Add("Referer", $"http://gall.dcinside.com/board/comment_view/?id={gallName}&no={articlenum}&page=1");
                client.DefaultRequestHeaders.TryAddWithoutValidation("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
                client.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.93 Safari/537.36");
                client.DefaultRequestHeaders.Add("X-Requested-With", "XMLHttpRequest");

                start:
                querystring = new FormUrlEncodedContent(new Dictionary<string, string> { { "ci_t", ci_t }, { "id", gallName }, { "no", articlenum }, { "comment_page", currentPage++.ToString() } });

                try
                {
                    result = client.PostAsync("http://gall.dcinside.com/comment/view", querystring).Result.Content.ReadAsStringAsync().Result;
                }
                catch (Exception)
                {
                    goto start;
                }

                if (string.IsNullOrEmpty(result))
                    return currentPage - 1;


                start = result.IndexOf("user_name=\'");
                while (start != -1 && comment_idx < comment.Length)
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

        bool GetArticleInfo(string gallName, string articlenum, ref article_t param, bool check)
        {
            int start, end;
            HttpClient client;
            string result;
            HttpClientHandler handler = new HttpClientHandler();
            handler.CookieContainer = cookie;

            client = new HttpClient(handler);
            client.Timeout = TimeSpan.FromMilliseconds(10000);

            start:
            try
            {
                result = client.GetAsync($"http://gall.dcinside.com/board/view/?id={gallName}&no={articlenum}").Result.Content.ReadAsStringAsync().Result;
            }
            catch (Exception)
            {
                goto start;
            }
            if (result.Contains("error/deleted"))
                return false;
            if ((start = result.IndexOf("<td class=\"t_notice\" >")) != -1)
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
            if (start == -1)
            {
                param.content = "";
                return false;
            }
            start = result.IndexOf('>', start + 12);
            end = result.IndexOf('<', start + 1);
            param.title = result.Substring(start + 1, end - start - 1);

            start = result.IndexOf("user_name=\"");
            end = result.IndexOf('\"', start + 11);
            param.nick = result.Substring(start + 11, end - start - 11);

            start = result.IndexOf("<div class=\"s_write\"");
            if (start == -1)
            {
                param.content = "";
                return false;
            }
            end = result.IndexOf("<!-- //con_substance -->", start);
            if (end == -1)
            {
                param.content = "";
                return false;
            }
            param.content = result.Substring(start, end - start);
            return true;
        }

        bool IsImproperString(string wrongstring, string str, int pos)
        {
            
            bool ret = false;
            hangul_t hangul_comb = new hangul_t();

            if(wrongstring == "되")
            {
                if (str.Length - pos <= 3)
                {
                    if (str.Length - pos == 3 && str[pos + 1] >= 32 && str[pos + 1] <= 126 && str[pos + 2] >= 32 && str[pos + 2] <= 126)
                        ret = false;
                    else if (str.Length - pos == 2 && str[pos + 1] >= 32 && str[pos + 1] <= 126)
                        ret = false;
                    else if (str.Length - pos == 1)
                        ret = false;
                    else
                        ret = true;
                }
                else
                    ret = true;
            }
            else if (wrongstring == "왠" && (str.Length > pos + 1 && (str.Substring(pos, 2) == "왠지" || str.Substring(pos, 2) == "왠만")) || (str.Length > pos + 2 && str.Substring(pos + 1, 2) == "종일"))
                ret = true;
            else if (wrongstring == "되도" && (str.Length > pos + 2 && str[pos + 2] != '<' && str[pos + 2] != ' ') || (str.Length > pos + 4 && str[pos + 2] == ' ' && str.Substring(pos + 3, 2) == "않는"))
                ret = true;
            else if (wrongstring == "되라" && str.Length > pos + 2 && (str[pos + 2] == '는' || str[pos + 2] == '능' || str[pos + 2] == '고' || str[pos + 2] == '더' || str[pos + 2] == '던' || str[pos + 2] == '지' || str[pos + 2] == '네' || str[pos + 2] == '니' || str[pos + 2] == '하'))
                ret = true;
            else if (wrongstring == "던대")
            {
                if (str.Length > pos + 2 && (str[pos + 2] == '로' || str[pos + 2] == '신'))
                    ret = true;
                else
                    ret = false;
            }
            else if (wrongstring == "몇일" && str.Length > pos + 2 && (str[pos + 2] == '까' || str.Length > pos + 3 && str.Substring(pos + 2, 2) == "텐데"))
                ret = true;
            else if (wrongstring == "뒷" || wrongstring == "윗" || wrongstring == "아랫")
            {
                if (wrongstring == "윗" && pos - 1 >= 0 && str[pos - 1] == ' ')
                    ret = true;
                else if (str.Length > pos + wrongstring.Length)
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
            else if (wrongstring == "파토" && pos - 1 >= 0 && str.Length > pos + 2 && str.Substring(pos - 1, 4) == "레파토리")
                ret = true;
            else if (wrongstring == "어따" && pos - 1 >= 0 && str[pos - 1] != ' ')
                ret = true;
            else if (wrongstring == "순대국" && str.Length > pos + 3 && str[pos + 3] == '밥')
                ret = true;

            return ret;
        }

        bool IsImproperString2(string wrongstring, string str, int pos)
        {
            bool ret = false;
            //int len;

            if (wrongstring == "에요" || wrongstring == "에여" || wrongstring == "에욤" || wrongstring == "에염" || wrongstring == "에용" || wrongstring == "에영" || wrongstring == "에욧" || wrongstring == "에엿" || wrongstring == "에양" || wrongstring == "에얌")
            {
                if (pos - 2 >= 0 && str.Substring(pos - 2, 2) == "아니" || pos - 1 >= 0 && (str[pos - 1] == '뭐' || str[pos - 1] == '머' || str[pos - 1] == '거' || str[pos - 1] == '꺼'))
                    ret = true;
            }
            else if (wrongstring == "이요" && pos - 1 >= 0 && str[pos - 1] == '하')
                ret = true;
            else if (wrongstring == "요세" && str.Length > pos + 2 && str[pos + 2] != ' ')
                ret = true;
            else if (wrongstring == "로써")
            {
                if (str.Length > pos + 2 && str[pos + 2] != ' ')
                    ret = true;
                else if (pos - 1 >= 0 && str[pos - 1] == '이')
                    ret = true;
                else if (pos - 2 >= 0 && (str.Substring(pos - 2, 2) == "어제" || str.Substring(pos - 2, 2) == "오늘"))
                    ret = true;
            }
            else if (wrongstring == "던")
            {
                if (str.Length > pos + 1)
                {
                    if (str.Length == pos + 2)
                    {
                        if (str[pos + 1] == '가' || str[pos + 1] == '지')
                            ret = false;
                        else
                            ret = true;
                    }
                    else if (str[pos + 1] == '말' || (str[pos + 1] == ' ' && str[pos + 2] == '말'))
                        ret = false;
                    else if (str[pos + 1] == '데' || str[pos + 1] == '거' || str[pos + 1] == '것' || str[pos + 1] == '건' || str[pos + 1] == '게')
                        ret = true;
                    else if(str.Length > pos + 2)
                    {
                        if (str[pos + 1] == '가')
                        {
                            if ((str[pos + 2] != ' ' && str[pos + 2] != '.'))
                                ret = true;
                            else
                                ret = false;
                        }
                        else if (str[pos + 1] == '지' || str[pos + 1] == '질')
                        {
                            if (str[pos + 2] != ' ')
                                ret = true;
                            else
                                ret = false;
                        }
                        else if(str[pos + 1] == ' ')
                        {
                            if (str[pos + 2] == '거' || str[pos + 2] == '것' || str[pos + 2] == '건')
                                ret = true;
                        }
                    }
                    else
                    {
                        
                        /*else
                        {
                            len = str.Length > pos + 6 ? 5 : str.Length - pos - 1;
                            if (str.IndexOf("던", pos + 1, len) == -1)
                            {
                                ret = true;
                            }
                            else
                                ret = false;
                        }*/
                    }
                }
            }

            return ret;
        }

        void _point_out(string gallName, string articlenum)
        {
            LinkedListNode<pointing_out_t> node;
            MyObject m;
            hangul_t hangul_comb = new hangul_t();
            hangul_t hangul_comb2 = new hangul_t();
            Point_out.pointed_out_t[] pointed_out = new Point_out.pointed_out_t[80];
            int i, j, k = 0, l, idx = 0, pos, len, count = 0, ignore_idx = 0;
            int []ignore_array = new int[512];
            bool ignore_comment = false;
            string str;

            for (i = 0; i < comment_idx; ++i)
            {
                for (j = 0; j < Point_out.get_ps_idx(); ++j)
                {
                    pos = comment[i].IndexOf(ps[j].wrongstring);
                    if (pos != -1)
                    {
                        if (IsImproperString(ps[j].wrongstring, comment[i], pos))
                            continue;
                        if (++count >= 7)
                        {
                            ignore_comment = true;
                            ignore_array[ignore_idx++] = i;
                            count = 0;
                            continue;
                        }
                    }
                }
            }

            for (i = 0; i < comment_idx; ++i)
            {
                if (comment[i].Contains("[리듬 맞춤법 봇♬]"))
                    continue;
                for (j = 0; j < Point_out.get_ps_idx(); ++j)
                {
                    if (comment[i].Contains(ps[j].wrongstring))
                    {
                        for (k = 0; k < comment_idx; ++k)
                        {
                            if (comment[k].Contains("[리듬 맞춤법 봇♬]"))
                            {
                                if (comment[k].Substring(nick[i].Length + 2, ps[j].wrongstring.Length) != ps[j].wrongstring)
                                    continue;
                                if (comment[k].Substring(0, nick[i].Length) == nick[i])
                                {
                                    if (idx >= 80)
                                    {
                                        idx = 0;
                                        Array.Clear(pointed_out, 0, pointed_out.Length);
                                    }
                                    pointed_out[idx].idx = j;
                                    pointed_out[idx++].nick = nick[i];
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            for (i = 0; i < comment_idx; ++i)
            {
                if (comment[i].Contains("[리듬 맞춤법 봇♬]"))
                    continue;
                for (j = 0; j < Point_out.get_ps_idx(); ++j)
                {
                    pos = comment[i].IndexOf(ps[j].wrongstring);
                    if (pos != -1)
                    {
                        if (IsImproperString(ps[j].wrongstring, comment[i], pos))
                            continue;
                        for (k = 0; k < idx; ++k)
                        {
                            if (j == pointed_out[k].idx && pointed_out[k].nick == nick[i])
                            {
                                k = 777;
                                ++overlapped;
                                if (overlapped >= OVERLAPPED_COUNT)
                                {
                                    overlapped = 0;
                                    if (++list_idx == gallid_list.Length)
                                        list_idx = 0;
                                    flip = 1;
                                    ListBoxClear(listBox2);
                                    ListBoxInsert(listBox2, 0, gallid_list[list_idx]);
                                    return;
                                }
                                break;
                            }
                        }
                        if (k == 777)
                            continue;

                        str = $"{nick[i]}//{ps[j].wrongstring}->{ps[j].correctstring} {ps[j].explanation} [리듬 맞춤법 봇♬]";
                        for (l = 0; l < ignore_idx; ++l)
                        {
                            if (ignore_comment && ignore_array[l] == i)
                            {
                                l = 777;
                                break;
                            }
                        }
                        if (l == 777)
                            continue;
                        WriteComment(gallName, articlenum, "ㅇㅇ", "0288", str);
                        str = $"{gallName} {articlenum} 에 {str}이라고 씀";
                        if (ListBoxTopIndex(listBox1) + selcount == ListBoxItemCount(listBox1))
                        {
                            ListBoxAdd(listBox1, str);
                            ListBoxSelectedIndex(listBox1, ListBoxItemCount(listBox1) - 1);
                        }
                        else
                            ListBoxAdd(listBox1, str);
                        sw.WriteLine(str);
                        if (idx >= 80)
                        {
                            idx = 0;
                            Array.Clear(pointed_out, 0, pointed_out.Length);
                        }
                        pointed_out[idx].idx = j;
                        pointed_out[idx++].nick = nick[i];
                    }
                }
            }

            for (i = 0; i < comment_idx; ++i)
            {
                if (comment[i].Contains("[리듬 맞춤법 봇♬]"))
                    continue;
                for (j = 0; j < Point_out.get_ps2_idx(); ++j)
                {
                    if (comment[i].Contains(ps2[j].wrongstring))
                    {
                        for (k = 0; k < comment_idx; ++k)
                        {
                            if (comment[k].Contains("[리듬 맞춤법 봇♬]"))
                            {
                                if (comment[k].Substring(nick[i].Length + 2, ps2[j].wrongstring.Length) != ps2[j].wrongstring)
                                    continue;
                                if (comment[k].Substring(0, nick[i].Length) == nick[i])
                                {
                                    pointing_out.AddLast(new pointing_out_t { gallName = gallName, articlenum = articlenum, nick = nick[i], idx = j });
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            for (i = 0; i < comment_idx; ++i)
            {
                if (comment[i].Contains("[리듬 맞춤법 봇♬]"))
                    continue;
                for (j = 0; j < Point_out.get_ps2_idx(); ++j)
                {
                    pos = comment[i].IndexOf(ps2[j].wrongstring);
                    if (pos != -1)
                    {
                        if (IsImproperString2(ps2[j].wrongstring, comment[i], pos))
                            continue;
                        node = pointing_out.First;
                        while (node != null)
                        {
                            if (j == node.Value.idx && nick[i] == node.Value.nick)
                            {
                                k = 777;
                                ++overlapped;
                                if (overlapped >= OVERLAPPED_COUNT)
                                {
                                    overlapped = 0;
                                    if (++list_idx == gallid_list.Length)
                                        list_idx = 0;
                                    flip = 1;
                                    ListBoxClear(listBox2);
                                    ListBoxInsert(listBox2, 0, gallid_list[list_idx]);
                                    return;
                                }
                                break;
                            }
                            node = node.Next;
                        }
                        if (k == 777)
                            continue;
                        k = comment[i].IndexOf(ps2[j].wrongstring);
                        pos = k - 4 >= 0 ? k - 4 : 0;
                        len = comment[i].Length > pos + 10 ? 10 : comment[i].Length - pos;
                        hangul_disassemble(comment[i].Substring(pos, len)[len - 1], ref hangul_comb);
                        hangul_disassemble(ps2[j].correctstring[ps2[j].correctstring.Length - 1], ref hangul_comb2);
                        m = new MyObject();
                        m.content = $"{comment[i].Substring(pos, len)}{(hangul_comb.l == 0 ? "를" : "을")} {ps2[j].correctstring}{(hangul_comb2.l == 0 ? "로" : "으로")} 지적 예정";
                        m.gallName = gallName;
                        m.articlenum = articlenum;
                        m.nick = nick[i];
                        m.idx = j;
                        CreateListViewItem(listView1, m);
                        m = null;
                        pointing_out.AddLast(new pointing_out_t { gallName = gallName, articlenum = articlenum, nick = nick[i], idx = j });
                    }
                }
            }
        }

        void point_out(string gallName, string articlenum)
        {
            LinkedListNode<pointing_out_t> node;
            MyObject m;
            hangul_t hangul_comb = new hangul_t();
            hangul_t hangul_comb2 = new hangul_t();
            Point_out.pointed_out_t[] pointed_out = new Point_out.pointed_out_t[80];
            int i, j = 0, idx = 0, pos, len, count = 0;
            bool ignore_title = false;
            string str;
            article_t param = new article_t();

            if (!GetArticleInfo(gallName, articlenum, ref param, false))
                return;

            GetComments(gallName, articlenum);

            for (i = 0; i < Point_out.get_ps_idx(); ++i)
            {
                pos = param.title.IndexOf(ps[i].wrongstring);
                if (pos != -1)
                {
                    if (IsImproperString(ps[i].wrongstring, param.title, pos))
                        continue;
                    if (++count >= 7)
                    {
                        ignore_title = true;
                        break;
                    }
                }
            }
            if (!ignore_title)
            {
                for (i = 0; i < Point_out.get_ps_idx(); ++i)
                {
                    pos = param.title.IndexOf(ps[i].wrongstring);
                    if (pos != -1)
                    {
                        if (IsImproperString(ps[i].wrongstring, param.title, pos))
                            continue;

                        for (j = 0; j < comment_idx; ++j)
                        {
                            if (comment[j] == $"{ps[i].wrongstring}->{ps[i].correctstring} {ps[i].explanation} [리듬 맞춤법 봇♬]")
                            {
                                j = 777;
                                ++overlapped;
                                if (overlapped >= OVERLAPPED_COUNT)
                                {
                                    overlapped = 0;
                                    if (++list_idx == gallid_list.Length)
                                        list_idx = 0;
                                    flip = 1;
                                    ListBoxClear(listBox2);
                                    ListBoxInsert(listBox2, 0, gallid_list[list_idx]);
                                    return;
                                }
                                break;
                            }
                        }
                        if (j == 777)
                            continue;

                        str = $"{ps[i].wrongstring}->{ps[i].correctstring} {ps[i].explanation} [리듬 맞춤법 봇♬]";
                        WriteComment(gallName, articlenum, "ㅇㅇ", "0288", str);
                        str = $"{gallName} {articlenum} 에 {str}이라고 씀";
                        if (ListBoxTopIndex(listBox1) + selcount == ListBoxItemCount(listBox1))
                        {
                            ListBoxAdd(listBox1, str);
                            ListBoxSelectedIndex(listBox1, ListBoxItemCount(listBox1) - 1);
                        }
                        else
                            ListBoxAdd(listBox1, str);
                        sw.WriteLine(str);
                        if (idx >= 80)
                        {
                            idx = 0;
                            Array.Clear(pointed_out, 0, pointed_out.Length);
                        }
                        pointed_out[idx].idx = i;
                        pointed_out[idx++].nick = "";

                    }

                    pos = param.content.IndexOf(ps[i].wrongstring);
                    if (pos != -1)
                    {
                        if (IsImproperString(ps[i].wrongstring, param.content, pos))
                            continue;
                        for (j = 0; j < idx; ++j)
                        {
                            if (i == pointed_out[j].idx && pointed_out[j].nick == "")
                            {
                                j = 777;
                                ++overlapped;
                                if (overlapped >= OVERLAPPED_COUNT)
                                {
                                    overlapped = 0;
                                    if (++list_idx == gallid_list.Length)
                                        list_idx = 0;
                                    flip = 1;
                                    ListBoxClear(listBox2);
                                    ListBoxInsert(listBox2, 0, gallid_list[list_idx]);
                                    return;
                                }
                                break;
                            }
                        }
                        if (j == 777)
                            continue;
                        for (j = 0; j < comment_idx; ++j)
                        {
                            str = $"{ps[i].wrongstring}->{ps[i].correctstring}";

                            if (comment[j].Contains("[리듬 맞춤법 봇♬]") && comment[j].IndexOf('/') == -1 && comment[j].Substring(0, str.Length) == str)
                            {
                                j = 777;
                                ++overlapped;
                                if (overlapped >= OVERLAPPED_COUNT)
                                {
                                    overlapped = 0;
                                    if (++list_idx == gallid_list.Length)
                                        list_idx = 0;
                                    flip = 1;
                                    ListBoxClear(listBox2);
                                    ListBoxInsert(listBox2, 0, gallid_list[list_idx]);
                                    return;
                                }
                                break;
                            }
                        }
                        if (j == 777)
                            continue;

                        str = $"{ps[i].wrongstring}->{ps[i].correctstring} {ps[i].explanation} [리듬 맞춤법 봇♬]";
                        WriteComment(gallName, articlenum, "ㅇㅇ", "0288", str);
                        str = $"{gallName} {articlenum} 에 {str}이라고 씀";
                        if (ListBoxTopIndex(listBox1) + selcount == ListBoxItemCount(listBox1))
                        {
                            ListBoxAdd(listBox1, str);
                            ListBoxSelectedIndex(listBox1, ListBoxItemCount(listBox1) - 1);
                        }
                        else
                            ListBoxAdd(listBox1, str);
                        sw.WriteLine(str);
                        if (idx >= 80)
                        {
                            idx = 0;
                            Array.Clear(pointed_out, 0, pointed_out.Length);
                        }
                        pointed_out[idx].idx = i;
                        pointed_out[idx++].nick = "";
                    }
                }
            }

            for (i = 0; i < Point_out.get_ps2_idx(); ++i)
            {
                pos = param.title.IndexOf(ps2[i].wrongstring);
                if (pos != -1)
                {
                    if (IsImproperString2(ps2[i].wrongstring, param.title, pos))
                        continue;
                    node = pointing_out.First;
                    while (node != null)
                    {
                        if (articlenum == node.Value.articlenum && gallName == node.Value.gallName && string.IsNullOrEmpty(node.Value.nick) && i == node.Value.idx)
                        {
                            j = 777;
                            ++overlapped;
                            if (overlapped >= OVERLAPPED_COUNT)
                            {
                                overlapped = 0;
                                if (++list_idx == gallid_list.Length)
                                    list_idx = 0;
                                flip = 1;
                                ListBoxClear(listBox2);
                                ListBoxInsert(listBox2, 0, gallid_list[list_idx]);
                                return;
                            }
                            break;
                        }
                        node = node.Next;
                    }
                    if (j == 777)
                        continue;

                    for (j = 0; j < comment_idx; ++j)
                    {
                        if (comment[j] == $"{ps2[i].wrongstring}->{ps2[i].correctstring} {ps2[i].explanation} [리듬 맞춤법 봇♬]")
                        {
                            j = 777;
                            ++overlapped;
                            if (overlapped >= OVERLAPPED_COUNT)
                            {
                                overlapped = 0;
                                if (++list_idx == gallid_list.Length)
                                    list_idx = 0;
                                flip = 1;
                                ListBoxClear(listBox2);
                                ListBoxInsert(listBox2, 0, gallid_list[list_idx]);
                                return;
                            }
                            break;
                        }
                    }
                    if (j == 777)
                        continue;

                    j = param.title.IndexOf(ps2[i].wrongstring);
                    pos = j - 4 >= 0 ? j - 4 : 0;
                    len = param.title.Length > pos + 10 ? 10 : param.title.Length - pos;
                    hangul_disassemble(param.title.Substring(pos, len)[len - 1], ref hangul_comb);
                    hangul_disassemble(ps2[i].correctstring[ps2[i].correctstring.Length - 1], ref hangul_comb2);
                    m = new MyObject();
                    m.content = $"{param.title.Substring(pos, len)}{(hangul_comb.l == 0 ? "를" : "을")} {ps2[i].correctstring}{(hangul_comb2.l == 0 ? "로" : "으로")} 지적 예정";
                    m.gallName = gallName;
                    m.articlenum = articlenum;
                    m.nick = "";
                    m.idx = i;
                    CreateListViewItem(listView1, m);
                    m = null;
                    pointing_out.AddLast(new pointing_out_t { gallName = gallName, articlenum = articlenum, nick = "", idx = i });
                }

            }
            for (i = 0, j = 0; i < Point_out.get_ps2_idx(); ++i)
            { 
                pos = param.content.IndexOf(ps2[i].wrongstring);
                if (pos != -1)
                {
                    if (IsImproperString2(ps2[i].wrongstring, param.content, pos))
                        continue;

                    node = pointing_out.First;
                    while (node != null)
                    {
                        if (articlenum == node.Value.articlenum && gallName == node.Value.gallName && string.IsNullOrEmpty(node.Value.nick) && i == node.Value.idx)
                        {
                            j = 777;
                            ++overlapped;
                            if (overlapped >= OVERLAPPED_COUNT)
                            {
                                overlapped = 0;
                                if (++list_idx == gallid_list.Length)
                                    list_idx = 0;
                                flip = 1;
                                ListBoxClear(listBox2);
                                ListBoxInsert(listBox2, 0, gallid_list[list_idx]);
                                return;
                            }
                            break;
                        }
                        node = node.Next;
                    }
                    if (j == 777)
                        continue;

                    for (j = 0; j < comment_idx; ++j)
                    {
                        str = $"{ps2[i].wrongstring}->{ps2[i].correctstring}";

                        if (comment[j].Contains("[리듬 맞춤법 봇♬]") && comment[j].IndexOf('/') == -1 && comment[j].Substring(0, str.Length) == str)
                        {
                            j = 777;
                            ++overlapped;
                            if (overlapped >= OVERLAPPED_COUNT)
                            {
                                overlapped = 0;
                                if (++list_idx == gallid_list.Length)
                                    list_idx = 0;
                                flip = 1;
                                ListBoxClear(listBox2);
                                ListBoxInsert(listBox2, 0, gallid_list[list_idx]);
                                return;
                            }
                            break;
                        }
                    }
                    if (j == 777)
                        continue;

                    j = param.content.IndexOf(ps2[i].wrongstring);
                    pos = j - 4 >= 0 ? j - 4 : 0;
                    len = param.content.Length > pos + 10 ? 10 : param.content.Length - pos;
                    hangul_disassemble(param.content.Substring(pos, len)[len - 1], ref hangul_comb);
                    hangul_disassemble(ps2[i].correctstring[ps2[i].correctstring.Length - 1], ref hangul_comb2);
                    m = new MyObject();
                    m.content = $"{param.content.Substring(pos, len)}{(hangul_comb.l == 0 ? "를" : "을")} {ps2[i].correctstring}{(hangul_comb2.l == 0 ? "로" : "으로")} 지적 예정";
                    m.gallName = gallName;
                    m.articlenum = articlenum;
                    m.nick = "";
                    m.idx = i;
                    CreateListViewItem(listView1, m);
                    m = null;
                    pointing_out.AddLast(new pointing_out_t { gallName = gallName, articlenum = articlenum, nick = "", idx = i });

                }
            }

            _point_out(gallName, articlenum);

        }

        void Work()
        {
            overlapped = 0;
            if (++list_idx == gallid_list.Length)
                list_idx = 0;
            flip = 1;
            flip2 = 1;
            if (sw.BaseStream != null)
                sw.Flush();
            ListBoxClear(listBox2);
            ListBoxInsert(listBox2, 0, gallid_list[list_idx]);

            ci_t = Getci_t("watch");
        }

        void TimerCallback(object sender, System.Timers.ElapsedEventArgs e)
        {
            BeginInvoke(new TimerEventFIredDelegate(Work));
        }


        public Form1()
        {
            InitializeComponent();
        }
        

        private void Form1_Load(object sender, EventArgs e)
        {
            Point_out.ps_init();
            System.Timers.Timer timer = new System.Timers.Timer(1000 * 60 * 5);
            timer.Elapsed += new System.Timers.ElapsedEventHandler(TimerCallback);
            timer.Start();

            sw = File.AppendText("C:\\Users\\mnjihw\\Documents\\visual studio 2017\\Projects\\리듬 DC 맞춤법 봇\\리듬 DC 맞춤법 봇\\bin\\Debug\\로그.txt");


            listView1.BeginUpdate();

            listView1.Columns.Add("내용", 220);
            listView1.Columns.Add("지적 여부", 70);

            listView1.EndUpdate();

            foreach (string s in gallid_list)
                comboBox1.Items.Add(s);

            selcount = (listBox1.ClientRectangle.Bottom - listBox1.ClientRectangle.Top) / listBox1.ItemHeight;

            thread = new Thread(new ThreadStart(MainThread));
            thread.Start();
        }


        private void listView1_KeyDown(object sender, KeyEventArgs e)
        {
            int i;

            if (listView1.Items.Count > 0)
            {
                if (listView1.SelectedItems.Count == 0)
                    return;
                i = listView1.SelectedIndices[0];

                if (e.KeyCode == Keys.Left)
                {
                    listView1.SelectedItems[0].SubItems[1].Text = "지적함";
                    if (i + 1 < listView1.Items.Count)
                    {
                        listView1.Items[i + 1].Selected = true;
                        listView1.Items[i + 1].Focused = true;
                        listView1.Items[i + 1].EnsureVisible();

                    }
                    listView1.Focus();
                }
                else if (e.KeyCode == Keys.Right)
                {
                    listView1.SelectedItems[0].SubItems[1].Text = "지적 안 함";
                    if (i + 1 < listView1.Items.Count)
                    {
                        listView1.Items[i + 1].Selected = true;
                        listView1.Items[i + 1].Focused = true;
                        listView1.Items[i + 1].EnsureVisible();
                    }
                }
            }
        }

        public void CreateListViewItem(ListView listView, MyObject obj)
        {
            ListViewItem item = new ListViewItem();
            item.Tag = obj;
            item.Text = obj.content;
            item.SubItems.Add("");


            ListViewAdd(listView, item);
             

        }

        private void button1_Click(object sender, EventArgs e)
        {
            string str;
            LinkedListNode<pointing_out_t> node, nextnode;
            MyObject obj;

            int count = listView1.Items.Count;
            for (int i = 0; i < listView1.Items.Count; ++i)
            {
                if (listView1.Items[i].SubItems[1].Text == "지적함")
                {
                    obj = (MyObject)listView1.Items[i].Tag;

                    str = $"{(string.IsNullOrEmpty(obj.nick) ? "" : obj.nick + "//")}{ps2[obj.idx].wrongstring}->{ps2[obj.idx].correctstring} {ps2[obj.idx].explanation} [리듬 맞춤법 봇♬]";

                    WriteComment(obj.gallName, obj.articlenum, "ㅇㅇ", "0288", str);
                    str = $"{obj.gallName} {obj.articlenum} 에 {str}이라고 씀";
                    if (ListBoxTopIndex(listBox1) + selcount == ListBoxItemCount(listBox1))
                    {
                        ListBoxAdd(listBox1, str);
                        ListBoxSelectedIndex(listBox1, ListBoxItemCount(listBox1) - 1);
                    }
                    else
                        ListBoxAdd(listBox1, str);
                    sw.WriteLine(str);

                    node = pointing_out.First;
                    while (node != null)
                    {
                        nextnode = node.Next;
                        if (node.Value.articlenum == obj.articlenum && node.Value.gallName == obj.gallName && node.Value.nick == obj.nick && node.Value.idx == obj.idx)
                            pointing_out.Remove(node);
                        node = nextnode;
                    }
                    listView1.Items.RemoveAt(i--);
                }
                else if (listView1.Items[i].SubItems[1].Text == "지적 안 함")
                    listView1.Items.RemoveAt(i--);
            }




        }

        private void listBox1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            string str, gallName, articlenum;
            int idx;

            if (listBox1.SelectedItem != null)
            {
                str = listBox1.SelectedItem.ToString();
                str = str.Substring(0, str.IndexOf("에"));
                idx = str.IndexOf(" ");
                gallName = str.Substring(0, idx);
                articlenum = str.Substring(idx + 1, str.Length - idx - 2);
                Process.Start("chrome.exe", $"http://gall.dcinside.com/board/view/?id={gallName}&no={articlenum}&page=1");
            }
        }

        private void listView1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            if (listView1.SelectedItems != null)
            {
                MyObject obj = (MyObject)listView1.SelectedItems[0].Tag;
                Process.Start("chrome.exe", $"http://gall.dcinside.com/board/view/?id={obj.gallName}&no={obj.articlenum}&page=1");
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (comboBox1.SelectedIndex != -1)
            {
                Process.Start("chrome.exe", $"http://gall.dcinside.com/board/lists/?id={comboBox1.SelectedItem}&s_type=search_all&s_keyword=맞춤법");
            }
        }

        private void listView1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {
            overlapped = 0;
            if (++list_idx == gallid_list.Length)
                list_idx = 0;
            flip = 1;
            ListBoxClear(listBox2);
            ListBoxInsert(listBox2, 0, gallid_list[list_idx]);
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {

            if (sw.BaseStream != null)
            {
                sw.Flush();
                sw.Close();
            }
            if (thread.IsAlive)
                thread.Abort();

        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }
    }

    public class MyObject
    {
        public string gallName;
        public string articlenum;
        public string nick;
        public string content;
        public int idx;

        public override string ToString()
        {
            return content;
        }
    }

}
