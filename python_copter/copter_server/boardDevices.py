#coding=UTF-8
import multiprocessing 
import serial
import base64
from serial.tools import list_ports
import time
import sys
import glob
import base64



# class for controlling and communicating with main board  
class boardMain(object):
	def __init__(self):
		self._mutex = multiprocessing.Lock()
		self.ser = ""
	def init(self):
		if not self.ser == "":
			try:
				self.ser.close()
			except:
				pass	
		ports_avaiable = list(list_ports.comports())
		t_port = ""
		for port in ports_avaiable:
			s = port[2]
			if not s.find('601') == -1:
				t_port = port[0]
		try:	
			self.ser = serial.Serial(t_port)
			self.ser.baudrate = 115200
			self.ser.timeout = 1
			print "ser opened"
			return 1
		except: 
			self.ser = ""
		return 0
		
	
		
	def isOk(self):
		return self.init()
		
	def sendRaw(self, data):
		if not self.ser == "": 
			try:		
				self.ser.write(cmd.encode())
				print cmd
				time.sleep(0.5)
				return 1
			except:
				return 0
		
	def ctrlBulletPower(self, state):
		pass
	def ctrlOdroidPower(self, f):
		pass
	def readData(self):
		data = ""
		if not self.ser == "":
			while self.ser.inWaiting() > 0:
				data = data + self.ser.read()
		return data
	
		
		

mainBoard = boardMain()
mainBoard.init()
		

cmdPlatformInit = "UART\r"
cmdPlatformFwd = "M1L!\r"
cmdPlatformRev = "M1R!\r"
cmdPlatformStop = "M1SS\rM1BB\r"


#cmdUartS = "$INE01,77,3,9600,\n"
#cmdUartSendBegin = "$INE02,77,3,"
#cmdUartSendEnd = ",\n"		
#cmdUartSendRxEnd = "$INE09,77,3,\n"	

# class for controlling wheeled platform module  
class boardPlatform(object):
	def __init__(self):
		self._mutex = multiprocessing.Lock()

	def init1(self):
		if mainBoard.init() == 0:
			return 0
		try:
			print "try write"
			#sendToMain(ser, cmdUartS)
			mainBoard.sendRaw(base64.b64encode(cmdPlatformInit))
			mainBoard.sendRaw(base64.b64encode(cmdPlatformInit))
			mainBoard.sendRaw(cmdPlatformInit)
			mainBoard.sendRaw(cmdPlatformInit)
			print "write ok"
			print mainBoard.readData()
			return 1
		except: 
			return 0
			
	def isRunning(self):
		return 0	
	def isOk(self):
		return mainBoard.init()
	def stop(self):
		if mainBoard.init() == 0:
			return 0
		return mainBoard.sendRaw(cmdPlatformStop) 
	def run(self, f):
		if mainBoard.init() == 0:
			return 0
		r = 0
		if f > 0:
			if mainBoard.sendRaw(cmdPlatformFwd) == 1:
				r = 1
		else: 
			if mainBoard.sendRaw(cmdPlatformRev) == 1:
				r = 1
			f = -f
		if r == 1:
			time.sleep(f)
			if self.stop() == 0:
				r = r & self.stop()
		return r

	def sendRaw(self, data):
		if mainBoard.init() == 0:
			return 0
		return mainBoard.sendRaw(data)	
	def readData(self):
		return mainBoard.readData()
	





wheeledPlatform = boardPlatform()
wheeledPlatform.stop()

if wheeledPlatform.init1() == 1:
	print "Init ok"
else:
	print "Init error"
	
	
	
