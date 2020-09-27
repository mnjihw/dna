using System;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using OpenQA.Selenium;
using OpenQA.Selenium.Edge;
using OpenQA.Selenium.Interactions;
using OpenQA.Selenium.Support.UI;
using Telegram.Bot;
using Newtonsoft.Json;
using System.Net.Http;
using HtmlAgilityPack;

namespace 소프라노몰
{
    class Program
    {
        static async Task Main()
        {
            HttpClient httpClient = new HttpClient();
            HtmlDocument htmlDocument = new HtmlDocument();
            var bot = new TelegramBotClient("631711537:AAG164NGqHcdoYUnV-0QzKqE0B39z4a0U_0");
            bool condition = true;

            while (condition)
            {
                var result = await httpClient.GetStringAsync("https://sofrano.com/product/list.html?cate_no=55");
                Console.WriteLine($"{DateTime.Now}: 상품 목록 가져옴");
                htmlDocument.LoadHtml(result);

                var products = htmlDocument.DocumentNode.SelectNodes("//ul[@class='prdList grid5']/*");
                string[] ids = { "553", "555", "839" };

                foreach(var product in products)
                {
                    if(!ids.Any(s => product.Attributes["id"].Value.Contains(s)))
                    {
                        var productInformation = product.SelectSingleNode("div[@class='description']/p[@class='name']");
                        var url = productInformation.SelectSingleNode("a").Attributes["href"].Value;
                        var title = productInformation.SelectSingleNode("a").InnerText;

                        if (new[] { "동물의숲", "동물의 숲", "동숲" }.Any(s => title.Contains(s)))
                        {
                            condition = false;
                            await bot.SendTextMessageAsync("581720134", $"https://sofrano.com{url} 스위치 동숲에디션 떴다 소프라노몰로 달려라");
                            await bot.SendTextMessageAsync("991922024", $"https://sofrano.com{url} 스위치 동숲에디션 떴다 소프라노몰로 달려라");
                            break;
                        }
                    }
                }
                if (!condition)
                    break;
                await Task.Delay(TimeSpan.FromMinutes(5));

            }


            return;
            /*const string id = "mnjihw";
            const string password = "flema0210~";
            const string name = "문지환";
            var edgeOptions = new EdgeOptions { BinaryLocation = @"C:\Program Files (x86)\Microsoft\Edge Beta\Application\msedge.exe", UseChromium = true };
            var service = EdgeDriverService.CreateDefaultServiceFromOptions(Directory.GetCurrentDirectory(), "msedgedriver.exe", edgeOptions);

            service.HideCommandPromptWindow = true;
            using var driver = new EdgeDriver(service, edgeOptions)
            {
                Url = "https://m.sofrano.com/member/login.html"
            };

            var wait = new WebDriverWait(driver, TimeSpan.FromSeconds(300));
            Actions actions = new Actions(driver);

            driver.FindElement(By.Id("member_id")).SendKeys(id);
            driver.FindElement(By.Id("member_passwd")).SendKeys(password);
            driver.FindElement(By.ClassName("btnSubmit")).Click();

            wait.Until(d => d.FindElements(By.Id("main")).Count != 0);
            Console.WriteLine("로그인 성공!");

            bool condition = true;
            while (condition)
            {
                driver.Url = "https://m.sofrano.com/product/list.html?cate_no=55";


                var products = driver.FindElements(By.XPath("//ul[@class='prdList grid2']/*"));
                string[] oldIds = { "553", "555", "839" };

                foreach (var product in products)
                {
                    if (!oldIds.Any(s => product.GetAttribute("id").Contains(s)))
                    {
                        condition = false;
                        var href = product.FindElement(By.XPath("div[@class='thumbnail']/a")).GetAttribute("href");
                        driver.Url = href;
                        break;
                    }
                }
                if (!condition)
                    break;
                await Task.Delay(TimeSpan.FromMinutes(5));
            }
            var bot = new TelegramBotClient("631711537:AAG164NGqHcdoYUnV-0QzKqE0B39z4a0U_0");
            await bot.SendTextMessageAsync("581720134", "스위치 동숲에디션 떴다 소프라노몰로 달려라");
            await bot.SendTextMessageAsync("991922024", "스위치 동숲에디션 떴다 소프라노몰로 달려라");
            return;
            driver.FindElement(By.ClassName("btnSubmit")).Click();


            Console.WriteLine("상품 들어가서 결제하기까지 직접 눌러주세요.");

            loop:
            try 
            {
                wait.Until(d =>
                {
                    var loadingBar = d.FindElements(By.Id("authssl_loadingbar")).FirstOrDefault();

                    if (loadingBar != null)
                        return !loadingBar.Displayed;
                    return false;
                });
            }
            catch(UnhandledAlertException)
            {
                goto loop;
            }
            Console.WriteLine("주문창 인식됨. 이제 움직이지 마세요.");

            var paymentButton = driver.FindElement(By.Id("btn_payment"));
            actions.MoveToElement(paymentButton);
            actions.Perform();
            await Task.Delay(150);

            driver.FindElement(By.Id("addr_paymethod3")).Click(); //결제방식 무통장입금
            await Task.Delay(150);


            actions.MoveToElement(paymentButton);
            actions.Perform();

            driver.FindElement(By.Id("pname")).SendKeys(name); //입금자명
            new SelectElement(driver.FindElement(By.Id("bankaccount"))).SelectByIndex(1); //국민은행
            driver.FindElement(By.Id("chk_purchase_agreement0")).Click();
            paymentButton.Click();

            Console.WriteLine("주문완료");
            //var bot = new TelegramBotClient("631711537:AAG164NGqHcdoYUnV-0QzKqE0B39z4a0U_0");
            //await bot.SendTextMessageAsync("581720134", "스위치 먹음");
            Console.ReadLine();
*/
        }
    }
}
