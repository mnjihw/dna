using System;
using System.Collections.Generic;
using System.Text;

namespace 유튜브_댓글
{
    public class Comment
    {
        public string Author { get; set; }
        public string AuthorId { get; set; }
        public string Id { get; set; }

        public string Content { get; set; }

        public List<Comment> SubComments;
    }
}
