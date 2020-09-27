using OpenQA.Selenium;
using OpenQA.Selenium.Edge;
using OpenQA.Selenium.Support.UI;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace 멜론_로그인
{
    class Program
    {
        static void Main()
        {
            var edgeOptions = new EdgeOptions 
            {
                UseChromium = true,
                BinaryLocation = @"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe"
            };

            var service = EdgeDriverService.CreateDefaultServiceFromOptions(Directory.GetCurrentDirectory(), "msedgedriver.exe", edgeOptions);
             
            //service.HideCommandPromptWindow = true;
            bool condition = false;

            using IWebDriver driver = new EdgeDriver(service, edgeOptions)
            {
                //Url = "https://member.melon.com/muid/web/login/login_inform.htm"
                //Url  = "https://ticket.melon.com/main/index.htm"
                Url = "https://www.naver.com"
            };
            Console.ReadLine();


            Stopwatch s = new Stopwatch();
            s.Start();
            var d = driver.SwitchTo().Window(driver.WindowHandles[1]); 
            s.Stop();
            Console.WriteLine("DD " + s.ElapsedMilliseconds);
            s.Reset();
            s.Start();
            //var c = driver.FindElements(By.Id("certification"));
            var c = d.FindElements(By.Id("certification")); //timed out connecting to chrome 떠서 4초 걸리는거임 이거 해결하자


            s.Stop();
            Console.WriteLine("DD2 " +s.ElapsedMilliseconds);
            Console.ReadLine();
            


            return;

            /*while (true)
            {
                Thread.Sleep(50);
            }*/
            Console.ReadLine();
            goto a;
            Console.WriteLine("공연 날짜 8글자로 입력(ex) 20200326, 0 입력 시 가장 빠른 날짜로 선택됨.)");
            var day = Console.ReadLine();
            if(day.Length != 1 && day.Length != 8)
            {
                Console.WriteLine("잘못 입력함");
                return;
            }
            Console.WriteLine("공연 시간 4글자로 입력(ex) 1930, 0 입력 시 가장 빠른 시간으로 선택됨.)");
            var time = Console.ReadLine();
            if(time.Length != 1 && time.Length != 4)
            {
                Console.WriteLine("잘못 입력함");
                return;
            }

            Console.WriteLine("공연 페이지에서 엔터 눌러라");
            
            Console.ReadLine();

            IWebElement element = null;

            if (day != "0")
            {
                element = driver.FindElements(By.XPath(@$"//li[@data-perfday='{day}']")).FirstOrDefault();
                if(element == null)
                {
                    Console.WriteLine("먼가 잘못 입력함");
                    return;
                }
            }

            if(element == null) //익셉션이 뜰 수도 있음
            {
                element = driver.FindElements(By.XPath(@"//ul[@id='list_date']/[1]")).FirstOrDefault();
                if(element == null)
                {
                    Console.WriteLine("먼가 잘못 입력함");
                    return;
                }
            }
            element.Click();
            element = null;

            if (time != "0")
            {
                var hour = time.Substring(0, 2);
                var minute = time.Substring(2, 2);

                element = driver.FindElements(By.XPath(@$"//li[contains(@class, 'item_time')]/button/span[contains(text(), '{hour}시 {minute}분')]")).FirstOrDefault();
                if(element == null)
                {
                    Console.WriteLine("먼가 잘못 입력함");
                    return;
                }
            }

            if(element == null)
            {
                element = driver.FindElements(By.XPath(@"//li[contains(@class, 'item_time')]")).FirstOrDefault();
                if(element == null)
                {
                    Console.WriteLine("먼가 잘못 입력함");
                    return;
                }
            }
            element.Click();
            element = driver.FindElements(By.Id("ticketReservation_Btn")).FirstOrDefault();
            if(element == null)
            {
                Console.WriteLine("머냐");
                return;
            }
            element.Click();

        a:
            driver.SwitchTo().Window(driver.WindowHandles[1]);
            var wait = new WebDriverWait(driver, TimeSpan.FromSeconds(300));

            if (driver.FindElements(By.Id("certification")).Count != 0) //이게 4초걸림   
            {
                driver.FindElement(By.XPath(@"//label[@for='label-for-captcha']")).Click();
                wait.Until(d => !d.FindElement(By.Id("certification")).Displayed);
            }
            driver.SwitchTo().Frame("oneStopFrame");

            if (driver.FindElements(By.Id("partTicketType")).Count == 0) //지정석
            {
                var center = int.Parse(driver.FindElement(By.XPath(@"//*[name()='image']")).GetAttribute("width")) >> 1;

                while (true)
                {
                    var seats = driver.FindElements(By.XPath(@"//*[name()='rect'][not(@fill='#DDDDDD') and not(@fill='none')]"));
                    var a = seats.ToList();
                    a.Sort((s1, s2) =>
                    {
                        double difference = (int)(double.Parse(s1.GetAttribute("y")) - double.Parse(s2.GetAttribute("y")));

                        if (difference > 0)
                            return 1;
                        else if (difference < 0)
                            return -1;
                        else
                        {
                            difference = Math.Abs(center - double.Parse(s1.GetAttribute("x"))) - Math.Abs(center - double.Parse(s2.GetAttribute("x")));
                            if (difference > 0)
                                return 1;
                            else
                                return -1;
                        }
                    });

                    for (int i = 0; i < 5; ++i)
                    {
                        a[i].Click();
                        driver.FindElement(By.Id("nextTicketSelection")).Click();
                        try
                        {
                            var alert = driver.SwitchTo().Alert();
                            alert.Accept(); //이미 선택된 좌석
                        }
                        catch (NoAlertPresentException)
                        {
                            condition = true;
                            break;
                        }
                    }

                    if (condition)
                        break;
                    driver.FindElement(By.Id("btnReloadSchedule")).Click(); //새로고침

                    while (true)
                    {
                        bool ajaxCompleted = (bool)(driver as IJavaScriptExecutor).ExecuteScript("return jQuery.active == 0");

                        if (ajaxCompleted)
                            break;
                        Thread.Sleep(100);
                    }


                }
            }

            Console.WriteLine("응12 " + DateTime.Now.ToLongTimeString());
            new SelectElement(driver.FindElement(By.XPath(@"//dd[@class='price wrap_sel']/select"))).SelectByValue("1");
            driver.FindElement(By.Id("nextPayment")).Click(); //다음
            driver.FindElement(By.Id("payMethodCode003")).Click(); //무통장입금
            driver.FindElement(By.Id("cashReceiptIssueCode3")).Click(); //현금영수증 미발행
            driver.FindElement(By.Id("chkAgreeAll")).Click(); //전체동의
            driver.FindElement(By.Id("chkAgreeChannel")).Click(); //[선택] 카카오톡 멜론티켓 채널 추가
            Console.ReadLine();
            //driver.FindElement(By.Id("btnFinalPayment")).Click(); //결제하기


            Console.ReadLine();

        }
    }
}
