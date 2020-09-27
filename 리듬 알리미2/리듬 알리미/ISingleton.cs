using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace 리듬_알리미
{
    public interface ISingleton<T> where T : new()
    {
        Lazy<T> _instanceHolder => new Lazy<T>(() => new T()); 

    }
}
