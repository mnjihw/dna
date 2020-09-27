using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using System.Threading.Tasks;

namespace 리듬_키워드_알리미
{
    public interface IKeywordAlert
    {
        public ReadOnlyCollection<string> Keywords { get; }
        //public List<string> AlertedList { get; }
        public Task Run();

    }
}
