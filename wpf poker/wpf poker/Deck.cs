using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace wpf_poker
{
    public class Deck
    {
        private static readonly List<Card> unpickedDeck = new List<Card>();
        private static readonly List<Card> pickedDeck = new List<Card>();
        private static readonly Random random = new Random();
        private readonly List<Card> deck = new List<Card>();
        private readonly List<Card>[] cardsByRank = new List<Card>[15];
        private readonly List<Card>[] cardsBySuit = new List<Card>[4];
        private readonly Result result = new Result();
        private List<Card> cardsHavingFlush;
        private List<Card> cardsHavingStraight;

        #region Ranking checking routine

        private bool IsFourOfAKind
        {
            get
            {
                if (deck.Count < 4)
                    return false;
                foreach (var cards in cardsByRank.Reverse())
                {
                    if (cards.Count >= 4)
                    {
                        result.Rank = cards[0].Rank;
                        result.Suit = cards[0].Suit;
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

                if (deck.Count < 5)
                    return false;

                foreach (var cards in cardsByRank.Reverse())
                {
                    if (cards.Count == 3)
                    {
                        if (hasThreeOfAKind) //3 3으로 풀하 나온 경우
                            return true;
                        result.Rank = cards[0].Rank;
                        result.Suit = cards[0].Suit;

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
                if (deck.Count < 5)
                    return false;
                foreach (var cards in cardsBySuit) //Reverse 무의미함
                {
                    if (cards.Count >= 5)
                    {
                        result.Rank = cards[0].Rank;
                        result.Suit = cards[0].Suit;
                        cardsHavingFlush = cards;
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
                if (deck.Count < 5)
                    return false;
                var cards = cardsHavingFlush ?? new SortedSet<Card>(deck).ToList();

                for (int i = 0; i < cards.Count - 4; ++i)
                {
                    var fiveCards = cards.Skip(i).Take(5);

                    if (fiveCards.First().Rank == fiveCards.Last().Rank + 4)
                    {
                        result.Rank = fiveCards.First().Rank;
                        result.Suit = fiveCards.First().Suit;
                        cardsHavingStraight = fiveCards.ToList();
                        return true;
                    }
                }
                for (int i = 0; i < cards.Count - 3; ++i)
                {
                    var fourCards = cards.Skip(i).Take(4);

                    if (fourCards.First().Rank == Rank.Five && fourCards.Last().Rank == Rank.Two && cards[0].Rank == Rank.Ace)
                    {
                        result.Rank = cards[0].Rank;
                        result.Suit = cards[0].Suit;
                        cardsHavingStraight = fourCards.Prepend(cards[0]).ToList();
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
                if (deck.Count < 3)
                    return false;
                foreach (var cards in cardsByRank.Reverse())
                {
                    if (cards.Count == 3)
                    {
                        result.Rank = cards[0].Rank;
                        result.Suit = cards[0].Suit;
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
                            result.Rank = cards[0].Rank;
                            result.Suit = cards[0].Suit;
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
                        result.Rank = cards[0].Rank;
                        result.Suit = cards[0].Suit;
                        return true;
                    }
                }
                return false;
            }
        }
        #endregion


        static Deck() => Initialize();


        private static void Initialize()
        {
            if (unpickedDeck.Count == 52)
                return;

            unpickedDeck.Clear();
            pickedDeck.Clear();

            for (var suit = Suit.Club; suit <= Suit.Spade; ++suit)
                for (var rank = Rank.Two; rank <= Rank.Ace; ++rank)
                    unpickedDeck.Add(new Card(rank, suit));
        }

        public void Clear()
        {
            Initialize();
            deck.Clear();
            cardsHavingFlush = null;
            cardsHavingStraight = null;
            Array.Clear(cardsByRank, 0, cardsByRank.Length);
            Array.Clear(cardsBySuit, 0, cardsBySuit.Length);
        }

        public void Remove(int index) => deck.RemoveAt(index);
        public ReadOnlyCollection<Card> GetDeck() => deck.AsReadOnly();

        public void Pick()
        {
            if (unpickedDeck.Count == 0)
                throw new Exception("unpickedDeck empty");

            var card = unpickedDeck[random.Next(unpickedDeck.Count)];
            int index = ~deck.BinarySearch(card, Comparer<Card>.Create(CardComparer));

            deck.Insert(index, card);
            pickedDeck.Add(card);
            unpickedDeck.Remove(card);
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

        public Result Judge()
        {
            result.Ranking = Ranking.Top;
            result.Rank = deck[0].Rank;
            result.Suit = deck[0].Suit;

            cardsByRank[0] = new List<Card>();
            cardsByRank[1] = new List<Card>();

            for (var suit = Suit.Club; suit <= Suit.Spade; ++suit)
                cardsBySuit[(int)suit] = (from cardsOfSameSuit in deck where cardsOfSameSuit.Suit == suit orderby cardsOfSameSuit.Rank descending select cardsOfSameSuit).ToList();
            for (var rank = Rank.Two; rank <= Rank.Ace; ++rank)
                cardsByRank[(int)rank] = (from cardsOfSameRank in deck where cardsOfSameRank.Rank == rank orderby cardsOfSameRank.Suit descending select cardsOfSameRank).ToList();

            if (IsFourOfAKind)
                result.Ranking = Ranking.FourOfAKind;
            else if (IsFullHouse)
                result.Ranking = Ranking.FullHouse;
            else if (IsFlush)
            {
                if (IsStraight)
                {
                    if (cardsHavingStraight[0].Rank == Rank.Ace)
                    {
                        if (cardsHavingStraight[1].Rank == Rank.King)
                            result.Ranking = Ranking.RoyalStraightFlush;
                        else
                            result.Ranking = Ranking.BackStraightFlush;
                    }
                    else
                        result.Ranking = Ranking.StraightFlush;
                }
                else
                    result.Ranking = Ranking.Flush;
            }
            else if (IsStraight)
            {
                if (cardsHavingStraight[0].Rank == Rank.Ace)
                {
                    if (cardsHavingStraight[1].Rank == Rank.King)
                        result.Ranking = Ranking.Mountain;
                    else
                        result.Ranking = Ranking.BackStraight;
                }
                else
                    result.Ranking = Ranking.Straight;
            }
            else if (IsThreeOfAKind)
                result.Ranking = Ranking.ThreeOfAKind;
            else if (IsTwoPair)
                result.Ranking = Ranking.TwoPair;
            else if (IsOnePair)
                result.Ranking = Ranking.OnePair;
            return result;

        }
        public void Add(Card card)
        {
            int index = ~deck.BinarySearch(card, Comparer<Card>.Create(CardComparer));
            deck.Insert(index, card);
        }
        public void Print()
        {
            string suits = "♣♥◆♠";
            string[] ranks = { "", "", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
            int i = 0;

            foreach (var card in deck)
            {
                Console.WriteLine($"{++i}. {suits[(int)card.Suit]} {ranks[(int)card.Rank]}");
            }

        }


    }
}
