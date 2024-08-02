using Network;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
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

namespace qawpf
{
    /// <summary>
    /// Menu.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class Menu : Page
    {
        private bool logFlag = false;


        //string serverIp = "10.10.21.115";  // 서버 ip설정
        //int serverPort = 12345;            // 포트번호
        //public static NetworkStream stream;
        public Menu()
        {
            InitializeComponent();
            //stream = User.Conncection(serverIp, serverPort);
        }

        private void Login_btn_Click(object sender, RoutedEventArgs e)
        {
            //if (MainWindow.login_check)
            //{
                MainWindow.login_check = true;
                NavigationService.Navigate(
                    //new Uri("/Direct_Chat.xaml", UriKind.Relative) 
                    new Uri("/User_Login.xaml", UriKind.Relative)   // Menu.xaml ( 페이지 ) 로 이동
                    );
            //}
            //else
            //{
            //    MessageBox.Show("로그인을 하고 이용해주세요");
            //}
        }
        private void QA_btn_Click(object sender, RoutedEventArgs e)
        {
            NavigationService.Navigate(new Uri("/QNA.xaml", UriKind.Relative));
        }

        private Window Direct_Chat;
        private async void DirectChat_btn_Click(object sender, RoutedEventArgs e)
        {
            if(MainWindow.login_check == true)
            {
                string message = "CHAT";
                byte[] data = Encoding.UTF8.GetBytes(message);
                await User_Login.stream.WriteAsync(data, 0, data.Length);
                if (logFlag == false)
                {
                    Direct_Chat = new Direct_Chat()
                    {
                        Height = 450,
                        Width = 300
                    };
                    Direct_Chat.Show();
                    logFlag = true;
                }
                else
                {
                    Direct_Chat.Close();
                    Direct_Chat = null;
                    logFlag = false;
                }
            }
            else
            {
                MessageBox.Show("로그인후 진행해주세요");
            }
            
        }

        private async void STD_btn_Click(object sender, RoutedEventArgs e)
        {
            
            if (MainWindow.login_check == true)
            {
                string message = "STUDY";
                byte[] data = Encoding.UTF8.GetBytes(message);
                await User_Login.stream.WriteAsync(data, 0, data.Length);
                NavigationService.Navigate(new Uri("/STUDY.xaml", UriKind.Relative));
            }
            else
            {
                MessageBox.Show("로그인후 진행해주세요");
            }
        }

        private async void TEST_btn_Click(object sender, RoutedEventArgs e)
        {
            if (MainWindow.login_check == true)
            {
                string message = "TEST";
                byte[] data = Encoding.UTF8.GetBytes(message);
                await User_Login.stream.WriteAsync(data, 0, data.Length);
                NavigationService.Navigate(new Uri("/TEST.xaml", UriKind.Relative));
            }
            else
            {
                MessageBox.Show("로그인후 진행해주세요");
            }
        }

        private async void join_btn_Click(object sender, RoutedEventArgs e)
        {
            //JObject temp;                                                             
            //temp = User.signin();
            //User.DataWrite(User_Login.stream, temp);
            NavigationService.Navigate(new Uri("/JOIN.xaml", UriKind.Relative));
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            NavigationService.Navigate(new Uri("/AAA.xaml", UriKind.Relative));
        }
    }
}
