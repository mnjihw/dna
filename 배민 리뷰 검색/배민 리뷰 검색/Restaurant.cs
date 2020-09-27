using System;
using System.Collections.Generic;
using System.Text;

namespace 배민_리뷰_검색
{
    public class Restaurant
    {
        public string Name { get; set; }
        public int Id { get; set; }
        public override string ToString() => Name;
    }
}
