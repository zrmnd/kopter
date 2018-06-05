/*
 * Created by SharpDevelop.
 * User: Egor
 * Date: 02.08.2017
 * Time: 18:48
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
namespace camera
{
	partial class MainForm
	{
		/// <summary>
		/// Designer variable used to keep track of non-visual components.
		/// </summary>
		private System.ComponentModel.IContainer components = null;
		private System.Windows.Forms.ComboBox comboBox1;
		private System.Windows.Forms.PictureBox pictureBox1;
		private System.Windows.Forms.HScrollBar hScrollBar1;
		private System.Windows.Forms.HScrollBar hScrollBar2;
		private System.Windows.Forms.HScrollBar hScrollBar3;
		private System.Windows.Forms.HScrollBar hScrollBar5;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.PictureBox pictureBox4;
		private System.Windows.Forms.PictureBox pictureBox5;
		private System.Windows.Forms.PictureBox pictureBox6;
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.TextBox textBox1;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.TextBox textBox2;
		private System.Windows.Forms.Timer timer1;
		private System.Windows.Forms.Timer timer2;
		private System.Windows.Forms.Timer timer3;
		
		/// <summary>
		/// Disposes resources used by the form.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing) {
				if (components != null) {
					components.Dispose();
				}
			}
			base.Dispose(disposing);
		}
		
		/// <summary>
		/// This method is required for Windows Forms designer support.
		/// Do not change the method contents inside the source code editor. The Forms designer might
		/// not be able to load this method if it was changed manually.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
			this.comboBox1 = new System.Windows.Forms.ComboBox();
			this.pictureBox1 = new System.Windows.Forms.PictureBox();
			this.hScrollBar1 = new System.Windows.Forms.HScrollBar();
			this.hScrollBar2 = new System.Windows.Forms.HScrollBar();
			this.hScrollBar5 = new System.Windows.Forms.HScrollBar();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.pictureBox4 = new System.Windows.Forms.PictureBox();
			this.pictureBox5 = new System.Windows.Forms.PictureBox();
			this.pictureBox6 = new System.Windows.Forms.PictureBox();
			this.hScrollBar3 = new System.Windows.Forms.HScrollBar();
			this.label3 = new System.Windows.Forms.Label();
			this.button1 = new System.Windows.Forms.Button();
			this.label5 = new System.Windows.Forms.Label();
			this.textBox1 = new System.Windows.Forms.TextBox();
			this.label6 = new System.Windows.Forms.Label();
			this.textBox2 = new System.Windows.Forms.TextBox();
			this.timer1 = new System.Windows.Forms.Timer(this.components);
			this.timer2 = new System.Windows.Forms.Timer(this.components);
			this.timer3 = new System.Windows.Forms.Timer(this.components);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox4)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox5)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox6)).BeginInit();
			this.SuspendLayout();
			// 
			// comboBox1
			// 
			this.comboBox1.FormattingEnabled = true;
			this.comboBox1.Location = new System.Drawing.Point(1094, 552);
			this.comboBox1.Name = "comboBox1";
			this.comboBox1.Size = new System.Drawing.Size(121, 24);
			this.comboBox1.TabIndex = 0;
			// 
			// pictureBox1
			// 
			this.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pictureBox1.Location = new System.Drawing.Point(12, 12);
			this.pictureBox1.Name = "pictureBox1";
			this.pictureBox1.Size = new System.Drawing.Size(720, 576);
			this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
			this.pictureBox1.TabIndex = 2;
			this.pictureBox1.TabStop = false;
			this.pictureBox1.Click += new System.EventHandler(this.PictureBox1Click);
			// 
			// hScrollBar1
			// 
			this.hScrollBar1.LargeChange = 30;
			this.hScrollBar1.Location = new System.Drawing.Point(770, 70);
			this.hScrollBar1.Maximum = 180;
			this.hScrollBar1.Name = "hScrollBar1";
			this.hScrollBar1.Size = new System.Drawing.Size(290, 72);
			this.hScrollBar1.TabIndex = 3;
			this.hScrollBar1.Scroll += new System.Windows.Forms.ScrollEventHandler(this.HScrollBar1Scroll);
			// 
			// hScrollBar2
			// 
			this.hScrollBar2.LargeChange = 30;
			this.hScrollBar2.Location = new System.Drawing.Point(770, 178);
			this.hScrollBar2.Maximum = 180;
			this.hScrollBar2.Name = "hScrollBar2";
			this.hScrollBar2.Size = new System.Drawing.Size(290, 72);
			this.hScrollBar2.TabIndex = 4;
			this.hScrollBar2.Scroll += new System.Windows.Forms.ScrollEventHandler(this.HScrollBar2Scroll);
			// 
			// hScrollBar5
			// 
			this.hScrollBar5.LargeChange = 5;
			this.hScrollBar5.Location = new System.Drawing.Point(770, 275);
			this.hScrollBar5.Maximum = 30;
			this.hScrollBar5.Name = "hScrollBar5";
			this.hScrollBar5.Size = new System.Drawing.Size(290, 72);
			this.hScrollBar5.TabIndex = 7;
			this.hScrollBar5.Scroll += new System.Windows.Forms.ScrollEventHandler(this.HScrollBar5Scroll);
			// 
			// label1
			// 
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label1.Location = new System.Drawing.Point(738, 96);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(29, 32);
			this.label1.TabIndex = 8;
			this.label1.Text = "X";
			// 
			// label2
			// 
			this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label2.Location = new System.Drawing.Point(738, 197);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(29, 32);
			this.label2.TabIndex = 9;
			this.label2.Text = "Y";
			// 
			// label4
			// 
			this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label4.Location = new System.Drawing.Point(736, 260);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(31, 102);
			this.label4.TabIndex = 11;
			this.label4.Text = "Z\r\no\r\no\r\nm";
			// 
			// pictureBox4
			// 
			this.pictureBox4.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pictureBox4.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox4.Image")));
			this.pictureBox4.Location = new System.Drawing.Point(1094, 56);
			this.pictureBox4.Name = "pictureBox4";
			this.pictureBox4.Size = new System.Drawing.Size(130, 130);
			this.pictureBox4.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
			this.pictureBox4.TabIndex = 15;
			this.pictureBox4.TabStop = false;
			this.pictureBox4.Click += new System.EventHandler(this.PictureBox4Click);
			// 
			// pictureBox5
			// 
			this.pictureBox5.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pictureBox5.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox5.Image")));
			this.pictureBox5.Location = new System.Drawing.Point(1094, 217);
			this.pictureBox5.Name = "pictureBox5";
			this.pictureBox5.Size = new System.Drawing.Size(130, 130);
			this.pictureBox5.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
			this.pictureBox5.TabIndex = 16;
			this.pictureBox5.TabStop = false;
			this.pictureBox5.Click += new System.EventHandler(this.PictureBox5Click);
			// 
			// pictureBox6
			// 
			this.pictureBox6.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pictureBox6.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox6.Image")));
			this.pictureBox6.Location = new System.Drawing.Point(1094, 375);
			this.pictureBox6.Name = "pictureBox6";
			this.pictureBox6.Size = new System.Drawing.Size(130, 130);
			this.pictureBox6.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
			this.pictureBox6.TabIndex = 17;
			this.pictureBox6.TabStop = false;
			this.pictureBox6.Click += new System.EventHandler(this.PictureBox6Click);
			// 
			// hScrollBar3
			// 
			this.hScrollBar3.LargeChange = 30;
			this.hScrollBar3.Location = new System.Drawing.Point(770, 375);
			this.hScrollBar3.Maximum = 180;
			this.hScrollBar3.Name = "hScrollBar3";
			this.hScrollBar3.Size = new System.Drawing.Size(290, 72);
			this.hScrollBar3.TabIndex = 19;
			this.hScrollBar3.Scroll += new System.Windows.Forms.ScrollEventHandler(this.HScrollBar3Scroll);
			// 
			// label3
			// 
			this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label3.Location = new System.Drawing.Point(736, 375);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(31, 102);
			this.label3.TabIndex = 20;
			this.label3.Text = "Lidar";
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(902, 473);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(130, 115);
			this.button1.TabIndex = 21;
			this.button1.Text = "Начать магнитное сканирование";
			this.button1.UseVisualStyleBackColor = true;
			this.button1.Click += new System.EventHandler(this.Button1Click);
			// 
			// label5
			// 
			this.label5.Location = new System.Drawing.Point(738, 486);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(29, 23);
			this.label5.TabIndex = 24;
			this.label5.Text = "IP";
			// 
			// textBox1
			// 
			this.textBox1.Location = new System.Drawing.Point(756, 486);
			this.textBox1.Name = "textBox1";
			this.textBox1.Size = new System.Drawing.Size(101, 22);
			this.textBox1.TabIndex = 25;
			this.textBox1.Text = "127.0.0.1";
			// 
			// label6
			// 
			this.label6.Location = new System.Drawing.Point(736, 518);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(61, 23);
			this.label6.TabIndex = 26;
			this.label6.Text = "Port";
			// 
			// textBox2
			// 
			this.textBox2.Location = new System.Drawing.Point(770, 519);
			this.textBox2.Name = "textBox2";
			this.textBox2.Size = new System.Drawing.Size(87, 22);
			this.textBox2.TabIndex = 27;
			this.textBox2.Text = "4355";
			// 
			// timer1
			// 
			this.timer1.Enabled = true;
			this.timer1.Interval = 1000;
			// 
			// timer2
			// 
			this.timer2.Interval = 2000;
			this.timer2.Tick += new System.EventHandler(this.Timer2Tick);
			// 
			// timer3
			// 
			this.timer3.Interval = 2000;
			this.timer3.Tick += new System.EventHandler(this.Timer3Tick);
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(1262, 602);
			this.Controls.Add(this.textBox2);
			this.Controls.Add(this.label6);
			this.Controls.Add(this.textBox1);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.button1);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.hScrollBar3);
			this.Controls.Add(this.pictureBox6);
			this.Controls.Add(this.pictureBox5);
			this.Controls.Add(this.pictureBox4);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.hScrollBar5);
			this.Controls.Add(this.hScrollBar2);
			this.Controls.Add(this.hScrollBar1);
			this.Controls.Add(this.pictureBox1);
			this.Controls.Add(this.comboBox1);
			this.Name = "MainForm";
			this.Text = "camera";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainFormFormClosing);
			this.Load += new System.EventHandler(this.MainFormLoad);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox4)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox5)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox6)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}
	}
}
