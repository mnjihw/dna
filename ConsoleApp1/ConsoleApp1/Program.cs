using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using PdfSharp.Pdf;
using PdfSharp.Drawing;

namespace ConsoleApp1
{
    public enum Suit
    {
        Club, Heart, Diamond, Spade
    }
    public enum Rank
    {
        Two = 2, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace
    }
    public enum Ranking
    {
        Top, OnePair, TwoPair, ThreeOfAKind, Straight, BackStraight, Mountain, Flush, FullHouse, FourOfAKind, StraightFlush, RoyalStraightFlush
    }
    public class Result
    {
        public Ranking Ranking { get; set; } = Ranking.Top;
        public Rank Rank { get; set; }
        public Suit Suit { get; set; }

        public override string ToString()
        {
            return $"{Rank} {Suit} {Ranking}";
        }
    }
    /*
     * 로티플: 10 J Q K A + 같은 모양 5개 @@@@@@@@@@@@@@@@@@@@@@@
     * 스티플: 숫자 연속 5개 + 같은 모양 5개 @@@@@@@@@@@@@@@@@@@@@@@
     * 풀하우스: 같은 숫자 3개 + 같은 숫자 2개 @@@@@@@@@@@@@@@@@@@@@@@
     * 포카드: 같은 숫자 4개 @@@@@@@@@@@@@@@@@@@@@@@
     * 플러시: 같은 모양 5개 @@@@@@@@@@@@@@@@@@@@@@@
     * 마운틴: 10 J Q K A
     * 백스트: A 2 3 4 5
     * 스트레이트: 연속 숫자 5개
     * 트리플: 같은 숫자 3개 @@@@@@@@@@@@@@@@@@@@@@@
     * 투페어: 원페어 2개 @@@@@@@@@@@@@@@@@@@@@@@
     * 원페어: 같은 숫자 1쌍 @@@@@@@@@@@@@@@@@@@@@@@
     * 탑: 없음 @@@@@@@@@@@@@@@@@@@@@@@
     */
    public class Card
    {
        public Rank Rank { get; set; }
        public Suit Suit { get; set; }
        
        public Card(int rank, int suit)
        {
            Rank = (Rank)rank;
            Suit = (Suit)suit;
        }

        public Card()  
        {
        }

        public override string ToString()
        {
            string suits = "♣♥◆♠";
            string[] ranks = { "", "", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};

            return $"{suits[(int)Suit]} {ranks[(int)Rank]}";
        }

