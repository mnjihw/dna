using System;

namespace 리듬_메이플_BGM_플레이어
{
    public class BGMListItem : ICloneable
    {
        public int Number { get; set; }
        public string Title { get; set; }
        public string Path { get; set; }
        public string Comment { get; set; }
        public string Location { get; set; }

        public object Clone()
        {
            BGMListItem item = new BGMListItem
            {
                Number = Number,
                Title = Title,
                Path = Path,
                Comment = Comment
            };

            return item;
        }
       
    }
}
