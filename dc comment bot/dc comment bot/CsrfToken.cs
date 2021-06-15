using System;
using System.Collections.Generic;
using System.Text;

namespace dc_comment_bot
{
    public class CsrfTokens
    {
        public string CurT { get; set; }
        public string UserIP { get; set; }
        public string[] Checks { get; } = new string[4];
        public string Recommend { get; set; }
        public string ServiceCode { get; set; }
        public string Esno { get; set; }

        public override string ToString() => $"cur_t: {CurT}{Environment.NewLine}" +
            $"user_ip: {UserIP}{Environment.NewLine}" +
            $"check_6: {Checks[0]}{Environment.NewLine}" +
            $"check_7: {Checks[1]}{Environment.NewLine}" +
            $"check_8: {Checks[2]}{Environment.NewLine}" +
            $"check_9: {Checks[3]}{Environment.NewLine}" +
            $"recommend: {Recommend}{Environment.NewLine}" +
            $"service_code: {ServiceCode}{Environment.NewLine}" +
            $"e_s_n_o: {Esno}";
    }
}
