using System;
using System.Collections.Generic;
using System.Net.Http;
using System.Text;

namespace 리듬_알리미
{
    public interface IWebsiteMonitor
    {

        List<(string telegramId, TimeSpan interval, dynamic param, int type)> UsersInfo { get; }

        void Function();
    }
}
