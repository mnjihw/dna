using Google;
using Google.Apis.Services;
using Google.Apis.YouTube.v3;
using Google.Apis.YouTube.v3.Data;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Printing;
using System.Runtime.CompilerServices;
using System.Security.Policy;
using System.Text;
using System.Text.RegularExpressions;
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

namespace 유튜브_댓글
{
    public partial class MainWindow : Window
    {
        private string[] Keywords { get; set; }
        private YouTubeService Youtube { get; set; } = new YouTubeService(new BaseClientService.Initializer { ApiKey = "AIzaSyC25aLdUwyNGCT7A1o-mURUnhKfux0H3ns", ApplicationName = "test" });
        public MainWindow()
        {
            InitializeComponent();
        }

        private async IAsyncEnumerable<Comment> GetComments(string videoId)
        {
            CommentThreadListResponse commentThreadListResponse;
            var commentThreads = Youtube.CommentThreads.List("id, replies, snippet");
            commentThreads.VideoId = videoId;
            commentThreads.MaxResults = 100;
            
            do
            {
                try
                {
                    commentThreadListResponse = await commentThreads.ExecuteAsync();
                }
                catch (GoogleApiException)
                {
                    MessageBox.Show("유튜브 링크 잘못됨");
                    yield break;
                }

                foreach (var item in commentThreadListResponse.Items)
                {
                    var comment = new Comment
                    {
                        Author = item.Snippet.TopLevelComment.Snippet.AuthorDisplayName,
                        AuthorId = item.Snippet.TopLevelComment.Snippet.AuthorChannelId?.Value,
                        Content = item.Snippet.TopLevelComment.Snippet.TextDisplay,
                        Id = item.Snippet.TopLevelComment.Id
                    };

                    if (item.Replies != null)
                    {
                        comment.SubComments = new List<Comment>();

                        foreach (var subComment in item.Replies.Comments)
                        {
                            comment.SubComments.Add(new Comment
                            {
                                Author = subComment.Snippet.AuthorDisplayName,
                                AuthorId = subComment.Snippet.AuthorChannelId.Value,
                                Content = subComment.Snippet.TextDisplay,
                                Id = subComment.Id
                            });
                        }
                    }

                    yield return comment;
                }

                commentThreads.PageToken = commentThreadListResponse.NextPageToken;
            }
            while (commentThreadListResponse.NextPageToken != null);
        }

        private async Task GetAndDisplayComments(string videoId)
        { 
            int commentCount = 0;
            var comments = new List<Comment>();
            commentCountTextBlock.Text = "";
            progressBar.Visibility = Visibility.Visible;
            //일단 대댓글로 주인장이 단 댓글은 인식이 안됨 이유 모르겠다
            //trimmed로 publish하면 System.Runtime.CompilerServices.AsyncIteratorMethodBuilder 못찾았다고 예외남 n 


            treeView.Items.Clear();

            await foreach (var comment in GetComments(videoId))
            {
                comments.Add(comment);
                commentCount += (comment.SubComments?.Count ?? 0) + 1;
                commentCountTextBlock.Text = $"댓글 {commentCount}개째 읽음";

                var stackPanel = new StackPanel { Orientation = Orientation.Horizontal };
                var nicknameTextBlock = new TextBlock {Text = $"[{comment.Author}]" };
                var contentTextBlock = new TextBlock { Text = comment.Content };

                contentTextBlock.Tag = comment.Content; //이거 comment로 바꾸기
                stackPanel.Children.Add(nicknameTextBlock);
                stackPanel.Children.Add(contentTextBlock);

                var item = new TreeViewItem { Header = stackPanel };

                if (comment.SubComments != null)
                {
                    foreach (var subComment in comment.SubComments)
                    {
                        var stackPanel2 = new StackPanel { Orientation = Orientation.Horizontal };
                        var nicknameTextBlock2 = new TextBlock { Text = $"[{subComment.Author}]" };
                        var contentTextBlock2 = new TextBlock { Text = subComment.Content };

                        contentTextBlock2.Tag = subComment.Content;
                        stackPanel2.Children.Add(nicknameTextBlock2);
                        stackPanel2.Children.Add(contentTextBlock2);
                        item.Items.Add(new TreeViewItem { Header = stackPanel2});
                    }
                }
                treeView.Items.Add(item);
            }

            progressBar.Visibility = Visibility.Hidden;
            commentCountTextBlock.Text = $"댓글: {commentCount}개";
        }

