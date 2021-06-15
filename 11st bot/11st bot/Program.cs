#define TEST
using OpenQA.Selenium;
using OpenQA.Selenium.Chrome;
using OpenQA.Selenium.DevTools;
using OpenQA.Selenium.Remote;
using OpenQA.Selenium.Support.UI;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Runtime.InteropServices;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace _11st_bot
{
    class Program
    {
        private static string Now => $"{DateTime.Now:yyyy-MM-dd HH:mm:ss}";
        private static HttpClient HttpClient { get; } = new HttpClient();
        private static WebDriverWait Wait { get; set; }
        private static bool IsOSWindows { get; } = RuntimeInformation.IsOSPlatform(OSPlatform.Windows);
        private static ChromeDriver Driver { get; set; }
        private static ChromeDriverService Service { get; set; }
        private static bool IsMe { get; set; }
        private static string Id { get; set; }
        private static string Password { get; set; }
        private static string PaymentPassword { get; set; }

        private static void Login(WebDriverWait wait, string id, string password)
        {
            Driver.Url = "https://login.11st.co.kr/auth/login.tmall";
            
            wait.Until(d => d.FindElements(By.Id("memId")).Count != 0);
            Driver.FindElement(By.Id("memId")).SendKeys(id);
            Driver.FindElement(By.Id("memPwd")).SendKeys(password);
            Driver.FindElement(By.XPath("//button[@data-log-actionid-label='login']")).Click(); //로그인 버튼 클릭

            wait.Until(d => d.FindElements(By.XPath("//img[@src='//m.11st.co.kr/MW/img/common/gnb/gnb-logo.png']")).Count != 0); //메인화면 기다림

        }
        private static async Task WaitUntil(DateTime dateTime)
        {
            while (true)
            {
                Console.WriteLine($"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}] {dateTime:yyyy-MM-dd HH:mm:ss}까지 기다리는 중...");

                if (DateTime.Now > dateTime)
                    break;
                await Task.Delay(1000);
            }
        }
        private static void OnExit(object sender, EventArgs e)
        {
            Console.WriteLine("셀레니움 종료함");
            Driver?.Dispose();
            Service?.Dispose();
        }
        private static int GetBlankIndex(string imageData, int blockCount)
        {
            using var ms = new MemoryStream(Convert.FromBase64String(imageData));
            using var bitmap = new Bitmap(ms);
            int blockSize = bitmap.Width / blockCount;

            for (int i = 0; i < blockCount; ++i)
            {
                bool found = false;

                for (int j = 0; j < blockSize; ++j)
                {
                    if (bitmap.GetPixel(i * blockSize + j, bitmap.Height / 2) == Color.FromArgb(255, 255, 255, 255))
                        found = true;
                }
                if (!found)
                    return i;
            }
            return -1;
        }
        private static async Task InitializeAsync()
        {
            AppDomain.CurrentDomain.ProcessExit += OnExit;
            Console.CancelKeyPress += OnExit;
            Logger.Initialize("1557742912:AAHJpIVYERHRxiXwME78ahwnYjg6-7tAbuE", 1390930895);

            if (!File.Exists("setting.json"))
            {
                await File.WriteAllTextAsync("setting.json", JsonSerializer.Serialize(new { Id = "", Password = "", PaymentPassword = "" }));
                Console.WriteLine("setting.json 파일에 계정 정보를 입력해주세요.");
                Console.ReadLine();
                return;
            }
            var json = JsonDocument.Parse(await File.ReadAllTextAsync("setting.json"));
            Id = json.RootElement.GetProperty("Id").ToString();
            Password = json.RootElement.GetProperty("Password").ToString();
            PaymentPassword = json.RootElement.GetProperty("PaymentPassword").ToString();

            if (string.IsNullOrWhiteSpace(Id) || string.IsNullOrWhiteSpace(Password) || string.IsNullOrWhiteSpace(PaymentPassword))
            {
                Console.WriteLine("setting.json 제대로 입력하셈");
                Console.ReadLine();
                return;
            }

            if (Id == "mnjihw0210")
                IsMe = true;
#if TEST
            IsMe = false;
#endif

            var chromeOptions = new ChromeOptions { PageLoadStrategy = PageLoadStrategy.None };
            chromeOptions.UnhandledPromptBehavior = UnhandledPromptBehavior.Accept;
            chromeOptions.AddAdditionalOption(CapabilityType.UnexpectedAlertBehavior, "ignore"); //메박 무시

            string argument;
            if (Id == "mnsuyu")
                argument = @$"user-data-dir={Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData)}\Google\Chrome\Selenium2";
            else
                argument = @$"user-data-dir={Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData)}\Google\Chrome\Selenium";
            chromeOptions.AddArgument(argument);
#if !TEST
            chromeOptions.AddArgument("headless"); //헤드리스랑 개발자도구 동시에 안되는듯
#endif
            chromeOptions.AddArgument("--auto-open-devtools-for-tabs");
            chromeOptions.AddArgument("disable-gpu");
            chromeOptions.AddArgument("disable-infobars");
            chromeOptions.AddArgument("--disable-extensions");
            //chromeOptions.AddArgument("--disk-cache-size=1"); //안 해도 disable cache 체크돼있음



            string deviceName;
            if (IsOSWindows)
                deviceName = "iPad";
            else
                deviceName = "iPhone X";
            chromeOptions.EnableMobileEmulation(deviceName);
            chromeOptions.AddUserProfilePreference("profile", new
            {
                managed_default_content_settings =
                new
                {
                    //images = 2,
                    images = 1,
                    plugins = 2,
                    popups = 2,
                    geolocation = 2,
                    notifications = 2,
                    auto_select_certificate = 2,
                    mixed_script = 2,
                    media_stream = 2,
                    media_stream_mic = 2,
                    media_stream_camera = 2,
                    protocol_handlers = 2,
                    app_banner = 2,
                }
            });

            Service = ChromeDriverService.CreateDefaultService(Directory.GetCurrentDirectory());

            Service.HideCommandPromptWindow = true;
#if !TEST
            await WaitUntil(new DateTime(2021, 5, 25, 9, 59, 55));
#endif
            if (IsMe)
                _ = Logger.SendTelegramMessageAsync("봇 가동 시작함");

            Driver = new ChromeDriver(Service, chromeOptions);
            Driver.ExecuteChromeCommand("Storage.clearCookies", new());


            Wait = new WebDriverWait(new SystemClock(), Driver, TimeSpan.FromMinutes(10), TimeSpan.FromMilliseconds(100));

        }
        private static long OptionId { get; set; }
        private static async Task<bool> IsInStockAsync(long productId, int sequence)
        {
            OptionId = -1;
            var postData = new Dictionary<string, string>
            {
                ["mixOptNo"] = $"1:{sequence},", //콜론 뒤 숫자만 바뀜
                ["selOptCnt"] = "1", //고정
                ["selOptTyp"] = "01", //고정
                ["optNmList"] = "1", //아무 값
                ["mixOptNm"] = "1", //아무 값
                ["xSiteCode"] = "1000966141", //11번가 고유값 추정
            };
            string result;
            try
            {
                result = await (await HttpClient.PostAsync($"http://m.11st.co.kr/products/v1/mw/products/{productId}/stock-option-nos", new FormUrlEncodedContent(postData))).Content.ReadAsStringAsync();
            }
            catch (Exception)
            {
                return false;
            }

            var match = Regex.Match(result, @"\(\[(.+)\]\)");
            var json = JsonDocument.Parse(match.Groups[1].Value);
            string productStatus = "";
            long quantity = 0, optionId = 0;
            if (json.RootElement.TryGetProperty("stckQty", out var element))
                element.TryGetInt64(out quantity);
            if (json.RootElement.TryGetProperty("prdStckNo", out element))
                element.TryGetInt64(out optionId);
            if (json.RootElement.TryGetProperty("prdStckStatCd", out element))
                productStatus = element.GetString();
            if (optionId != 0)
                OptionId = optionId;

            Console.WriteLine($"[{Now}] 상품 ID: {productId} 옵션 ID: {optionId} 재고: {quantity}개");

            return quantity != 0 && productStatus == "01";
        }
        private static bool AcceptAlert()
        {
            IAlert alert;
            try
            {
                alert = Driver.SwitchTo().Alert();
            }
            catch(NoAlertPresentException)
            {
                return false;
            }
            alert.Accept();
            return true;
        }
        static async Task Main()
        {
            HttpClient.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (iPad; CPU OS 11_0 like Mac OS X) AppleWebKit/604.1.34 (KHTML, like Gecko) Version/11.0 Mobile/15A5341f Safari/604.1 Edg/90.0.4430.212");

            ReadOnlyCollection<IWebElement> elements = default;

            await InitializeAsync();
            Console.WriteLine($"[{Now}] 초기화 완료");

            Login(Wait, Id, Password);
            
            Console.WriteLine($"[{Now}] 로그인 완료");

            #region 재고 들어오길 기다리는 루틴
            bool condition = true;
#if TEST
            var productId = 2299825175; //2299825175 //3166873599
#else
            var productId = 3127972957; //찐
#endif
            while (condition)
            {
                for (int i = 1; i <= 2; ++i)
                {
                    if (await IsInStockAsync(productId, i)) //재고 들어오면 break
                    {
                        condition = false;
                        break;
                    }
                    if (IsMe)
                        _ = Logger.SendTelegramMessageAsync("새로고침 중...");
                    await Task.Delay(600);
                }
            }
            Console.WriteLine($"[{Now}] 재고 들어옴");

            Stopwatch s = new();
            s.Start();



            #endregion

            #region 결제 페이지로 이동해서 버튼 누르는 루틴

            condition = true;

            while(condition)
            {
                Driver.Url = $"https://buy.m.11st.co.kr/MW/Order/orderBasicFirstStep.tmall?incommingCode=01&optionPrc=0&optionStock=1&prdNo={productId}&optionStckNo={OptionId}";
                
                while (true)
                {
                    if (AcceptAlert()) //메박 떴으면 새로고침
                    {
                        Console.WriteLine($"[{Now}] 결제 페이지 진입 실패함");
                        break;
                    }

                    try
                    {
                        if (Driver.ExecuteScript("return mwSkpay.isAvailable();") is true) //SK페이 모듈 로드됐으면 break
                        {
                            condition = false;
                            break;
                        }
                    }
                    catch (JavaScriptException) { }
                    await Task.Delay(100);
                }
                if (!condition)
                    break;
                await Task.Delay(500);
            }
      

            if (IsMe)
                _ = Logger.SendTelegramMessageAsync("결제 페이지 들어옴");
            Wait.Until(d =>
            {
                elements = d.FindElements(By.Id("doPaySubmit"));
                return elements.Count != 0;
            });

            if (IsMe)
                _ = Logger.SendTelegramMessageAsync("결제버튼 누름");
            Console.WriteLine($"[{Now}] 결제버튼 누름");

            elements[0].Click(); //n원 결제 버튼

            #endregion


            #region 결제비밀번호 누르는 루틴
            Wait.Until(d => d.FindElements(By.Id("skpay-ui-frame")).Count != 0);

            Wait.Until(d =>
            {
                Driver.SwitchTo().DefaultContent();
                Driver.SwitchTo().Frame("skpay-ui-frame");
                elements = Driver.FindElements(By.XPath("//button[starts-with(@id, 'keypad11pay-keypad')]/span"));
                return elements.Count != 0;
            });

            var match = Regex.Match(elements[0].GetAttribute("style"), @"url\(""data:image.+,(.+)""\)");

            var imageData = match.Groups[1].Value;
            var blankIndex = GetBlankIndex(imageData, 11);
            if (IsMe)
                _ = Logger.SendTelegramMessageAsync("보안 키패드 이미지 얻어옴");
            Console.WriteLine($"[{Now}] 보안 키패드 이미지 얻어옴");
            if (blankIndex == -1)
            {
                if (IsMe)
                    _ = Logger.SendTelegramMessageAsync("블랭크인덱스 -1뜸");
                blankIndex = 0;
            }
            
            var dict = new Dictionary<int, int>
            {
                [0] = 9,
                [1] = 0,
                [2] = 1,
                [3] = 2,
                [4] = 3,
                [5] = 4,
                [6] = 5,
                [7] = 6,
                [8] = 7,
                [9] = 8,

            };
#if TEST
            int count = 0;
#endif
            dynamic keys = Driver.ExecuteScript("return KeypadLauncher.webSDK.keys;"); //헤드리스 모드에서 개발자도구 실행 안돼서 이거 안되는듯

            foreach(var character in PaymentPassword)
            {
                int index;
#if TEST
                if (++count == 6)
                    break;
#endif
                int num = (int)char.GetNumericValue(character);

                if (num > blankIndex || num == 0)
                    index = dict[num] + 1;
                else
                    index = dict[num];
                Driver.ExecuteScript($"KeypadLauncher.webSDK.viewRenderer.viewEventListener.addValueEventListener('{keys[index]}');");
            }
            
            s.Stop();
            
            if (IsMe)
                _ = Logger.SendTelegramMessageAsync("결제완료!");
            Console.WriteLine($"[{Now}] 결제완료!");
            Console.WriteLine($"[{Now}] 재고 들어온 뒤로 결제시까지 {s.ElapsedMilliseconds}ms 소요됨");
            
            #endregion

            while (true)
                await Task.Delay(1000);
        }
    }
}
