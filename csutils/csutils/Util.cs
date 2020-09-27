using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Windows;

namespace csutils
{
    class Util
    {
        [DllImport("ImageSearch.dll")]
        private static extern IntPtr ImageSearch(int left, int top, int right, int bottom, [MarshalAs(UnmanagedType.LPStr)]string imagePath);

        public static (int, int) ImageSearch(Point pt1, Point pt2, string imagePath)
        {
            var result = Marshal.PtrToStringAnsi(ImageSearch((int)pt1.X, (int)pt1.Y, (int)pt2.X, (int)pt2.Y, imagePath));
            
            
            if (result[0] == '0')
                return (-1, -1);
            else
            {
                var slice = result.Split('|');
                return (int.Parse(slice[1]), int.Parse(slice[2]));
            }

        }

    }
}
