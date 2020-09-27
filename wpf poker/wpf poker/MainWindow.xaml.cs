using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace wpf_poker
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    /// 

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
        Top, OnePair, TwoPair, ThreeOfAKind, Straight, BackStraight, Mountain, Flush, FullHouse, FourOfAKind, StraightFlush, BackStraightFlush, RoyalStraightFlush
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

    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {

        }

        private void Button2_Click(object sender, RoutedEventArgs e)
        {
            Deck deck = new Deck();
            for (int i = 0; i < 3; ++i)
                deck.Pick();
            DisplayDeck(grid, deck);
        }

        private void DisplayDeck(Grid grid, Deck deck)
        {
            stackPanel.Children.Clear();
            foreach(var card in deck.GetDeck())
            {
                Border border = new Border { BorderBrush = Brushes.Black, BorderThickness= new Thickness(1)};
                //TextBlock textBlock = new TextBlock { Text = card.ToString(), FontSize = 25, Width = 100, Height = 130};
                RichTextBox textBlock = new RichTextBox {FontSize=25, Width=100, Height=130 };
                textBlock.Document.Blocks.Add(new Paragraph(new Run("♣♥♦♠")));
                border.Child = textBlock;
                Separator separator = new Separator { Width = 10, Visibility = Visibility.Hidden};
                stackPanel.Children.Add(border);
                stackPanel.Children.Add(separator);
                

            }
        }
    }
}
