using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
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

namespace qawpf
{
    /// <summary>
    /// TEST.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class TEST : Page
    {
        private List<TestItem> TestItems;
        private int currentIndex;
        public static int Score;
        public TEST()
        {
            InitializeComponent();
            LoadTest();
            DisplayCurrentItem();
        }

        private void LoadTest()
        {
            byte[] buffer = new byte[4096];
            int bytesRead = User_Login.stream.Read(buffer, 0, buffer.Length);
            string response = Encoding.UTF8.GetString(buffer, 0, bytesRead);

            var responseData = JsonConvert.DeserializeObject<Dictionary<string, List<TestItem>>>(response);
            TestItems = responseData["TEST_GO"];
            currentIndex = 0;
        }

        private void DisplayCurrentItem()
        {
            if (currentIndex + 5 >= TestItems.Count)
            {
                NavigationService.Navigate(new Uri("/score.xaml", UriKind.Relative));
                return;
            }

            var currentItem = TestItems[currentIndex];
            te_question.Text = currentItem.ST_CITY_K;
            te_answer.Text = ""; // 답변 텍스트 박스를 초기화합니다.
        }

        private void CheckAnswer()
        {
            if (currentIndex < TestItems.Count)
            {
                var currentItem = TestItems[currentIndex];
                string aaa = te_question.Text;
                string user_answer = te_answer.Text;
                string corr = currentItem.ST_CON_K;

                if (currentItem.ST_CON_K.Contains(user_answer) && user_answer != "")
                {
                    Score += 20;
                }
                //Score += 11; // 이 부분이 의도된 것인지 확인 필요/
                Console.WriteLine("***********************");
                Console.WriteLine(aaa);
                Console.WriteLine(corr);
                Console.WriteLine(user_answer);
                Console.WriteLine(Score);
                Console.WriteLine("***********************");
            }
        }

        private void next_btn_Click(object sender, RoutedEventArgs e)
        {
            CheckAnswer(); // 현재 문제의 정답을 체크하고 점수를 계산합니다.
            currentIndex++; // 다음 문제로 이동합니다.
            DisplayCurrentItem(); // 다음 문제를 표시합니다.
        }

        private void back_Click(object sender, RoutedEventArgs e)
        {
            NavigationService.Navigate(new Uri("/Menu.xaml", UriKind.Relative));
        }
    }

    public class TestItem
    {
        public string ST_CITY_K { get; set; }
        public string ST_CON_K { get; set; }
    }
}
