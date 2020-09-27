using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using android_test.Droid;

[assembly: Xamarin.Forms.Dependency(typeof(Toast_Android))]

namespace android_test.Droid
{
    public class Toast_Android : IToast
    {
        public void Show(string message)
        {
            var toast = Toast.MakeText(Application.Context, message, ToastLength.Long);
            toast.SetGravity(GravityFlags.Top, 0, 0);
            toast.Show();
        }
    }
}