using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokerCommon
{
    public enum Rank
    {
        Two = 2, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace
    }
    public enum Suit
    {
        Club, Heart, Diamond, Spade
    }
    [Serializable]
    public class Card : IComparable<Card>
    {
        public Rank Rank { get; set; }
        public Suit Suit { get; set; }
        public bool IsVisible { get; set; } //나중에 구현

        public Card() { }
        public Card(int rank, int suit) : this((Rank)rank, (Suit)suit) { }
        public Card(Rank rank, Suit suit)
        {
            Rank = rank;
            Suit = suit;
        }
        public override string ToString()
        {
            string suits = "♣♥◆♠";
            string[] ranks = { "", "", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };

            return $"{suits[(int)Suit]} {ranks[(int)Rank]}";
        }

        public int CompareTo(Card other)
        {
            return other.Rank - Rank;
        }
    }
}
