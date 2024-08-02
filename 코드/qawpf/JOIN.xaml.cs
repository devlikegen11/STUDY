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
using Network;   // Network 라이브러리 
using Newtonsoft.Json.Linq;
using Newtonsoft.Json;
using System.Security.Policy;
using System.Text.RegularExpressions;

namespace qawpf
{
    /// <summary>
    /// JOIN.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class JOIN : Page
    {
        string serverIp = "10.10.21.115";  // 서버 ip설정
        int serverPort = 12345;            // 포트번호
        public static NetworkStream stream;
        public JOIN()
        {
            InitializeComponent();
            stream = User.Conncection(serverIp, serverPort);
            JObject TRegit;
            TRegit = User.signin();// 임시 JSON 인스턴스 생성
            JOIN.DataWrite(stream, TRegit);
        }

        public static void DataWrite(NetworkStream stream, JObject registerMessage)
        {
            byte[] bytes = Encoding.UTF8.GetBytes(registerMessage.ToString());
            stream.Write(bytes, 0, bytes.Length);
        }

        public static string DataRecive(NetworkStream stream)
        {
            byte[] array = new byte[256];
            int count = stream.Read(array, 0, array.Length);

            return Encoding.UTF8.GetString(array, 0, count);
        }

        public static JObject RegitObject(string id, string password, string name, string phone, string state)
        {

            JObject jObject = new JObject();

            jObject["id"] = id;
            jObject["pw"] = password;
            jObject["name"] = name;
            jObject["phone"] = int.Parse(phone);
            jObject["state"] = state;

            return jObject;
        }
        private void regit_btn_Click(object sender, RoutedEventArgs e)
        {

            JObject Regit;
            Regit = JOIN.RegitObject(RID_txtbox.Text, RPassword_txtbox.Text,
                 name_txtbox.Text, phone_txtbox.Text, state_txtbox.Text);            // LoginObject, Jobject 보낼 오브젝트 생성 후 return
            JOIN.DataWrite(stream, Regit);                                             // Jobject 보냄           

            MessageBox.Show("회원가입이 완료되었습니다.");
            stream.Close();

            NavigationService.Navigate(
                    new Uri("/Menu.xaml", UriKind.Relative)                           // Menu.xaml ( 페이지 ) 로 이동
                    );

        }

        private void RID_txtbox_TextChanged(object sender, TextChangedEventArgs e)
        {

        }

        private void RPassword_txtbox_TextChanged(object sender, TextChangedEventArgs e)
        {

        }

        private void name_txtbox_TextChanged(object sender, TextChangedEventArgs e)
        {

        }

        private void phone_txtbox_TextChanged(object sender, TextChangedEventArgs e)
        {

        }

        private void state_txtbox_TextChanged(object sender, TextChangedEventArgs e)
        {

        }

        private void back_Click(object sender, RoutedEventArgs e)
        {
            stream.Close();

            NavigationService.Navigate(
                    new Uri("/Menu.xaml", UriKind.Relative)                           // Menu.xaml ( 페이지 ) 로 이동
                    );
        }
    }
}