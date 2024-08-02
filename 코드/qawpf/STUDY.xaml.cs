using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
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
    public partial class STUDY : Page
    {
        private List<StudyItem> studyItems;
        private int currentIndex;

        public STUDY()
        {
            InitializeComponent();
            LoadData();
            DisplayCurrentItem();
            LoadImageFromServer();
        }

        private void LoadData()
        {
            // 서버에서 데이터를 받아오는 부분
            byte[] buffer = new byte[4096];
            int bytesRead = User_Login.stream.Read(buffer, 0, buffer.Length);
            string response = Encoding.UTF8.GetString(buffer, 0, bytesRead);

            var responseData = JsonConvert.DeserializeObject<Dictionary<string, List<StudyItem>>>(response);
            studyItems = responseData["STUDY_GO"];
            currentIndex = 0;
            //LoadImageFromServer();
        }

        private void DisplayCurrentItem()
        {
            if (currentIndex >= studyItems.Count)
            {
                currentIndex = 0; // 데이터가 끝나면 처음으로 돌아감
            }

            var currentItem = studyItems[currentIndex];
            question.Text = currentItem.ST_CITY_K;
            answer.Text = currentItem.ST_CON_K;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            currentIndex++;
            DisplayCurrentItem();
        }

        private void LoadImageFromServer()
        {
            try
            {
                MessageBox.Show("이미지 로드 성공");

                // 이미지 데이터 크기 수신
                byte[] sizeBuffer = new byte[sizeof(int)];
                User_Login.stream.Read(sizeBuffer, 0, sizeBuffer.Length);
                int imageSize = BitConverter.ToInt32(sizeBuffer.Reverse().ToArray(), 0); // 네트워크 바이트 순서로부터 변환
                Debug.WriteLine("*******************");
                Debug.WriteLine(imageSize);

                byte[] buffer = new byte[4096];
                using (MemoryStream ms = new MemoryStream())
                {
                    int bytesRead;
                    int totalBytesRead = 0;
                    while (totalBytesRead < imageSize && (bytesRead = User_Login.stream.Read(buffer, 0, buffer.Length)) > 0)
                    {
                        ms.Write(buffer, 0, bytesRead);
                        totalBytesRead += bytesRead;
                        //Debug.WriteLine("&&&&&&&&&&&&&&&&&&&&");
                        //Debug.WriteLine(bytesRead);
                    }
                    ms.Seek(0, SeekOrigin.Begin);
                    BitmapImage bitmap = new BitmapImage();
                    bitmap.BeginInit();
                    bitmap.StreamSource = ms;
                    bitmap.CacheOption = BitmapCacheOption.OnLoad;
                    bitmap.EndInit();
                    displayImage.Source = bitmap;
                    Debug.WriteLine("이미지 로드 완료");

                }
            }
            catch (IOException ex)
            {
                MessageBox.Show($"이미지 로드 실패 - 네트워크 오류: {ex.Message}");
                Debug.WriteLine($"네트워크 오류: {ex.Message}");
            }
            catch (Exception ex)
            {
                MessageBox.Show($"이미지 로드 실패: {ex.Message}");
                Debug.WriteLine($"예외 발생: {ex.Message}");
            }
        }

        private void back_Click(object sender, RoutedEventArgs e)
        {
            NavigationService.Navigate(new Uri("/Menu.xaml", UriKind.Relative));
        }
    }

    public class StudyItem
    {
        public string ST_CITY_K { get; set; }
        public string ST_CON_K { get; set; }
    }
}
