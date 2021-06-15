using S22.Imap;
using System;
using System.Linq;
using System.Net.Mail;
using System.Threading.Tasks;

namespace imap_client
{
    class Program
    {
        static async Task Main()
        {
            /*SmtpClient a = new SmtpClient("127.0.0.1", 143);
            a.Send("mnjihw@naver.com", "abc@125.137.84.29", "gd", "bd");
            return;*/
            Console.WriteLine("접속한다");
            var client = new ImapClient("127.0.0.1", 143, "ANONYMOUS", "hi123", AuthMethod.Login, false);

            Console.WriteLine("인증됨? " + client.Authed);
             
            var uids = client.Search(SearchCondition.All());
            var messages = client.GetMessages(uids.Take(10));

            foreach (var message in messages)
            {
                Console.WriteLine($"Body: {message.Body}");
            }
        }
    }
}
