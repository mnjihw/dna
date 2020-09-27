using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Support.V4.App;
using Android.Views;
using Android.Widget;
using android_test.Droid;

[assembly: Xamarin.Forms.Dependency(typeof(Notification_Android))]
namespace android_test.Droid
{
    public class Notification_Android : INotification
    {
        public void Notify(string title, string text, string url)
        {
            var intent = new Intent(Intent.ActionView, Android.Net.Uri.Parse(url));
            PendingIntent pendingIntent = PendingIntent.GetActivity(Application.Context, 0, intent, PendingIntentFlags.OneShot);

            var manager = NotificationManagerCompat.From(Application.Context);
            var notification = new NotificationCompat.Builder(Application.Context, "1000")
                    .SetContentTitle(title) //생략 가능
                    .SetContentText(text) //생략 가능
                    .SetSmallIcon(Resource.Drawable.abc_btn_default_mtrl_shape) //이거 없으면 TargetInvocationException 뜬다
                    .SetContentIntent(pendingIntent)
                    .SetAutoCancel(true)
                    .Build();

            manager.Notify((int)SystemClock.UptimeMillis(), notification);
        }
         

   
    }
}