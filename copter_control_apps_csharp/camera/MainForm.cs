/*
 * Created by SharpDevelop.
 * User: Egor
 * Date: 02.08.2017
 * Time: 18:48
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using System.IO.Ports;
using System.Reflection;
using Microsoft.Win32;
using System.IO;

using System.Text;

using AForge.Video;
using AForge.Video.DirectShow;

using System.Net;
using System.Net.Sockets;

namespace camera
{
	/// <summary>
	/// Description of MainForm.
	/// </summary>
	public partial class MainForm : Form
	{
		public MainForm()
		{
			//
			// The InitializeComponent() call is required for Windows Forms designer support.
			//
			InitializeComponent();
			
			//
			// TODO: Add constructor code after the InitializeComponent() call.
			//
		}
	
		private FilterInfoCollection CaptureDevice; // list of webcam
        private VideoCaptureDevice FinalFrame;
        private Bitmap varBmp, oldBmp;
        bool isIntrosRun = false;
        
        
		int sendCmd(string cmd) {
			int res = 0;
			IPAddress remoteIPAddress;
        	int remotePort;
			remoteIPAddress = IPAddress.Parse(textBox1.Text);
			remotePort = Convert.ToInt16(textBox2.Text);			
			TcpClient sendingTcpClient = new TcpClient();        
            IPEndPoint endPoint = new IPEndPoint(remoteIPAddress, remotePort);  // Создаем endPoint по информации об удаленном хосте

            try {
            	sendingTcpClient.Connect(endPoint);
                byte[] bytes = Encoding.UTF8.GetBytes(cmd); // Преобразуем данные в массив байтов
                sendingTcpClient.GetStream().Write(bytes,  0, bytes.Length);		        
            }
            catch (Exception ex) {
            	res = 1;           	
            }
            finally {             
                sendingTcpClient.Close();  // Закрыть соединение
            }
            return res;
		}
		
		
		

        void SetCamXY(int x, int y) {
        	sendCmd("$PUI03,0,1," + x.ToString() +"," + y.ToString() + ",0,\n");
        }

        
        void SetLidarPos(int pos) {
			sendCmd("$PUI03,0,2," + pos.ToString() +",0,0,\n");
        }
        
        void SetZoom(int value) {
        	sendCmd("$PUI06,0,1," + value.ToString() +",\n");
        }
        
        void TakePhoto() {
        	if (pictureBox1.Image != null) {
	        	string exePath =  Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
	        	string s = DateTime.Now.ToString("dd.MM.yyyy_HH-mm-ss")+".jpg";
	        	pictureBox1.Image.Save(Path.Combine(exePath, s));
        	}
        }
        
        
        
        void MovePlatformForward() {
        	sendCmd("$PUI12,0,3,\n");
        }
        
        void MovePlatformReverse() {
        	sendCmd("$PUI12,0,-3,\n");
        }
        
        void StopPlatform() {
        	sendCmd("$PUC01,0,\n");
        }
        
        void IntrosStartScan() {
        	if (sendCmd("$PUI23,0,\n") == 0) 
        		isIntrosRun = true;
        }
        
        void IntrosStopScan() {
        	if (sendCmd("$PUI24,0,\n") == 0) 
        		isIntrosRun = false;
        }
        
        void SearchAndOpenVideoDevice() {
        	CaptureDevice = new FilterInfoCollection(FilterCategory.VideoInputDevice);//constructor
			int i = 0, index = -1;
			comboBox1.Items.Clear();
            foreach (FilterInfo Device in CaptureDevice)
            {
                comboBox1.Items.Add(Device.Name);
                if (Device.Name == "AVerMedia USB Polaris Analog Capture") // Имя платы видеозахвата
                	index = i;
                i++;
            }
            
            if (index == -1) { // устройство не найдено
            	
            	return ;
            } 
            else {           	
	            comboBox1.SelectedIndex = index; // default
	            FinalFrame = new VideoCaptureDevice();
				FinalFrame = new VideoCaptureDevice(CaptureDevice[comboBox1.SelectedIndex].MonikerString);// specified web cam and its filter moniker string
	            FinalFrame.NewFrame += new NewFrameEventHandler(FinalFrame_NewFrame);// click button event is fired, 
	            
	            //FinalFrame.VideoResolution = FinalFrame.VideoCapabilities[8];
	            FinalFrame.CrossbarVideoInput = FinalFrame.AvailableCrossbarVideoInputs [1];
	             
	 
	            FinalFrame.Start(); 
            }     	
        }
        
        
    
    	

        
		void MainFormLoad(object sender, EventArgs e)
		{
			// ---------------------------------------------сделать, чтобы кнопки при наведении курсора меняли цвет 
			SearchAndOpenVideoDevice();
			
			
		}
		
		void FinalFrame_NewFrame(object sender, NewFrameEventArgs eventArgs) // must be void so that it can be accessed everywhere.
    // New Frame Event Args is an constructor of a class
        {     		
			if (oldBmp != null)
				oldBmp.Dispose();		
			oldBmp = varBmp;		
			varBmp = new Bitmap(eventArgs.Frame);					
			if (varBmp != null) 
				pictureBox1.Image = varBmp;			
        }
		

		void MainFormFormClosing(object sender, FormClosingEventArgs e)
		{
			if (FinalFrame != null)
				if (FinalFrame.IsRunning==true) FinalFrame.Stop();
			
			
		}
		void Timer1Tick(object sender, EventArgs e)
		{
			if (FinalFrame != null)
				if (FinalFrame.IsRunning==false)
					SearchAndOpenVideoDevice();
			if (FinalFrame == null)
				SearchAndOpenVideoDevice();
				
		}

		void PictureBox4Click(object sender, EventArgs e)
		{
			timer2.Enabled = true;
			timer3.Enabled = false;
			MovePlatformForward();
		}
		void PictureBox6Click(object sender, EventArgs e)
		{
			timer3.Enabled = true;
			timer2.Enabled = false;
			MovePlatformReverse();
		}
		void PictureBox5Click(object sender, EventArgs e)
		{
			timer2.Enabled = false;
			timer3.Enabled = false;
		}
		void HScrollBar1Scroll(object sender, ScrollEventArgs e)
		{
			SetCamXY(hScrollBar1.Value, hScrollBar2.Value);
		}
		void HScrollBar2Scroll(object sender, ScrollEventArgs e)
		{
			SetCamXY(hScrollBar1.Value, hScrollBar2.Value);
		}
		void HScrollBar3Scroll(object sender, ScrollEventArgs e)
		{
			SetLidarPos(hScrollBar3.Value);
		}
		void HScrollBar5Scroll(object sender, ScrollEventArgs e)
		{
			SetZoom(hScrollBar5.Value);
		}
		void PictureBox1Click(object sender, EventArgs e)
		{
	 		TakePhoto();
		}
		void Timer2Tick(object sender, EventArgs e)
		{
			MovePlatformForward();
		}
		void Timer3Tick(object sender, EventArgs e)
		{
			MovePlatformReverse();
		}
		void Button1Click(object sender, EventArgs e)
		{
			if (isIntrosRun) {
				IntrosStopScan();
				button1.Text = "Начать магнитное сканирование";
			}
			else {
				IntrosStartScan();
				button1.Text = "Остановить магнитное сканирование";
			}
				
		}
		
		
		
		
		
	}
}
