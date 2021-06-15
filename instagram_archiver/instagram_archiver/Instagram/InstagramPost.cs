using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Instagram_archiver.Instagram
{
    public class InstagramPost
    {
        /*public string Id { get; set; }
        public string Content { get; set; }
        public InstagramUser Writer { get; set; }*/
        public string Id { get; set; }
        public string Content { get; set; }
        public List<InstagramImage> Images { get; set; }
    }
}
