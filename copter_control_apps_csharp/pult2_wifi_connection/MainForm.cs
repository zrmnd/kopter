/*
 * Created by SharpDevelop.
 * User: Egor
 * Date: 20.12.2017
 * Time: 15:17
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace pult2_wifi_connection
{
	/// <summary>
	/// Description of MainForm.
	/// </summary>
	/// 
	public partial class MainForm : Form
	{
		
		

         
		
        
		public MainForm()
		{
			//
			// The InitializeComponent() call is required for Windows Forms designer support.
			//
			InitializeComponent();
			
			System.Windows.Forms.Control.CheckForIllegalCrossThreadCalls = false;

			//
			// TODO: Add constructor code after the InitializeComponent() call.
			//
		}
		
		
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
		
		void MainFormLoad(object sender, EventArgs e)
		{

			
			

            //receivingTcpClient = new TcpClient(localPort);
            //receivingTcpClient
    		//receivingTcpClient.BeginReceive(DataReceived, null);
	
		}
		void Button1Click(object sender, EventArgs e)
		{			
			
	
		}
		void Timer1Tick(object sender, EventArgs e)
		{

		}
		void MainFormFormClosing(object sender, FormClosingEventArgs e)
		{

		}
		void RichTextBox1MouseDoubleClick(object sender, MouseEventArgs e)
		{
			this.Text = "Sending...";
			int index = richTextBox1.SelectionStart;
			int line = richTextBox1.GetLineFromCharIndex(index);
			if (sendCmd(richTextBox1.Lines[line])==1) 
				this.Text = "Error send";
			else
				this.Text = "Success send";
		}
		
        
		    
		
	}
	
	
	
	
	
	
	
}
