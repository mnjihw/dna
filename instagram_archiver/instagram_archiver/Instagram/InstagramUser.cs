using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Instagram_archiver.Instagram
{
    /*  public class InstagramUser
      {
          public string Id { get; set; }
          public string Name { get; set; }
      }*/
    public class InstagramUser
    {
        public string Id { get; set; }
        public string Name { get; set; }
        public List<InstagramPost> Posts { get; set; }

    }
}
