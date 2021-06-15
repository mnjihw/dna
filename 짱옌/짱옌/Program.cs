using System;
using System.Linq;
using System.Text;


namespace 짱옌
{
    class Program
    {
        private static Random Random { get; } = new Random();
        static void Main()
        {
            var sb = new StringBuilder(512);
            Console.Title = "짱옌";

            while (true)
            {

                Console.WriteLine("엔터 누르면 숫자 5개 뽑음");
                Console.ReadLine();
                sb.Clear();
                sb.AppendJoin(", ", Enumerable.Range(1, 10).OrderBy(x => Random.Next()).Take(5).OrderBy(x => x));
                Console.WriteLine(sb);
                Console.WriteLine();
            }
            
            
        }
    }
}
