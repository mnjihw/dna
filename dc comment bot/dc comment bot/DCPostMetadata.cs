using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace dc_comment_bot
{
    public class DCPostMetadata : IPostMetadata
    {
        public string GalleryName { get; set; }
        public int PostId { get; set; }
        public bool IsMinor { get; set; }

    }
}
