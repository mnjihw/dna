using System;
using System.IO;

namespace Instagram_archiver.Instagram
{
    public class InstagramImage
    { 
        public string Id { get; set; }
        public string Url { get; set; }
        public string Hash { get; set; }
        public Stream Stream { get; set; }

        //public InstagramPost Post { get; set; }

        public override string ToString() => //$"UserId: {Post.Writer.Id}{Environment.NewLine}" +
                //$"UserName: {Post.Writer.Name}{Environment.NewLine}" +  
                $"Id: {Id}" +
                $"Hash: {Hash}";
    }

}
