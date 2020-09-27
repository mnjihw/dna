using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace coinbottest
{
    public class PricesByExchange
    {
        public string Name { get; set; }
        public double Binance { get; set; }
        public double Bithumb { get; set; }
        public double Gateio { get; set; }
        public double Huobi { get; set; }
        public double Upbit { get; set; }

       

        public override string ToString()
        {
            string s = "";
            //return $"Binance: {Binance} Bithumb: {Bithumb} Gateio: {Gateio} Huobi: {Huobi} Upbit: {Upbit}";

            //바이낸스가 있으면 "Binance: "출력
            //없으면 아무것도 안출력
            //??연산자는 널일 경우 발동되는거
            //

            s += $"{(Binance == 0 ? "" : "Binance: ")}{Binance} ";
            s += $"{(Bithumb == 0 ? "" : "Bithumb: ")}{Bithumb} ";
            s += $"{(Gateio == 0 ? "" : "Gateio: ")}{Gateio} ";
            s += $"{(Huobi == 0 ? "" : "Huobi: ")}{Huobi} ";
            s += $"{(Upbit == 0 ? "" : "Upbit: ")}{Upbit} ";
            return s;

        }
    }
}
