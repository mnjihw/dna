using System;
using System.Collections.Generic;
using System.Text;

namespace instagram
{
    public class InstagramRequestHeaders
    {
        public string UserAgent => "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.141 Safari/537.36 Edg/87.0.664.75";
        public string XIGAppId => "936619743392459";
        public string XRequestedWith => "XMLHttpRequest";

        public string XIGWwwClaim { get; set; }
    }
}
