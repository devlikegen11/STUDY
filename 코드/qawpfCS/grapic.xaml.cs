using LiveCharts;
using LiveCharts.Defaults;
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
using System.Windows.Shapes;
using Newtonsoft.Json.Linq;
using System.Windows.Navigation;

namespace qawpfCS
{
    public partial class grapic : Window
    {
        public ChartValues<ObservableValue> Values { get; set; }
        public grapic()
        {
            InitializeComponent();
            Values = new ChartValues<ObservableValue>();
            foreach(var score in DBCHECK.score_list)
            {
                Values.Add(new ObservableValue(score));
            }

            DataContext = this;
        }

    }
}
