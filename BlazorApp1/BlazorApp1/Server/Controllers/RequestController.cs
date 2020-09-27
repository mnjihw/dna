using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Threading.Tasks;

namespace BlazorApp1.Server.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class RequestController : ControllerBase
    {
        private static readonly string[] list = new[]
        {
            "김동현", "문지환", "이현재", "정덕영", "김민석"
        };
        
        [HttpGet]
        public IEnumerable<string> Get() 
        {
            Random r = new Random();
            return Enumerable.Range(1, 10).Select(idx => list[r.Next(list.Length)]).ToArray();
            /*
                        HttpClient client = new HttpClient();
                        return new[] { client.GetStringAsync("https://www.naver.com").Result, "" };*/
        }
    }
}
