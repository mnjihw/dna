using System;
using System.Collections.Generic;
using System.Text;

namespace android_test
{
    public interface INotification
    {
        void Notify(string title, string text, string url);
        
    }
}
