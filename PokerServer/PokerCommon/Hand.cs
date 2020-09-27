using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokerCommon
{
    [Serializable]
    public class Hand
    {
        private readonly List<Card> hand = new List<Card>();
        private readonly List<Card> sortedHand = new List<Card>();
        public static readonly List<Card> stub = new List<Card>();
        [NonSerialized]
        private static readonly Random random = new Random();
        [NonSerialized]
        private readonly List<Card>[] cardsByRank = new List<Card>[15];
        [NonSerialized]
        private readonly List<Card>[] cardsBySuit = new List<Card>[4];
        [NonSerialized] 
        private readonly HandRanking ranking = new HandRanking();
        [NonSerialized]
        private List<Card> cardsIncludingFlush;
        [NonSerialized]
        private List<Card> cardsIncludingStraight; 

        #region Ranking checking routine

        private bool IsFourOfAKind
        {
            get
            {
                if (hand.Count < 4)
                    return false;
                foreach (var cards in cardsByRank.Reverse())
                {
                    if (cards.Count >= 4)
                    {
                        ranking.Rank = cards[0].Rank;
                        ranking.Suit = cards[0].Suit;
                        return true;
                    }
                }
                return false;
            }
        }

        private bool IsFullHouse
        {
            get
            {
                bool hasThreeOfAKind = false, hasOnePair = false;

                if (hand.Count < 5)
                    return false;

                foreach (var cards in cardsByRank.Reverse())
                {
                    if (cards.Count == 3)
                    {
                        if (hasThreeOfAKind) //3 3으로 풀하 나온 경우
                            return true;
                        ranking.Rank = cards[0].Rank;
                        ranking.Suit = cards[0].Suit;

                        if (hasOnePair)
                            return true;
                        hasThreeOfAKind = true;
                    }
                    else if (cards.Count == 2)
                    {
                        if (hasThreeOfAKind)
                            return true;
                        hasOnePair = true;
                    }
                }
                return false;
            }
        }


        private bool IsFlush
        {
            get
            {
                if (hand.Count < 5)
                    return false;
                foreach (var cards in cardsBySuit) //Reverse 무의미함
                {
                    if (cards.Count >= 5)
                    {
                        ranking.Rank = cards[0].Rank;
                        ranking.Suit = cards[0].Suit;
                        cardsIncludingFlush = cards;
                        return true;
                    }
                }
                return false;
            }
        }

        private bool IsStraight
        {
            get
            {
                if (hand.Count < 5)
                    return false;
                var cards = cardsIncludingFlush ?? new SortedSet<Card>(sortedHand).ToList();
                

                for (int i = 0; i < cards.Count - 4; ++i)
                {
                    var fiveCards = cards.Skip(i).Take(5);

                    if (fiveCards.First().Rank == fiveCards.Last().Rank + 4)
                    {
                        ranking.Rank = fiveCards.First().Rank;
                        ranking.Suit = fiveCards.First().Suit;
                        cardsIncludingStraight = fiveCards.ToList();
                        return true;
                    }
                }
                for (int i = 0; i < cards.Count - 3; ++i)
                {
                    var fourCards = cards.Skip(i).Take(4);

                    if (fourCards.First().Rank == Rank.Five && fourCards.Last().Rank == Rank.Two && cards[0].Rank == Rank.Ace)
                    {
                        ranking.Rank = cards[0].Rank;
                        ranking.Suit = cards[0].Suit;
                        cardsIncludingStraight = fourCards.Prepend(cards[0]).ToList();
                        return true;
                    }
                }
                return false;
            }
        }
        private bool IsThreeOfAKind
        {
            get
            {
                if (hand.Count < 3)
                    return false;
                foreach (var cards in cardsByRank.Reverse())
                {
                    if (cards.Count == 3)
                    {
                        ranking.Rank = cards[0].Rank;
                        ranking.Suit = cards[0].Suit;
                        return true;
                    }
                }
                return false;
            }
        }

        private bool IsTwoPair
        {
            get
            {
                int pairCount = 0;

                foreach (var cards in cardsByRank.Reverse())
                {
                    if (cards.Count == 2)
                    {
                        if (++pairCount == 1)
                        {
                            ranking.Rank = cards[0].Rank;
                            ranking.Suit = cards[0].Suit;
                        }
                    }
                }

                return pairCount >= 2;
            }
        }
        private bool IsOnePair
        {
            get
            {
                foreach (var cards in cardsByRank.Reverse())
                {
                    if (cards.Count == 2)
                    {
                        ranking.Rank = cards[0].Rank;
                        ranking.Suit = cards[0].Suit;
                        return true;
                    }
                }
                return false;
            }
        }
        #endregion


        static Hand() => Initialize();

        public int Count => hand.Count;
        private static void Initialize()
        {
            if (stub.Count == 52)
                return;

            stub.Clear();

            for (var suit = Suit.Club; suit <= Suit.Spade; ++suit)
                for (var rank = Rank.Two; rank <= Rank.Ace; ++rank)
                    stub.Add(new Card(rank, suit));
        }

        public void Clear()
        {
            Initialize();
            hand.Clear();
            sortedHand.Clear();
            cardsIncludingFlush = null;
            cardsIncludingStraight = null;
            Array.Clear(cardsByRank, 0, cardsByRank.Length);
            Array.Clear(cardsBySuit, 0, cardsBySuit.Length);
        }

        public void Remove(int index)
        {
            var card = hand[index];

            hand.RemoveAt(index);
            sortedHand.Remove(card);
        }
        

        public void TakeFromStub(int count)
        {
            if (stub.Count == 0)
                throw new Exception("stub empty");

            for (int i = 0; i < count; ++i)
            {
                var card = stub[random.Next(stub.Count)];
                int index = ~sortedHand.BinarySearch(card, Comparer<Card>.Create(CardComparer));
                sortedHand.Insert(index, card);
                hand.Add(card);
                stub.Remove(card);
            }
        }

        private int CardComparer(Card x, Card y)
        {
            if (x.Rank < y.Rank)
                return 1;
            else if (x.Rank > y.Rank)
                return -1;
            else
                return y.Suit - x.Suit;
        }

        public HandRanking Judge()
        {
            
            ranking.Ranking = Ranking.Top;
            ranking.Rank = sortedHand[0].Rank;
            ranking.Suit = sortedHand[0].Suit;

            cardsByRank[0] = new List<Card>();
            cardsByRank[1] = new List<Card>();

            for (var suit = Suit.Club; suit <= Suit.Spade; ++suit)
                cardsBySuit[(int)suit] = (from cardsOfSameSuit in sortedHand where cardsOfSameSuit.Suit == suit orderby cardsOfSameSuit.Rank descending select cardsOfSameSuit).ToList();
            for (var rank = Rank.Two; rank <= Rank.Ace; ++rank)
                cardsByRank[(int)rank] = (from cardsOfSameRank in sortedHand where cardsOfSameRank.Rank == rank orderby cardsOfSameRank.Suit descending select cardsOfSameRank).ToList();

            if (IsFourOfAKind)
                ranking.Ranking = Ranking.FourOfAKind;
            else if (IsFullHouse)
                ranking.Ranking = Ranking.FullHouse;
            else if (IsFlush)
            {
                if (IsStraight)
                {
                    if (cardsIncludingStraight[0].Rank == Rank.Ace)
                    {
                        if (cardsIncludingStraight[1].Rank == Rank.King)
                            ranking.Ranking = Ranking.RoyalStraightFlush;
                        else
                            ranking.Ranking = Ranking.BackStraightFlush;
                    }
                    else
                        ranking.Ranking = Ranking.StraightFlush;
                }
                else
                    ranking.Ranking = Ranking.Flush;
            }
            else if (IsStraight)
            {
                if (cardsIncludingStraight[0].Rank == Rank.Ace)
                {
                    if (cardsIncludingStraight[1].Rank == Rank.King)
                        ranking.Ranking = Ranking.Mountain;
                    else
                        ranking.Ranking = Ranking.BackStraight;
                }
                else
                    ranking.Ranking = Ranking.Straight;
            }
            else if (IsThreeOfAKind)
                ranking.Ranking = Ranking.ThreeOfAKind;
            else if (IsTwoPair)
                ranking.Ranking = Ranking.TwoPair;
            else if (IsOnePair)
                ranking.Ranking = Ranking.OnePair;
            return ranking;

        }
        public void Add(Card card)
        {
            int index = ~sortedHand.BinarySearch(card, Comparer<Card>.Create(CardComparer));
            sortedHand.Insert(index, card);
            hand.Add(card);
            stub.Remove(card);
        }
        public void Print()
        {
            string suits = "♣♥◆♠";
            string[] ranks = { "", "", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
            int i = 0;

            foreach (var card in hand)
            {
                Console.WriteLine($"{++i}. {suits[(int)card.Suit]} {ranks[(int)card.Rank]}");
            }

        }

    }
}
