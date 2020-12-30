using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.IO.Ports;

namespace MySerialPortApp {
    public partial class mainForm : Form {
        private const int gSect = 40;     // MCU flash开始扇区号
        public mainForm() {
            InitializeComponent();
            CheckForIllegalCrossThreadCalls = false;
            serialPort.DataReceived += new SerialDataReceivedEventHandler(port_DataReceived);
            serialPort.Encoding = Encoding.GetEncoding("GB2312");
            // 搜索串口，将串口号下拉框初始化
            initSerialComboBox(serialPort, comboBoxCom);
            System.Windows.Forms.Control.CheckForIllegalCrossThreadCalls = false;
        }

        /// <summary>
        /// 在窗体初始化函数中，定义了一个事务，将接收函数增加到该事物队列中
        /// 数据接收函数
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void port_DataReceived(object sender, SerialDataReceivedEventArgs e) {
            try {
                tbRecv.Text += serialPort.ReadExisting();
                //tbRecv.AppendText(serialPort.ReadExisting());
            }
            catch {
                MessageBox.Show("error");
            }
        }

        private void sci_sendData(SerialPort s, string str) {
            s.Write(str);
            System.Threading.Thread.Sleep(90);
        }

        /// <summary>
        /// 搜索串口函数，将可用的串口添加到 box
        /// 思路就是遍历20以内的串口号并且尝试打开
        /// 如果成功就将该串口号添加到box
        /// </summary>
        /// <param name="port"></param>
        /// <param name="box"></param>
        private void initSerialComboBox(SerialPort port, ComboBox box) {
            //string[] portNumber = new string[20];
            string comNumber;
            box.Items.Clear();
            for (int i = 0; i < 20; i++) {
                try {
                    comNumber = "COM" + i.ToString();
                    port.PortName = comNumber;
                    port.Open();        // 如果打开失败，后面的代码不会执行，进行下一轮循环
                    //portNumber[i] = temp;
                    box.Text = comNumber;
                    port.Close();
                }
                catch { }
            }
        }


        /// <summary>
        /// 打开和关闭串口
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnOpenCom_Click(object sender, EventArgs e) {
            if (btnOpenCom.Text.Equals("打开串口")) {
                try {
                    serialPort.PortName = comboBoxCom.Text;     // 设置端口号
                    serialPort.BaudRate = Convert.ToInt32(comboBoxBaud.Text);   // 设置波特率
                    serialPort.Open();  // 打开串口
                    btnOpenCom.Text = "关闭串口";
                }
                catch {
                    MessageBox.Show("串口打开错误");
                }
            }
            else {
                try {
                    serialPort.Close();     // 关闭串口
                    btnOpenCom.Text = "打开串口";
                }
                catch { }
            }
        }

        private void btnUpPic_Click(object sender, EventArgs e) {
            OpenFileDialog openfile = new OpenFileDialog();
            openfile.Title = " 请选择客户端longin的图片";
            openfile.Filter = "Login图片 (*.jpg;*.bmp;*png)|*.jpeg;*.jpg;*.bmp;*.png|AllFiles(*.*)|*.*";
            if (DialogResult.OK == openfile.ShowDialog()) {
                try {
                    Bitmap bmp = new Bitmap(openfile.FileName);
                    picBox1.Image = bmp;
                    picBox1.SizeMode = PictureBoxSizeMode.Zoom;

                    //字面是对当前图片进行了二进制转换
                    MemoryStream ms = new MemoryStream();
                    bmp.Save(ms, System.Drawing.Imaging.ImageFormat.Png);
                    byte[] arr = new byte[ms.Length];
                    ms.Position = 0;
                    ms.Read(arr, 0, (int)ms.Length);
                    ms.Close();
                    //这里将arr强转Base64
                    string base64Str = Convert.ToBase64String(arr);
                    tbRecv.Clear();
                    tbRecv.Text = base64Str;

                    int len = base64Str.Length;
                    int i = 0;

                    if (len > 1023) {
                        while (true) {
                            if (len > 1023) {
                                sci_sendData(serialPort, ("1" + (char)(gSect + i) + base64Str.Substring(i * 1023, 1023)).ToString());
                                len -= 1023;
                            }
                            else {
                                sci_sendData(serialPort, ("1" + (char)(gSect + i) + base64Str.Substring(i * 1023, len)).ToString());
                              
                                break;
                            }
                            i++;
                        }
                    }
                    else {
                        sci_sendData(serialPort, ("1" + (char)(gSect + i) + base64Str + "\0"));
                       
                    }

                    sci_sendData(serialPort, ("1" + (char)(63) + (i + 1).ToString()).ToString());
                }
                catch (Exception err){
                    MessageBox.Show(err.ToString());
                }
            }

        }

        private void btnDownPic_Click(object sender, EventArgs e) {
            if (btnDownPic.Text.Equals("加载记录")) {
                try {
                    tbRecv.Text = "";
                    sci_sendData(serialPort, ("2" + (char)(63)).ToString());
                    

                    btnDownPic.Text = "获取图片";
                }
                catch { }
            }
            else if(btnDownPic.Text.Equals("获取图片")) {
                try {
                    int len = Convert.ToInt32(tbRecv.Text);
                    tbRecv.Clear();
                    for(int i = 0; i < len; i++) {
                        sci_sendData(serialPort, ("2" + (char)(gSect + i)).ToString());
                   
                    }      

                    btnDownPic.Text = "显示图片";
                }
                catch (Exception ex) {
                    btnDownPic.Text = "显示图片";
                    Console.WriteLine(ex);
                }
            }
            else {
                //MessageBox.Show(tbRecv.Text);

                var base64 = tbRecv.Text;

                //MessageBox.Show(base64.Length.ToString());
                //将base64强转图片
                base64 = base64.Replace("data:image/png;base64,", "").Replace("data:image/jgp;base64,", "").Replace("data:image/jpg;base64,", "").Replace("data:image/jpeg;base64,", "");//将base64头部信息替换
                byte[] bytes = Convert.FromBase64String(base64);
                MemoryStream ms = new MemoryStream(bytes);
                Image mImage = Image.FromStream(ms);
                Bitmap bmpt = new Bitmap(ms);

                picBox2.Image = bmpt;
                picBox2.SizeMode = PictureBoxSizeMode.Zoom;

                btnDownPic.Text = "加载记录";
            }

        }

        private void button1_Click(object sender, EventArgs e) {
            tbRecv.Clear();
            serialPort.Write("2?");
        }

    }
}
