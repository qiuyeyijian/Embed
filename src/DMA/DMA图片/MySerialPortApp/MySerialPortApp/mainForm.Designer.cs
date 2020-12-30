namespace MySerialPortApp
{
    partial class mainForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.serialPort = new System.IO.Ports.SerialPort(this.components);
            this.picBox1 = new System.Windows.Forms.PictureBox();
            this.comboBoxCom = new System.Windows.Forms.ComboBox();
            this.lbCom = new System.Windows.Forms.Label();
            this.lbBaud = new System.Windows.Forms.Label();
            this.comboBoxBaud = new System.Windows.Forms.ComboBox();
            this.btnOpenCom = new System.Windows.Forms.Button();
            this.btnUpPic = new System.Windows.Forms.Button();
            this.btnDownPic = new System.Windows.Forms.Button();
            this.ofdPic = new System.Windows.Forms.OpenFileDialog();
            this.picBox2 = new System.Windows.Forms.PictureBox();
            this.tbRecv = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.picBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.picBox2)).BeginInit();
            this.SuspendLayout();
            // 
            // serialPort
            // 
            this.serialPort.BaudRate = 115200;
            this.serialPort.PortName = "COM5";
            // 
            // picBox1
            // 
            this.picBox1.Image = global::MySerialPortApp.Properties.Resources.Favicon;
            this.picBox1.InitialImage = null;
            this.picBox1.Location = new System.Drawing.Point(327, 73);
            this.picBox1.Name = "picBox1";
            this.picBox1.Size = new System.Drawing.Size(292, 284);
            this.picBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.picBox1.TabIndex = 0;
            this.picBox1.TabStop = false;
            // 
            // comboBoxCom
            // 
            this.comboBoxCom.FormattingEnabled = true;
            this.comboBoxCom.Location = new System.Drawing.Point(87, 73);
            this.comboBoxCom.Name = "comboBoxCom";
            this.comboBoxCom.Size = new System.Drawing.Size(84, 20);
            this.comboBoxCom.TabIndex = 1;
            // 
            // lbCom
            // 
            this.lbCom.AutoSize = true;
            this.lbCom.Location = new System.Drawing.Point(28, 76);
            this.lbCom.Name = "lbCom";
            this.lbCom.Size = new System.Drawing.Size(41, 12);
            this.lbCom.TabIndex = 2;
            this.lbCom.Text = "串口号";
            // 
            // lbBaud
            // 
            this.lbBaud.AutoSize = true;
            this.lbBaud.Location = new System.Drawing.Point(28, 114);
            this.lbBaud.Name = "lbBaud";
            this.lbBaud.Size = new System.Drawing.Size(41, 12);
            this.lbBaud.TabIndex = 4;
            this.lbBaud.Text = "波特率";
            // 
            // comboBoxBaud
            // 
            this.comboBoxBaud.FormattingEnabled = true;
            this.comboBoxBaud.Items.AddRange(new object[] {
            "115200",
            "9600"});
            this.comboBoxBaud.Location = new System.Drawing.Point(87, 111);
            this.comboBoxBaud.Name = "comboBoxBaud";
            this.comboBoxBaud.Size = new System.Drawing.Size(84, 20);
            this.comboBoxBaud.TabIndex = 3;
            this.comboBoxBaud.Text = "115200";
            // 
            // btnOpenCom
            // 
            this.btnOpenCom.Location = new System.Drawing.Point(30, 160);
            this.btnOpenCom.Name = "btnOpenCom";
            this.btnOpenCom.Size = new System.Drawing.Size(141, 31);
            this.btnOpenCom.TabIndex = 5;
            this.btnOpenCom.Text = "打开串口";
            this.btnOpenCom.UseVisualStyleBackColor = true;
            this.btnOpenCom.Click += new System.EventHandler(this.btnOpenCom_Click);
            // 
            // btnUpPic
            // 
            this.btnUpPic.Location = new System.Drawing.Point(30, 221);
            this.btnUpPic.Name = "btnUpPic";
            this.btnUpPic.Size = new System.Drawing.Size(141, 31);
            this.btnUpPic.TabIndex = 6;
            this.btnUpPic.Text = "上传图片";
            this.btnUpPic.UseVisualStyleBackColor = true;
            this.btnUpPic.Click += new System.EventHandler(this.btnUpPic_Click);
            // 
            // btnDownPic
            // 
            this.btnDownPic.Location = new System.Drawing.Point(30, 282);
            this.btnDownPic.Name = "btnDownPic";
            this.btnDownPic.Size = new System.Drawing.Size(141, 31);
            this.btnDownPic.TabIndex = 7;
            this.btnDownPic.Text = "加载记录";
            this.btnDownPic.UseVisualStyleBackColor = true;
            this.btnDownPic.Click += new System.EventHandler(this.btnDownPic_Click);
            // 
            // ofdPic
            // 
            this.ofdPic.FileName = "openFileDialog1";
            // 
            // picBox2
            // 
            this.picBox2.Location = new System.Drawing.Point(706, 76);
            this.picBox2.Name = "picBox2";
            this.picBox2.Size = new System.Drawing.Size(292, 284);
            this.picBox2.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            this.picBox2.TabIndex = 8;
            this.picBox2.TabStop = false;
            // 
            // tbRecv
            // 
            this.tbRecv.Location = new System.Drawing.Point(30, 515);
            this.tbRecv.Multiline = true;
            this.tbRecv.Name = "tbRecv";
            this.tbRecv.Size = new System.Drawing.Size(1022, 148);
            this.tbRecv.TabIndex = 9;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(923, 414);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 10;
            this.button1.Text = "点击";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // mainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1088, 685);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.tbRecv);
            this.Controls.Add(this.picBox2);
            this.Controls.Add(this.btnDownPic);
            this.Controls.Add(this.btnUpPic);
            this.Controls.Add(this.btnOpenCom);
            this.Controls.Add(this.lbBaud);
            this.Controls.Add(this.comboBoxBaud);
            this.Controls.Add(this.lbCom);
            this.Controls.Add(this.comboBoxCom);
            this.Controls.Add(this.picBox1);
            this.Name = "mainForm";
            this.Text = "秋叶依剑";
            ((System.ComponentModel.ISupportInitialize)(this.picBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.picBox2)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.IO.Ports.SerialPort serialPort;
        private System.Windows.Forms.PictureBox picBox1;
        private System.Windows.Forms.ComboBox comboBoxCom;
        private System.Windows.Forms.Label lbCom;
        private System.Windows.Forms.Label lbBaud;
        private System.Windows.Forms.ComboBox comboBoxBaud;
        private System.Windows.Forms.Button btnOpenCom;
        private System.Windows.Forms.Button btnUpPic;
        private System.Windows.Forms.Button btnDownPic;
        private System.Windows.Forms.OpenFileDialog ofdPic;
        private System.Windows.Forms.PictureBox picBox2;
        private System.Windows.Forms.TextBox tbRecv;
        private System.Windows.Forms.Button button1;
    }
}