# class for controlling and communicating with Intros device 
class magneticScaner(object):
	def __init__(self):
		self._mutex = multiprocessing.Lock()
		self.ser = ""
	def init(self):
		if not self.ser == "":
			try:
				self.ser.close()
			except:
				pass	
		ports_avaiable = list(list_ports.comports())
		t_port = ""
		for port in ports_avaiable:
			s = port[2]
			if not s.find('502') == -1:
				t_port = port[0]
		try:	
			self.ser = serial.Serial(t_port)
			self.ser.baudrate = 115200
			self.ser.timeout = 1
			print "intros opened"
			return 1
		except: 
			self.ser = ""
		return 0
		
	def initialize(self):
		if self.init() == 0:
			return 0
		print "intros initializing..."
		self.sendRaw(b'\r')
		time.sleep(0.5)
		self.sendRaw(b'KEYBOARD') 
		time.sleep(0.1)
		self.sendRaw(b'\r') 
		time.sleep(0.3)
		self.sendRaw(b'7') 
		self.sendRaw(b'7') 
		time.sleep(0.3)
		self.sendRaw(b'\r') 
		time.sleep(0.1)
		self.sendRaw(b'\r') 
		time.sleep(0.5)

		time.sleep(0.5)
		self.sendRaw(b'12 0 0 1 4 18\r')
		time.sleep(0.5)
		self.sendRaw('TIME!\r')
		time.sleep(0.5)

		return 1
	def startScan(self):
		if self.init() == 0:
			return 0
		print "intros start scan"
		self.sendRaw(b'\r')
		time.sleep(0.5)
		self.sendRaw(b'10\r')
		time.sleep(0.5)
		self.sendRaw(b'Spd!\r')
		time.sleep(0.5)
		self.sendRaw(b'\r')
		time.sleep(0.5)
		self.sendRaw(b'10\r')
		time.sleep(0.5)
		self.sendRaw(b'Spd!\r')
		time.sleep(0.5)
		self.sendRaw(b'\r')
		time.sleep(0.5)
		self.sendRaw(b'KEYBOARD\r9\r')
		time.sleep(0.5)
		self.sendRaw(b'\r')
		time.sleep(0.5)
		return 1
	def stopScan(self):
		if self.init() == 0:
			return 0
		print "intros stop scan"
		self.sendRaw(b'\r')
		time.sleep(1)
		self.sendRaw(b'KEYBOARD\r9\r')
		time.sleep(1)
		self.sendRaw(b'\r')
		time.sleep(0.5)
		self.sendRaw(b'\r')
		time.sleep(0.5)
		self.sendRaw(b'0\r')
		time.sleep(0.7)
		self.sendRaw(b'Spd!\r')
		time.sleep(1)
		self.sendRaw(b'\r')
		time.sleep(0.5)
		self.sendRaw(b'0\r')
		time.sleep(0.5)
		self.sendRaw(b'Spd!\r')
		time.sleep(1)
		print self.readData()
		
		return 1
	def isOn(self): #####                         fix 
		return 1		
	def sendRaw(self, data):
		if not self.ser == "": 
			try:		
				self.ser.write(data.encode())
				#print data
				time.sleep(0.5)
				return 1
			except:
				return 0			
	def readData(self):
		data = ""
		if not self.ser == "":
			while self.ser.inWaiting():
				data = data + self.ser.read()
		return data
	
pass	


intros = magneticScaner()






# class for controlling and communicating with camera controller and lidar controller 
class boardCamera(object):
	def __init__(self):
		self._mutex = multiprocessing.Lock()
		self.ser = ""
	def init(self, id):
		self._id = id
		if not self.ser == "":
			try:
				self.ser.close()
			except:
				pass	
		ports_avaiable = list(list_ports.comports())
		t_port = ""
		for port in ports_avaiable:
			s = port[2]
			if not s.find(self._id) == -1:
				t_port = port[0]
		try:	
			self.ser = serial.Serial(t_port)
			self.ser.baudrate = 115200
			print "Camera" + self._id + " ser opened"
			return 1
		except: 
			self.ser = ""
		return 0
		
	
		
	def isOk(self):
		return self.init()
		
	def sendRaw(self, data):
		if not self.ser == "": 
			try:		
				self.ser.write(cmd.encode())
				print cmd
				time.sleep(0.5)
				return 1
			except:
				return 0
		
	def setPos(self, x, y, z):
		print "set pos stub"		
		self.sendRaw('$INE03,77,1,'+str(x)+',\n')
		self.sendRaw('$INE03,77,2,'+str(y)+',\n')
		self.sendRaw('$INE03,77,3,'+str(z)+',\n')
		pass
	def setZoom(self, z):
		print "set zoom stub"
		#self.sendRaw(base64.b64encode(""))
		pass
	def readData(self):
		data = ""
		if not self.ser == "":
			while self.ser.inWaiting():
				data = data + self.ser.read()
		return data

camera1 = boardCamera() 
camera1.init('501')
lidar = boardCamera()
lidar.init('503')




