using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
using Newtonsoft.Json.Linq;
using qawpfCS.Model;

namespace qawpfCS
{
    public partial class DBCHECK : Window
    {
        private bool GRFlag = false;
        public ObservableCollection<ChatRecord> ScoreRecords { get; set; }
        static public List<int> score_list = new List<int>();
        public DBCHECK()
        {
            InitializeComponent();
            ScoreRecords = new ObservableCollection<ChatRecord>();
            user_search.ItemsSource = ScoreRecords;
            score_list = new List<int>();
        }

        private void userch_start_Click(object sender, RoutedEventArgs e)
        {
            string message = "SCORE_CHECK";
            byte[] data = Encoding.UTF8.GetBytes(message);
            CS_Login.stream.Write(data, 0, data.Length);

            string userchoose = usercheck.Text;
            data = Encoding.UTF8.GetBytes(userchoose);
            CS_Login.stream.Write(data, 0, data.Length);    // 검색하고자 하는 유저id 전송

            // 서버로부터 응답 받기
            byte[] buffer = new byte[4096];
            int bytesRead = CS_Login.stream.Read(buffer, 0, buffer.Length);
            string response = Encoding.UTF8.GetString(buffer, 0, bytesRead);

            Console.WriteLine("*************************************************************");
            Console.WriteLine(response);

            // JSON 파싱
            JObject jsonResponse = JObject.Parse(response);
            DisplayUserInfo(jsonResponse["user_info"]);
            DisplayScoreHistory(jsonResponse["SCORE"]["SCORE_GO"]);
        }

        private void DisplayUserInfo(JToken userInfo)
        {
            if (userInfo != null)
            {
                userdb.Items.Clear();
                userdb.Items.Add(new
                {
                    USER_NO = userInfo["USER_NO"].ToString(),
                    USER_ID = userInfo["USER_ID"].ToString(),
                    USER_PASSWORD = userInfo["USER_PASSWORD"].ToString(),
                    USER_NAME = userInfo["USER_NAME"].ToString(),
                    USER_PHONE = userInfo["USER_PHONE"].ToString(),
                    USER_STATE = userInfo["USER_STATE"].ToString()
                });
            }
        }
        
        private void DisplayScoreHistory(JToken chatHistory)
        {
            string set_score;
            int score_num;
            if (chatHistory != null)
            {
                ScoreRecords.Clear();
                foreach (var chatRecord in chatHistory)
                {
                    ScoreRecords.Add(new ChatRecord
                    {
                        CHAT_RECORD = chatRecord["SCORE_RECORD"].ToString(),
                        USER_ID = chatRecord["USER_ID"].ToString(),
                    });
                    set_score = chatRecord["SCORE_RECORD"].ToString();
                    score_num = int.Parse(set_score);
                    score_list.Add(score_num);
                }
            }
        }
        private Window grapic;
        private void show_Click(object sender, RoutedEventArgs e)
        {
            if (GRFlag == false)
            {
                grapic = new grapic()
                {
                    Height = 450,
                    Width = 650
                };
                grapic.Show();
                GRFlag = true;
            }
            else
            {
                grapic.Close();
                grapic = null;
                GRFlag = false;
            }
        }
    }
}