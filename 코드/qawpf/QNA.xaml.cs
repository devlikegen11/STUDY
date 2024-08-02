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

namespace qawpf
{
    /// <summary>
    /// QNA.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class QNA : Page
    {
        public QNA()
        {
            InitializeComponent();
        }

        private void Q1_Click(object sender, RoutedEventArgs e)
        {
            answer.Text = "추후 추가할 예정이며 확실히 정해 졌을시 공지 드리겠습니다.\n" +
                          "저희 프로그램을 이용해주셔서 감사합니다.";

        }

        private void Q2_Click(object sender, RoutedEventArgs e)
        {
            answer.Text = "저희는 검증된 API를 통해 공식적인 정보를 가져오기에 달라졌을 경우에도\n" +
                          "빠르게 업데이트 되고 있습니다.\n" +
                          "저희 프로그램을 이용해주셔서 감사합니다.";
        }

        private void Q3_Click(object sender, RoutedEventArgs e)
        {
            answer.Text = "유저분께서 직접 아이디를 찾는 방법은 아직 추후 추가할 예정이지만\n" +
                          "잃어버리셔서 곤란하신 경우는 1:1 상담을 부탁 드립니다.\n" +
                          "저희 프로그램을 이용해주셔서 감사합니다.";
        }

        private void Q4_Click(object sender, RoutedEventArgs e)
        {
            answer.Text = "1:1상담운영시간은 09:00 ~18:00입니다.\n" +
                          "저희 프로그램을 이용해주셔서 감사합니다.";
        }

        private void Q5_Click(object sender, RoutedEventArgs e)
        {
            answer.Text = "1:1상담을 통하여 제보를 해주시면 빠르게 답변 드리겠습니다.\n" +
                          "\r\n또는 오류나 버그를 발견 하신다면 LMS3LDJ@lmail.com으로 스크린샷이나\r\n" +
                          "영상을 촬영해서 보내주시면 더욱 빠르게 원인을 파악하여 해결해드리겠습니다." +
                          " \r\n저희 프로그램을 이용해주셔서 감사합니다.";
        }

        private void Q6_Click(object sender, RoutedEventArgs e)
        {
            answer.Text = "사용자는 정답을 맞히면 점수를 얻고, 오답 시 올바른 답을 학습할 수 있습니다.\n" +
                          "어플리케이션은 사용자의 진행 상황을 추적하여 개인적인 학습 경로를 제시합니다.\n" +
                          "게임적인 요소가 포함되어 있어 사용자가 재미를 느끼면서도 지식을 쌓을 수 있습니다.\n" +
                          "이 어플리케이션들은 교육적인 요소와 함께 사용자가 자신의 지식을 향상시키고,\n 재미있게 학습할 수 있는 기회를 제공합니다.\n" +
                          "각 문제에는 국가에 관한 추가적인 배경 정보가 포함되어 사용자가 국가에 대해 더 많이 알 수 있도록 돕습니다";
        }

        private void back_Click(object sender, RoutedEventArgs e)
        {
            NavigationService.Navigate(new Uri("/Menu.xaml", UriKind.Relative));
        }
    }
}