        private async void UrlTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if(e.Key == Key.Enter)
            {
                var text = (sender as TextBox).Text;
                var match = Regex.Match(text, @"watch\?v=([^&]+)");

                if (!match.Success)
                {
                    MessageBox.Show("유튜브 링크 잘못됨");
                    return;
                }

                var videoId = match.Groups[1].Value;
                await GetAndDisplayComments(videoId);
            }
        }


   
        private async void KeywordTextBox_KeyDown(object sender, KeyEventArgs e)
        { 
            if (e.Key == Key.Enter)
            {
                if(treeView.Items.Count == 0)
                {
                    var match = Regex.Match(urlTextBox.Text, @"watch\?v=([^&]+)");

                    if (!match.Success)
                    {
                        MessageBox.Show("유튜브 링크 잘못됨");
                        return;
                    }

                    var videoId = match.Groups[1].Value;
                    await GetAndDisplayComments(videoId);
                }

                Keywords = (sender as TextBox).Text.Split(Environment.NewLine).Where(s => !string.IsNullOrWhiteSpace(s)).ToArray();

                if(Keywords.Length == 0 || Keywords.Length == 1 && string.IsNullOrWhiteSpace(Keywords[0]))
                {
                    DehighlightItems(treeView.Items);
                    foreach (TreeViewItem item in treeView.Items)
                        if (item.HasItems)
                            item.Items.Filter = null;
                    treeView.Items.Filter = null;
                    return;
                }
                foreach(TreeViewItem item in treeView.Items)
                {
                    ((item.Header as StackPanel).Children[^1] as TextBlock).Inlines.Clear();
                    ((item.Header as StackPanel).Children[^1] as TextBlock).Inlines.Add(new Run(((item.Header as StackPanel).Children[^1] as TextBlock).Tag as string));
                }

                treeView.Items.Filter = FilterItemsByKeywords;
            }


            void HighlightItem(TreeViewItem item, string[] splitTexts)
            {
                ((item.Header as StackPanel).Children[^1] as TextBlock).Inlines.Clear();

                for (int i = 0; i < splitTexts.Length - 1; ++i)
                {
                    if (Keywords.Contains(splitTexts[i]))
                        ((item.Header as StackPanel).Children[^1] as TextBlock).Inlines.Add(new Run(Regex.Unescape(splitTexts[i])) { Background = Brushes.Yellow });
                    else
                        ((item.Header as StackPanel).Children[^1] as TextBlock).Inlines.Add(new Run(splitTexts[i]));
                }
                ((item.Header as StackPanel).Children[^1] as TextBlock).Inlines.Add(new Run(splitTexts[^1]));

            }

            static void DehighlightItems(ItemCollection items)
            {
                foreach(TreeViewItem item in items)
                {
                    if(item.HasItems)
                        DehighlightItems(item.Items);
                    ((item.Header as StackPanel).Children[^1] as TextBlock).Inlines.Clear();
                    ((item.Header as StackPanel).Children[^1] as TextBlock).Inlines.Add(new Run(((item.Header as StackPanel).Children[^1] as TextBlock).Tag as string));
                }
            } 

            bool FilterItemsByKeywords(object obj)
            {
                var item = obj as TreeViewItem;

                if (item.HasItems)
                    item.Items.Filter = FilterItemsByKeywords;
                
                var pattern = $"({Regex.Escape(Keywords.Aggregate((a, b) => $"{a}|{b}")).Replace("\\|", "|")})";
                var splitTexts = Regex.Split(((item.Header as StackPanel).Children[^1] as TextBlock).Tag as string, pattern, RegexOptions.IgnoreCase);

                if (splitTexts.Length != 1 && !andRadioButton.IsChecked.Value || Keywords.All(k => (((item.Header as StackPanel).Children[^1] as TextBlock).Tag as string).Contains(k)))
                {
                    HighlightItem(item, splitTexts);
                    return true;
                }
                return false;
            }

        }
        
        private bool FilterItemsByLanguage(object obj)
        {
            var item = obj as TreeViewItem;

            if (item.HasItems)
                item.Items.Filter = FilterItemsByLanguage;

            return Regex.IsMatch(((item.Header as StackPanel).Children[^1] as TextBlock).Tag as string, "[ㄱ-ㅎ가-힣]");
        }

        private void ExpandButton_Click(object sender, RoutedEventArgs e)
        {
            foreach (TreeViewItem item in treeView.Items)
                item.IsExpanded = true;
        }

        private void CollapseButton_Click(object sender, RoutedEventArgs e)
        {
            foreach (TreeViewItem item in treeView.Items)
                item.IsExpanded = false;
        }

        private void PickButton_Click(object sender, RoutedEventArgs e)
        {
            if(!int.TryParse(headcountTextBox.Text, out var headcount))
            {
                MessageBox.Show("숫자만 입력 가능함");
                return;
            }

            


        }

        private void CheckBox_Checked(object sender, RoutedEventArgs e)
        {
            treeView.Items.Filter = FilterItemsByLanguage;
        }

        private void CheckBox_Unchecked(object sender, RoutedEventArgs e)
        {
            foreach (TreeViewItem item in treeView.Items)
                if (item.HasItems)
                    item.Items.Filter = null;
            treeView.Items.Filter = null;
        }
    }
}
