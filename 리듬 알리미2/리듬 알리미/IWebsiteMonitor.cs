using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace 리듬_알리미
{
    public interface IWebsiteMonitor
    {
        string TelegramId { get; set; }
        TimeSpan Interval { get; set; }
        void Function();

    }
}