        public static Result JudgeCards(List<Card> cards)
        {
            var backStraightFlushDeck = new List<Card>
            { 
                new Card{Rank = Rank.Ace},
                new Card{Rank = Rank.Five},
                new Card{Rank = Rank.Four},
                new Card{Rank = Rank.Three},
                new Card {Rank = Rank.Two}
            };
            var cardsBySuit = new List<Card>[4];
            var cardsByRank = new List<Card>[15];
            var rankCount = new int[15];
            var result = new Result();
            int count;

            cardsByRank[1] = cardsByRank[0] = new List<Card>();

            cards = cards.OrderByDescending(c => c.Rank).ThenByDescending(c => c.Suit).ToList();
            
            for (var suit = Suit.Club; suit <= Suit.Spade; ++suit)
                cardsBySuit[(int)suit] = (from cardOfSameSuit in cards where cardOfSameSuit.Suit == suit orderby cardOfSameSuit.Rank descending select cardOfSameSuit).ToList();
            for(var rank = Rank.Two; rank <= Rank.Ace; ++rank)
                cardsByRank[(int)rank] = (from cardOfSameRank in cards where cardOfSameRank.Rank == rank orderby cardOfSameRank.Suit descending select cardOfSameRank).ToList();

            
            foreach (var suit in cardsBySuit)
            {
                count = 1;

                if (suit.Count < 5)
                    continue;
                result.Rank = suit[0].Rank;
                result.Suit = suit[0].Suit;

                for(int i = 0; i < suit.Count - 1; ++i)
                {
                    if (suit[i].Rank == suit[i + 1].Rank + 1)
                        ++count;
                    else
                        count = 1;

                    if(count == 5)
                    {
                        if (suit[0].Rank == Rank.Ace)
                            result.Ranking = Ranking.RoyalStraightFlush;
                        else
                            result.Ranking = Ranking.StraightFlush;
                        return result; 
                    }
                }

                if(suit.Count == 5)
                    result.Ranking = Ranking.Flush;
            }
            bool isOnePair = false, isThreeOfAKind = false;

            for (int i = 0; i < cardsByRank.Length; ++i)
            {
                if ((rankCount[i] = cardsByRank[i].Count) == 0)
                    continue;

                if (rankCount[i] == 2)
                {
                    if (isThreeOfAKind)
                    {
                        result.Ranking = Ranking.FullHouse;
                        return result;
                    }
                    else if (isOnePair)
                    {
                        result.Ranking = Ranking.TwoPair;
                        result.Rank = cardsByRank[i][0].Rank;
                        result.Suit = cardsByRank[i][0].Suit;
                    }
                    else
                    {
                        isOnePair = true;
                        result.Ranking = Ranking.OnePair;
                        result.Rank = cardsByRank[i][0].Rank;
                        result.Suit = cardsByRank[i][0].Suit;
                    }
                }
                else if (rankCount[i] == 3)
                {
                    if (isOnePair || isThreeOfAKind)
                    {
                        result.Ranking = Ranking.FullHouse;
                        result.Rank = cardsByRank[i][0].Rank;
                        result.Suit = cardsByRank[i][0].Suit;
                        return result;
                    }
                    else
                    {
                        isThreeOfAKind = true;
                        result.Ranking = Ranking.ThreeOfAKind;
                        result.Rank = cardsByRank[i][0].Rank;
                        result.Suit = cardsByRank[i][0].Suit;
                    }
                }
                else if(rankCount[i] == 4) 
                {
                    result.Ranking = Ranking.FourOfAKind;
                    result.Rank = cardsByRank[i][0].Rank;
                    result.Suit = cardsByRank[i][0].Suit;
                    return result;   
                }
            }

            Card start = cards[0];
            count = 1;

            for(int i = 0; i < cards.Count - 1; ++i)
            {
                if(cards[i].Rank - cards[i + 1].Rank > 1)
                {
                    count = 1;
                    start = cards[i + 1];
                    continue;
                }
                if (cards[i] == start && start.Rank == cards[i + 1].Rank && start.Suit < cards[i + 1].Suit)
                    start = cards[i + 1];
                if (cards[i].Rank == cards[i + 1].Rank + 1)
                    ++count;
                if(count == 5)
                {
                    result.Rank = start.Rank;
                    result.Suit = start.Suit;

                    if (start.Rank == Rank.Ace)
                        result.Ranking = Ranking.Mountain;
                    else
                    {
                        if (backStraightFlushDeck.All(c => cards.Contains(c)))
                        {
                            result.Ranking = Ranking.BackStraight;
                            result.Rank = cards[0].Rank;
                            result.Suit = cards[0].Suit;
                        }
                        else
                            result.Ranking = Ranking.Straight;
                    }
                    return result;
                }
            }

            if(backStraightFlushDeck.All(c => cards.Contains(c)))
            {
                result.Ranking = Ranking.BackStraight;
                result.Rank = cards[0].Rank;
                result.Suit = cards[0].Suit;
            }

            if(result.Ranking == Ranking.Top)
            {
                result.Rank = cards[0].Rank;
                result.Suit = cards[0].Suit;
            }

            return result;
        }
    }
    class Program
    {
        

        static  void Main(string[] args)
        {
            PdfDocument doc = new PdfDocument();
            var page = doc.AddPage();

            var width = 4032 / 4;
            var height = 3024 / 4;
            page.Width = width;
            page.Height = height;
            XGraphics xgr = XGraphics.FromPdfPage(page);
            XImage img = XImage.FromFile(@"C:\Users\mnjihw\Desktop\20200527_231155.jpg");
            xgr.DrawImage(img, 0, 0, width, height);

            doc.Save(@"C:\Users\mnjihw\Desktop\1.pdf");
            doc.Close();

            return;

            

            Console.WriteLine("끝");
            Console.ReadLine();

            return;
            var cards = new List<Card>();
            Random r = new Random();
            
            while(true)
            {
                for (int i = 0; i < 7; ++i)
                    cards.Add(new Card(r.Next() % 13 + 2, r.Next() % 4));

                cards = cards.OrderByDescending(c => c.Rank).ThenByDescending(c => c.Suit).ToList();


                var result = Card.JudgeCards(cards);

                foreach(var card in cards)
                    Console.WriteLine(card);
                Console.WriteLine(result);

                if(result.Ranking > Ranking.FourOfAKind)
                {
                    Thread.Sleep(5000);
                }

                Console.Clear();
                cards.Clear();
            }


        }
    }
}
