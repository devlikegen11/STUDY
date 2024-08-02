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

namespace qawpfCS
{
    /// <summary>
    /// Q_A.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class Q_A : Page
    {
        public Q_A()
        {
            InitializeComponent();
        }

        private void Q1_Click(object sender, RoutedEventArgs e)
        {
            answer.Text = "고객이 오류나 버그에 대한 문제나 질문을 제기했는지 구체적으로 작성하여\n" +
                          "이내용들을 담당자 이메일로 전송해주세요.";

        }

        private void Q2_Click(object sender, RoutedEventArgs e)
        {
            answer.Text = "문제가 긴급한 경우 또는 신속한 조치가 필요한 경우\n" +
                          "그에 맞는 우선순위와 조치 방안을 검토해주세요.";
        }

        private void Q3_Click(object sender, RoutedEventArgs e)
        {
            answer.Text = "고객이 원하는 해결 방법이나 상담을 통해 이루고자 하는 목표를\n" +
                          "명확하게 이해하고 있어야 합니다.";
        }

        private void Q4_Click(object sender, RoutedEventArgs e)
        {
            answer.Text = "고객이 이전에 받은 상담 내용이 있다면 그 내용을 확인할 수 있도록 해주세요.\n" +
                          "이전 상담 이력을 통해 문제 해결에 더욱 효율적으로 접근할 수 있습니다.";
        }

        private void Q5_Click(object sender, RoutedEventArgs e)
        {
            answer.Text = "상담을 진행하실때 욕설이나 폭언 등 비매너 행동을 그 내용을 저장해두고\n" +
                          "담당자 이메일로 보내주시기 바랍니다.\n" +
                          "만약 고객이 1:1상담요청을 신청한다면 수락하지 마십시오.";
        }

        private void back_Click(object sender, RoutedEventArgs e)
        {
            NavigationService.Navigate(new Uri("/Menu.xaml", UriKind.Relative));
        }
    }
}
