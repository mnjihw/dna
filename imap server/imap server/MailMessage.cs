using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace imap_server
{
    public class MailMessage
    {
        public string Subject { get; set; }
        public string From { get; set; }
        public string To { get; set; }
        public string Body { get; set; }
        public uint MessageSequenceNumber { get; set; }

    }
}
