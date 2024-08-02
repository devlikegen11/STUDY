using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
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
using Network;

namespace qawpf
{
    /// <summary>
    /// score.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class score : Page
    {
        NetworkStream stream;
        public score()
        {
            InitializeComponent();
            score_bar.Text = TEST.Score.ToString();
        }

        private void enter_Click(object sender, RoutedEventArgs e)
        {
            stream = User_Login.stream;
            string final_score = score_bar.Text;
            User.Write(stream, final_score);    // 성적 서버에 보내기
            NavigationService.Navigate(new Uri("/Menu.xaml", UriKind.Relative));
        }
    }
}
