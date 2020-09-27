using System;
using System.Collections.Generic;
using System.Text;

namespace forebet_console
{
    public static class DateTimeExtensions
    {
        public static string FormatAsDatebaseDateTime(this DateTime dateTime) => $"{dateTime:yyyy-MM-dd HH:mm:ss}";
    }
}
