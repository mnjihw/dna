using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Telegram.Bot;
using Telegram.Bot.Types;
using 리듬_DC_콘솔_봇;

namespace 임경민
{
    class Program
    {
        #region DllImports
        [DllImport("user32.dll")]
        private static extern int SendMessage(IntPtr hWnd, uint message, IntPtr wParam, IntPtr lParam);
        [DllImport("user32.dll")]
        private static extern IntPtr FindWindow(string className, string windowName);
        [DllImport("user32.dll")]
        private static extern uint SendInput(uint count, [MarshalAs(UnmanagedType.LPArray), In] INPUT[] inputs, int size);
        [DllImport("imm32.dll")]
        private static extern IntPtr ImmGetDefaultIMEWnd(IntPtr hWnd);

        [DllImport("user32.dll")]
        private static extern bool SetForegroundWindow(IntPtr hWnd);



        [StructLayout(LayoutKind.Sequential)]
        public struct INPUT
        {
            internal uint type;
            internal InputUnion U;
            internal static int Size
            {
                get { return Marshal.SizeOf(typeof(INPUT)); }
            }
        }

        [StructLayout(LayoutKind.Explicit)]
        internal struct InputUnion
        {
            [FieldOffset(0)]
            internal MOUSEINPUT mi;
            [FieldOffset(0)]
            internal KEYBDINPUT ki;
            [FieldOffset(0)]
            internal HARDWAREINPUT hi;
        }
        [StructLayout(LayoutKind.Sequential)]
        internal struct MOUSEINPUT
        {
            internal int dx;
            internal int dy;
            internal int mouseData;
            internal MOUSEEVENTF dwFlags;
            internal uint time;
            internal UIntPtr dwExtraInfo;
        }
        [Flags]
        internal enum MOUSEEVENTF : uint
        {
            ABSOLUTE = 0x8000,
            HWHEEL = 0x01000,
            MOVE = 0x0001,
            MOVE_NOCOALESCE = 0x2000,
            LEFTDOWN = 0x0002,
            LEFTUP = 0x0004,
            RIGHTDOWN = 0x0008,
            RIGHTUP = 0x0010,
            MIDDLEDOWN = 0x0020,
            MIDDLEUP = 0x0040,
            VIRTUALDESK = 0x4000,
            WHEEL = 0x0800,
            XDOWN = 0x0080,
            XUP = 0x0100
        }
        [StructLayout(LayoutKind.Sequential)]
        internal struct KEYBDINPUT
        {
            internal short wVk;
            internal short wScan;
            internal KEYEVENTF dwFlags;
            internal int time;
            internal UIntPtr dwExtraInfo;
        }
        [Flags]
        internal enum KEYEVENTF : uint
        {
            EXTENDEDKEY = 0x0001,
            KEYUP = 0x0002,
            SCANCODE = 0x0008,
            UNICODE = 0x0004
        }

        [StructLayout(LayoutKind.Sequential)]
        internal struct HARDWAREINPUT
        {
            internal int uMsg;
            internal short wParamL;
            internal short wParamH;
        }
        private const int WM_IME_CONTROL = 0x0283;
        private const int VK_RETURN = 0xD;
        private const int VK_SHIFT = 0x10;
        private const int VK_HANGUL = 0x15;
        private const int INPUT_KEYBOARD = 1;
        #endregion
        #region phonemes
        private static ReadOnlyDictionary<char, char> phonemes = new ReadOnlyDictionary<char, char>(new Dictionary<char, char>
        {
            ['ㅂ'] = 'q',
            ['ㅃ'] = 'Q',
            ['ㅈ'] = 'w',
            ['ㅉ'] = 'W',
            ['ㄷ'] = 'e',
            ['ㄸ'] = 'E',
            ['ㄱ'] = 'r',
            ['ㄲ'] = 'R',
            ['ㅅ'] = 't',
            ['ㅆ'] = 'T',
            ['ㅛ'] = 'y',
            ['ㅕ'] = 'u',
            ['ㅑ'] = 'i',
            ['ㅐ'] = 'o',
            ['ㅒ'] = 'O',
            ['ㅔ'] = 'p',
            ['ㅖ'] = 'P',
            ['ㅁ'] = 'a',
            ['ㄴ'] = 's',
            ['ㅇ'] = 'd',
            ['ㄹ'] = 'f',
            ['ㅎ'] = 'g',
            ['ㅗ'] = 'h',
            ['ㅓ'] = 'j',
            ['ㅏ'] = 'k',
            ['ㅣ'] = 'l',
            ['ㅋ'] = 'z',
            ['ㅌ'] = 'x',
            ['ㅊ'] = 'c',
            ['ㅍ'] = 'v',
            ['ㅠ'] = 'b',
            ['ㅜ'] = 'n',
            ['ㅡ'] = 'm',
        });
        private static ReadOnlyDictionary<char, string> phonemes2 = new ReadOnlyDictionary<char, string>(new Dictionary<char, string>
        {
            ['ㅢ'] = "ml",
            ['ㅘ'] = "hk",
            ['ㅙ'] = "ho",
            ['ㅚ'] = "hl",
            ['ㅝ'] = "nj",
            ['ㅞ'] = "np",
            ['ㅟ'] = "nl",
        });
        #endregion
        
