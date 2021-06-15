using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace imap_server
{
    class Program
    {
        private static byte[] buffer = new byte[8192];
        //private static List<MailMessage> Messages = new();
        private static Dictionary<uint, MailMessage> Messages = new();
        private static long UidNext { get; set; }
        private static long UidValidity { get; set; }
        


        public static string GetResponse(NetworkStream stream)
        {
            using var ms = new MemoryStream();
            int bytesRead;

            while(stream.DataAvailable)
            {
                bytesRead = stream.Read(buffer, 0, buffer.Length);
                if (bytesRead == 0)
                    break;
                ms.Write(buffer, 0, bytesRead);
            }

            return Encoding.UTF8.GetString(ms.ToArray()); 
        }

        public static void SendResponse(NetworkStream stream, string response)
        { 
            stream.Write(Encoding.ASCII.GetBytes(response));
        }

        public static void ProcessResponse(NetworkStream stream, string response)
        {
            var match = Regex.Match(response, @"([^ ]+) (.+)\r\n");

            var tag = match.Groups[1].Value;
            var commandString = match.Groups[2].Value;


            Console.WriteLine($"command: {commandString}");

            if(commandString.Contains("LOGIN"))
            {
                SendResponse(stream, $"{tag} OK\r\n");
            }
            else if(commandString.Contains(@"SELECT ""INBOX"""))
            {
                var s = $"* {Messages.Count} EXISTS\r\n" +
                    $"* 1 RECENT\r\n" +
                    //$"* OK [UNSEEN 0]\r\n" + //
                    $"* OK [UIDVALIDITY 3]\r\n" + //
                    $"* OK [UIDNEXT 1]\r\n" + //
                    //$"* FLAGS ()\r\n" +
                    //$"* OK [PERMANENTFLAGS ()]\r\n" +
                    $"{tag} OK [READ-ONLY]\r\n";
                SendResponse(stream, s);
            }
            else if(commandString.Contains("UID SEARCH ALL"))
            {
                //var s = $"* SEARCH {string.Join(' ', Messages.Select(message => message.Uid))}\r\n" +
                var s = $"* SEARCH {string.Join(' ', Messages.Keys)}\r\n" +
                    $"{tag} OK\r\n";
                SendResponse(stream, s);
            }
            else if(commandString.Contains("UID FETCH")) // 1 (BODY[])
            {
                match = Regex.Match(commandString, @"UID FETCH (\d+)"); // \(BODY\[\]\)");
                var uid = uint.Parse(match.Groups[1].Value);
                //uid랑 메시지를 엮어서 딕셔너리로 만들어야 할듯

                string s = "";
                if(Messages.ContainsKey(uid))
                {
                    /*var data = "Date: Wed, 17 Jul 1996 02:23:25 -0700 (PDT)\r\n" +
                        "From: Rhythm\r\n" +
                        "Subject: hi123\r\n";*/
                    var data = $"Subject: {Messages[uid].Subject}\r\n" +
                        $"From: {Messages[uid].From}\r\n" +
                        $"To: {Messages[uid].To}\r\n" + 
                        $"Body: {Messages[uid].Body}\r\n";
                    

                    s += $"* {uid} FETCH (BODY[] {{{data.Length}}}\r\n" + 
                        $"{data})\r\n" +
                        $"{tag} OK UID FETCH completed\r\n";
                    Console.WriteLine(s);
                }
                else
                    s += $"{tag} OK\r\n";
                SendResponse(stream, s);
            }
        }

        /*
                 * 
                 * UID: 오름차순이어야 함, 고유해야 함, 인접할필요 없음, 변경되면 안됨
                 * next UID value: 새 메시지의 예상 UID임, UID 유효함이 바뀌지 않는 한(새 메시지가 추가되지 않는 한 절대 변경되면 안 됨, 새 메시지가 추가되면 그게 나중에 지워져도 반드시 변경돼야 함)
                 * UID validity value: 메일박스 선택 시 OK untagged 응답의 UIDVALIDITY 응답 코드에서 전송됨, 이전 세션의 UID가 이 세션에서 이어지지 않으면 반드시 이전 세션의 값보다 커야 함
                 * 메시지 시퀀스 넘버: 1부터 메시지 개수까지의 상대적인 위치임, 반드시 오름차순 UID로 정렬돼야 함, 새 메시지가 추가될 때 기존 메시지 개수 + 1로 할당됨, 메시지가 삭제되면 그 이후 추가되는 값은 1 줄어듦
                 * 
                 * [Message Numbers]
                 * 메시지는 UID와 메시지 시퀀스 넘버 중 하나로 접근 가능함
                 * 
                 * [Unique Identifier(UID) Message Attribute]
                 * UID는 각 메시지에 할당된 32비트 정수고 UID validity value와 사용될 때 64비트로 형성됨(이 값은 영원히 해당 메일박스 및 나중에 같은 이름으로 사용될 메일박스의 메시지랑 겹치면 절대 안됨)
                 * 각 메시지가 메일박스에 추가될 때 이전에 추가된 UID보다 높은 값으로 할당됨. 
                 * 메시지 시퀀스 넘버와 달리 UID는 반드시 인접한 값일 필요는 없음.
                 * 
                 * 메시지의 UID는 한 세션에서 절대 변경되면 안 되고 세션 간에도 변경되면 안 됨. 세션 간 UID를 변경하면 반드시 UIDVALIDITY 메커니즘을 사용해서 인지 가능해야 함.
                 * 클라이언트가 서버와 이전 세션의 상태를 재동기화하려면 지속 UID가 필요함. (연결이 끊겼거나 오프라인 클라이언트 사용 시)
                 * 
                 * UID 핸들링에 도움을 주는 2가지 값인 next UID value랑 UID validity value는 모든 메일박스와 연관돼 있음.
                 * next UID value는 메일박스에 할당될 새 메시지의 예상값임. UID 유효함이 바뀌지 않으면, next UID value는 반드시 다음 2가지 특성을 가진다.
                 * 첫쨰, next UID value는 새 메시지가 메일박스에 추가되지 않는 한 절대 변경되지 말아야 한다.
                 * 둘째, next UID value는 새 메시지가 메일박스에 추가되면 그 새 메시지가 나중에 지워져도 반드시 변경돼야 한다. 
                 * 
                 * 참고: next UID value는 클라이언트가 이 값을 확인한 이후부터 새 메시지가 메일박스에 도착했는지 확인할 수 있는 수단을 제공하도록 의도되지
                 * 메시지가 이 UID를 가진다는 걸 보장하려고 의도된 게 아니다.
                 * 클라이언트는 오로지 이때 next UID value를 얻는다는 것과 이 시간 이후로 도착하는 메시지가 해당 값과 같거나 더 큰 UID를 가질 것이라는 것만 알 수 있을 뿐이다. 
                 * 
                 * UID validity value는 메일박스 선택 시 OK untagged 응답의 UIDVALIDITY 응답 코드에서 전송된다.
                 * 이전 세션의 UID가 이 세션에서 지속되는 데 실패하면, UID validity value는 반드시 이전 세션에서 사용됐던 값보다 커야 한다.
                 * 
                 * 참고: 이상적으로, UID는 항상 지속돼야 한다. 비록 이 스펙이 일부 서버 환경에서 지속될 실패를 피할 수 없다는 사실을 알지만,
                 * 이 스펙은 이 문제를 회피할 메시지 보관 구현 테크닉을 강하게 권장한다. 예를 들어,
                 * 1) UID는 메일박스에서 항상 반드시 엄격히 오름차순이어야 한다. 만약 물리적 메시지 저장소가 IMAP이 아닌 에이전트에 의해 재배열되면, 
                 * 이전의 UID가 재배열의 결과로 더 이상 엄격히 오름차순이 아니기 때문에 이것은 메일박스에서 UID가 재생성될 것을 요구한다. 
                 * 
                 * 2) 만약 메시지 저장소가 UID를 저장할 메커니즘을 갖고 있지 않으면, 매 세션 UID를 반드시 재생성해야 하고 각 세션은 반드시 유일한 UIDVALIDITY 값을 가져야 한다.
                 * 
                 * 3) 만약 메일박스가 삭제되고 나중에 같은 이름의 새 메일박스가 생성되면, 서버는 반드시 UID를 이전 메일박스로부터 추적하거나 반드시 새 메일박스에 대한 새 UIDVALIDITY 값을 할당해야 한다.
                 * 이 상황에서 사용하기 좋은 UIDVALIDITY 값은 메일박스의 생성 날짜/시간의 32비트 표현이다. 메일박스가 삭제되고(혹은 이름이 바뀌고) 미래에 같은 이름의 새 메일박스가 생성돼도 UID가 결코 재사용되지 않는다는 것이 보장된다면 1과 같은 상수를 사용해도 좋다.
                 * 
                 * 4) 메일박스 이름, UIDVALIDITY, 그리고 UID의 조합은 반드시 그 서버의 하나의 불변 메시지를 나타내야 한다(영원히).
                 * 특히 내부 시간, 크기, 봉투(첨부파일?), body 구조, 메시지 텍스트는 결코 바뀌지 말아야 한다. 이것은 메시지 개수나 STORE 명령으로 변경 가능한 속성을(FLAGS 등) 포함하지 않는다.
                 * 
                 * [Message Sequence Number Message Attribute]
                 * 이것은 1부터 메시지의 개수까지의 상대 위치이다.
                 * 이 위치는 반드시 UID 오름차순으로 정렬돼야 한다. 새 메시지가 추가될 때, 새 메시지가 추가되기 전 메일박스의 메시지 개수보다 1 높은 메시지 시퀀스 넘버가 할당된다.
                 * ㅋ
                 * 메시지 시퀀스 넘버는 세션 진행 중 재할당될 수 있다. 예를 들어 메시지가 메일박스에서 영구적으로 삭제되면, 모든 차후의 메시지에 대한 메시지 시퀀스 넘버가 1 감소된다.
                 * 메일박스의 메시지 개수도 또한 1 감소된다. 이와 비슷하게, 새 메시지에는 삭제되기 전에 다른 메시지에 의해 한번 사용됐었던 메시지 시퀀스 넘버가 할당될 수 있다.
                 * 
                 * 메일박스에서 상대 위치로 메시지에 접근하는 것뿐만 아니라 메시지 시퀀스 넘버는 수학적 계산에 사용될 수 있다. 예를 들어, 만약 untagged "11 EXISTS"가 수신되고
                 * 이전에 untagged "8 EXISTS"가 수신되었으면, 3개의 새 메시지가 9, 10, 11의 메시지 시퀀스 넘버를 가지고 도착한 것이다. 또 다른 예로, 만약 523 메시지 메일박스의 287 메시지가 
                 * UID 12345를 가지면, 더 낮은 UID를 가지는 286개의 메시지와 더 높은 UID를 가지는 236개의 메시지가 있는 것이다.
                 */

        static async Task Main()
        {
            Messages.Add(5, new MailMessage { MessageSequenceNumber = 5, Body = "GD1", Subject = "ffdz1", From = "mjh <what@cac.edu>", To = "imap@cac.washington.edu" });
            Messages.Add(6, new MailMessage { MessageSequenceNumber = 6, Body = "GD2", Subject = "ffdz2", From = "mjh <what@cac.edu>", To = "imap@cac.washington.edu" });
            Messages.Add(7, new MailMessage { MessageSequenceNumber = 7, Body = "GD3", Subject = "ffdz3", From = "mjh <what@cac.edu>", To = "imap@cac.washington.edu" });
            var tcpListener = new TcpListener(IPAddress.Any, 143);
            tcpListener.Start();

            var tcpClient = tcpListener.AcceptTcpClient();
            var stream = tcpClient.GetStream();

            SendResponse(stream, $"* OK\r\n");


            while (true)
            {
                if (stream.DataAvailable)
                {
                    var response = GetResponse(stream);
                    Console.WriteLine(response);

                    ProcessResponse(stream, response);
                }
            }
            
        }
    }
}
