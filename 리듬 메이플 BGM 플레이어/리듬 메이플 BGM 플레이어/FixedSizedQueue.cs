using System.Collections.Generic;

namespace 리듬_메이플_BGM_플레이어
{
    class FixedSizedQueue<T> : Queue<T>
    {
        public FixedSizedQueue(int limit) { Limit = limit; }
        public int Limit { get; set; }

        public new void Enqueue(T item)
        {
            base.Enqueue(item);
            while (Count > Limit)
                Dequeue();
        }
    }
}