        private static TelegramBotClient Bot { get; set; } = new TelegramBotClient("1180330939:AAF-CcmmlgjvyqqCXoOj7NhKw6v2h0LcocQ"); //1
       // private static TelegramBotClient Bot { get; set; } = new TelegramBotClient("1186238033:AAGtUwfCmGOMxikBQOq5T3z-WSbBqWnIeOE"); //2
        private static long ChatId => 1192751434;

     
        static void Main()
        {
            Random random = new Random();

            Bot.OnMessage += async (sender, e) =>
            {
                if (e.Message.Chat.Id == ChatId)
                {
                    Console.WriteLine($"{e.Message.Text} 왔음");
                    var sb = new StringBuilder();

                    foreach(var character in e.Message.Text)
                    {
                        var h = Hangul.Disassemble(character);

                        if (h.Initial.HasValue)
                        {
                            if (phonemes.ContainsKey(h.Initial.Value))
                                sb.Append(phonemes[h.Initial.Value]);
                            else if (phonemes2.ContainsKey(h.Initial.Value))
                                sb.Append(phonemes2[h.Initial.Value]);
                        }
                        if (h.Medial.HasValue)
                        {
                            if (phonemes.ContainsKey(h.Medial.Value))
                                sb.Append(phonemes[h.Medial.Value]);
                            else if (phonemes2.ContainsKey(h.Medial.Value))
                                sb.Append(phonemes2[h.Medial.Value]);
                        }
                        if (h.Final != ' ')
                        {
                            if (phonemes.ContainsKey(h.Final))
                                sb.Append(phonemes[h.Final]);
                            else if (phonemes2.ContainsKey(h.Final))
                                sb.Append(phonemes2[h.Final]);
                        }
                    }

                    IntPtr hWnd = FindWindow("MapleStoryClass", null);

                    if (hWnd != null)
                    {
                        SetForegroundWindow(hWnd);
                        IntPtr imeWnd = ImmGetDefaultIMEWnd(hWnd);

                        if (imeWnd != null)
                        {
                            var input = new[] { new INPUT { type = INPUT_KEYBOARD }, new INPUT { type = INPUT_KEYBOARD } };
                            var state = SendMessage(imeWnd, WM_IME_CONTROL, new IntPtr(5), new IntPtr(0));

                            if (state == 0)
                            {
                                input[0].U.ki.wVk = VK_HANGUL;
                                input[0].U.ki.dwFlags = 0;
                                SendInput(1, input, INPUT.Size);
                                await Task.Delay(10);
                            }
      

                            foreach (var i in sb.ToString())
                            {
                                if (char.IsUpper(i))
                                {
                                    input[0].U.ki.wVk = VK_SHIFT;
                                    input[1].U.ki.wVk = (short)i;
                                    input[0].U.ki.dwFlags = 0;
                                    input[1].U.ki.dwFlags = 0;
                                    SendInput(2, input, INPUT.Size);

                                    await Task.Delay(5 + random.Next(5));
                                    input[0].U.ki.dwFlags = KEYEVENTF.KEYUP;
                                    input[1].U.ki.dwFlags = KEYEVENTF.KEYUP;
                                    SendInput(2, input, INPUT.Size);
                                }
                                else
                                {
                                    input[0].U.ki.wVk = (short)char.ToUpper(i);
                                    input[0].U.ki.dwFlags = 0;
                                    SendInput(1, input, INPUT.Size);
                                }
                                await Task.Delay(40 + random.Next(10));

                            }
                            input[0].U.ki.wVk = VK_RETURN;
                            input[0].U.ki.dwFlags = 0;
                            SendInput(1, input, INPUT.Size);

                        }
                        else
                            await Bot.SendTextMessageAsync(e.Message.Chat.Id, "IME 윈도우 찾지 못함!");
                    }
                    else
                        await Bot.SendTextMessageAsync(e.Message.Chat.Id, "메이플 찾지 못함!");                    

                    
                }
            };
            Bot.StartReceiving();
            Console.WriteLine("텔레그램 봇 서버 작동중...");
            Console.ReadLine();
            return;
            
        }
    }
}
