using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace csgametest2
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {
        private class Matrix
        {
            public int Row { get; set; }
            public int Column { get; set; }
            
            public Matrix(int row, int column)
            {
                Row = row;
                Column = column;
            }

            public Matrix() { }
        }

        private const int BoardWidth = 10 & ~1;
        private object mutex = new object();
        private bool IsPlaying { get; set; }
        private readonly List<Matrix> snake = new List<Matrix>();
        private readonly Matrix apple = new Matrix();
        private readonly Rectangle[,] rectangles = new Rectangle[BoardWidth, BoardWidth];
        private Random random = new Random();
        private readonly TextBlock textBlock = new TextBlock() { Text = "Head", HorizontalAlignment = HorizontalAlignment.Center, VerticalAlignment = VerticalAlignment.Center };
        private TextBlock block = new TextBlock() { HorizontalAlignment = HorizontalAlignment.Center, VerticalAlignment = VerticalAlignment.Center, Text = "게임 오버! 다시 시작하려면 스페이스바를 누르세요.", FontSize = 20};
        private readonly bool[,] map = new bool[BoardWidth, BoardWidth];
        private readonly Timer timer = new Timer() { Interval = 130};
        private Key Direction { get; set; }

        public MainWindow()
        {
            InitializeComponent();
        }

        private void DisplayGameOver()
        {
            //mutex 쓰기
            Dispatcher.Invoke(() =>
            {
                foreach (var matrix in snake)
                    rectangles[matrix.Row, matrix.Column].Fill = Brushes.Transparent;
                snake.Clear();
                rectangles[apple.Row, apple.Column].Fill = Brushes.Transparent;
                Array.Clear(map, 0, map.Length);
                block.Visibility = Visibility.Visible;
                IsPlaying = false;
            });
        }

        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
            if (IsPlaying)
            {
                switch(e.Key)
                {
                    case Key.Left:
                    case Key.Up:
                    case Key.Right:
                    case Key.Down:
                        if (Direction == e.Key || Math.Abs(Direction - e.Key) == 2)
                            break;
                        Direction = e.Key;
                        Timer_Elapsed(null, null);
                        timer.Stop();
                        timer.Start();
                        break;
                }
            }
            else if(e.Key == Key.Space)
            {
                block.Visibility = Visibility.Collapsed;
                Initialize();
            }
        }
        private void Initialize()
        {
            snake.Add(new Matrix(random.Next(BoardWidth - 4) + 2, random.Next(BoardWidth - 4) + 2));
            rectangles[snake[0].Row, snake[0].Column].Fill = Brushes.Green;

            Grid.SetRow(textBlock, snake[0].Row);
            Grid.SetColumn(textBlock, snake[0].Column);


            while(true)
            {
                Direction = random.Next(4) + Key.Left;

                switch(Direction)
                {
                    case Key.Left:
                        if (snake[0].Column <= 3)
                            continue;
                        snake.Add(new Matrix(snake[0].Row, snake[0].Column + 1));
                        snake.Add(new Matrix(snake[0].Row, snake[0].Column + 2));
                        break;
                    case Key.Up:
                        if (snake[0].Row <= 3)
                            continue;
                        snake.Add(new Matrix(snake[0].Row + 1, snake[0].Column));
                        snake.Add(new Matrix(snake[0].Row + 2, snake[0].Column));
                        break;
                    case Key.Right:
                        if (snake[0].Column >= BoardWidth - 4)
                            continue;
                        snake.Add(new Matrix(snake[0].Row, snake[0].Column - 1));
                        snake.Add(new Matrix(snake[0].Row, snake[0].Column - 2));
                        break;
                    case Key.Down:
                        if (snake[0].Row >= BoardWidth - 4)
                            continue;
                        snake.Add(new Matrix(snake[0].Row - 1, snake[0].Column));
                        snake.Add(new Matrix(snake[0].Row - 2, snake[0].Column));
                        break;
                }
                break;
            }



            rectangles[snake[1].Row, snake[1].Column].Fill = Brushes.Green;
            rectangles[snake[2].Row, snake[2].Column].Fill = Brushes.Green;



            apple.Row = random.Next(BoardWidth);
            apple.Column = random.Next(BoardWidth);

            while (snake.Any(m =>
            {
                if (apple.Row == m.Row && apple.Column == m.Column)
                {
                    apple.Row = random.Next(BoardWidth);
                    apple.Column = random.Next(BoardWidth);
                    return true;
                }
                else
                    return false;
            })) ;

            rectangles[apple.Row, apple.Column].Fill = Brushes.IndianRed;
            IsPlaying = true;
        }
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {

            for(int i = 0; i < BoardWidth; ++i)
            {
                grid.RowDefinitions.Add(new RowDefinition());
                grid.ColumnDefinitions.Add(new ColumnDefinition());

            }

            for (int i = 0; i < BoardWidth; i++)
            {
                for (int j = 0; j < BoardWidth; j++)
                {
                    rectangles[i, j] = new Rectangle();

                    grid.Children.Add(rectangles[i, j]);
                    Grid.SetRow(rectangles[i, j], i);
                    Grid.SetColumn(rectangles[i, j], j);
                }
            }

            grid.Children.Add(textBlock);

            block.Visibility = Visibility.Collapsed;
            grid.Children.Add(block);
            Grid.SetRowSpan(block, BoardWidth);
            Grid.SetColumnSpan(block, BoardWidth);

            Initialize();

            timer.Elapsed += Timer_Elapsed;
            timer.Start();
        }

        private void Timer_Elapsed(object sender, ElapsedEventArgs e)
        {
            int previousRow = 0, previousColumn = 0;
            
            for(int i = 0; i < snake.Count; ++i)
            {
                if(i == 0)
                {
                    previousRow = snake[0].Row;
                    previousColumn = snake[0].Column;

                    switch (Direction)
                    {
                        case Key.Left:
                            if (snake[0].Column == 0 || (snake[0].Column - 1 == snake[1].Column && snake[0].Row == snake[1].Row) || map[snake[0].Row, snake[0].Column - 1])
                            {
                                DisplayGameOver();
                                return;
                            }
                            map[snake[0].Row, --snake[0].Column] = true;
                            break;
                        case Key.Up:
                            if (snake[0].Row == 0 || (snake[0].Row - 1 == snake[1].Row && snake[0].Column == snake[1].Column) || map[snake[0].Row - 1, snake[0].Column])
                            {
                                DisplayGameOver();
                                return;
                            }
                            map[--snake[0].Row, snake[0].Column] = true;
                            break;
                        case Key.Right:
                            if (snake[0].Column == BoardWidth - 1 || (snake[0].Column + 1 == snake[1].Column && snake[0].Row == snake[1].Row) || map[snake[0].Row, snake[0].Column + 1])
                            {
                                DisplayGameOver();
                                return;
                            }
                            if (map[snake[0].Row, snake[0].Column + 1])
                            {
                                DisplayGameOver();
                                return;
                            }
                            map[snake[0].Row, ++snake[0].Column] = true;
                            break;
                        case Key.Down:
                            if (snake[0].Row == BoardWidth - 1 || (snake[0].Row + 1 == snake[1].Row && snake[0].Column == snake[1].Column) || map[snake[0].Row + 1, snake[0].Column])
                            {
                                DisplayGameOver();
                                return;
                            }
                            map[++snake[0].Row, snake[0].Column] = true;
                            break;
                    }
                    Dispatcher.Invoke(() =>
                    {
                        rectangles[snake[0].Row, snake[0].Column].Fill = Brushes.Green;
                        Grid.SetRow(textBlock, snake[0].Row);
                        Grid.SetColumn(textBlock, snake[0].Column);
                    });
                }
                else
                {
                    int tempRow, tempColumn;

                    tempRow = snake[i].Row;
                    tempColumn = snake[i].Column;

                    snake[i].Row = previousRow;
                    snake[i].Column = previousColumn;

                    previousRow = tempRow;
                    previousColumn = tempColumn;

                }
            }
            map[previousRow, previousColumn] = false;
            Dispatcher.Invoke(() => rectangles[previousRow, previousColumn].Fill = Brushes.Transparent);

            if (snake[0].Row == apple.Row && snake[0].Column == apple.Column) 
            {
                snake.Add(new Matrix(previousRow, previousColumn));
                map[previousRow, previousColumn] = true;
                Dispatcher.Invoke(() => rectangles[previousRow, previousColumn].Fill = Brushes.Green);

                apple.Row = random.Next(BoardWidth);
                apple.Column = random.Next(BoardWidth);

                while (snake.Any(m =>
                {
                    if (apple.Row == m.Row && apple.Column == m.Column)
                    {
                        apple.Row = random.Next(BoardWidth);
                        apple.Column = random.Next(BoardWidth);
                        return true;
                    }
                    else
                        return false;
                })) ;
                Dispatcher.Invoke(() => rectangles[apple.Row, apple.Column].Fill = Brushes.IndianRed);
            }
        }
    }
}
