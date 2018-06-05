#coding=UTF8
import serial
import base64
import time
from serial.tools import list_ports








def findIntros():
	ports_avaiable = list(list_ports.comports())
	t_port = ""
	for port in ports_avaiable:
		s = port[2]
		if not s.find('502') == -1:
			t_port = port[0]
	try:
		ser = serial.Serial(t_port)
		ser.baudrate = 115200
		print "ser opened"
	except:
		ser = ""
	return ser



def sendToIntrosBoard(ser, cmd):
	if not ser == "":
		try:
			for char in cmd:
				#ser.write(cmd.encode())
				ser.write(char)
				time.sleep(0.3)
			print "sended: " + cmd
			return 1
		except:
			return 0


def SendToController(ser, cmd):
	if not ser == "":
		try:
			cmdE = base64.b64encode(cmd)
			str = cmdUartSendBegin+cmdE+cmdUartSendEnd
			sendToIntrosBoard(ser, str)

			return 1
		except:
			return 0


def introsPwrOnOff(ser):
	sendToIntrosBoard(ser, "$INE07,77,2,1,\n")
	time.sleep(1)
	sendToIntrosBoard(ser, "$INE07,77,2,0,\n")
	time.sleep(1)


def introsInit(ser):
    sendToIntrosBoard(ser, b'\r\r\nKEYBOARD\r7\r\r')
    sendToIntrosBoard(ser, b'\r\r12\r0\r0\r1\r3\r18\rTIME!\r\r')
    sendToIntrosBoard(ser, b'\r\r12\r0\r0\r1\r3\r18\rTIME!\r\r')
    sendToIntrosBoard(ser, b'\r\r12\r0\r0\r1\r3\r18\rTIME!\r\r')
    pass

def introsStartScan(ser):
    sendToIntrosBoard(ser, b'\r\r10\rSpd!\r\r')
    sendToIntrosBoard(ser, b'\r\r10\rSpd!\r\r')
    sendToIntrosBoard(ser, b'\r\r\nKEYBOARD\r9\r\r')



def introsStopScan(ser):
    sendToIntrosBoard(ser, b'\r\r0\rSpd!\r\r')
    sendToIntrosBoard(ser, b'\r\r0\rSpd!\r\r')
    sendToIntrosBoard(ser, b'\r\r\nKEYBOARD\r9\r\r')








ports_avaiable = list(list_ports.comports())

for port in ports_avaiable:
	print port[0] + " - " + port[1] + " - " + port[2]



	
	
	
	
class magneticScaner(object):
	_ser = ""
	def __init__(self):
		pass
	def init(self):
		try:
			if not self._ser == "":
				self._ser.close()
		except:
			self._ser = ""
		ports_avaiable = list(list_ports.comports())
		t_port = ""
		for port in ports_avaiable:
			s = port[2]
			if not s.find('502') == -1:
				t_port = port[0]
				print "found " + t_port
		try:	
			print "try open"
			self._ser = serial.Serial(t_port)
			self._ser.baudrate = 115200
			print "intros opened"
			self._ser.write(b'\r\r\nKEYBOARD\r7\r\r')
			print "write ok"
			return 1
		except: 
			print "open error"
			self._ser = ""
		return 0
		
	def powerOnOff(self):
		if self._ser == "":
			return 0
		print "power on\off"
		self.sendRaw(b'$INE07,77,2,1,\n')
		#time.sleep(1)
		#self.sendRaw(b'$INE07,77,2,0,\n')
		time.sleep(3)
		return 1
	def initialize(self):
		if self._ser == "":
			return 0
		print "intros initializing..."
		self.sendRaw(b'\r\r\nKEYBOARD\r7\r\r')
		self.sendRaw(b'\r\r\nKEYBOARD\r7\r\r')
		time.sleep(0.5)
		self.sendRaw(b'\r\r12 0 0 1 4 18\r')
		time.sleep(0.3)
		self.sendRaw('TIME!\r')
		time.sleep(0.3)
		self.sendRaw(b'\r\r12 0 0 1 4 18\rTIME!\r\r')
		time.sleep(0.3)
		return 1
	def startScan(self):
		if self._ser == "":
			return 0
		print "intros start scan"
		self.sendRaw(b'\r')
		time.sleep(0.3)
		self.sendRaw(b'10\r')
		time.sleep(0.3)
		self.sendRaw(b'Spd!\r')
		time.sleep(0.5)
		self.sendRaw(b'\r')
		time.sleep(0.3)
		self.sendRaw(b'10\r')
		time.sleep(0.3)
		self.sendRaw(b'Spd!\r')
		time.sleep(0.5)
		self.sendRaw(b'\r')
		time.sleep(0.3)
		self.sendRaw(b'KEYBOARD\r9\r')
		time.sleep(0.3)
		self.sendRaw(b'\r')
		time.sleep(0.3)
		return 1
	def stopScan(self):
		if self._ser == "":
			return 0
		print "intros stop scan"
		self.sendRaw(b'\r')
		time.sleep(0.3)
		self.sendRaw(b'0\r')
		time.sleep(0.3)
		self.sendRaw(b'Spd!\r')
		time.sleep(0.5)
		self.sendRaw(b'\r')
		time.sleep(0.3)
		self.sendRaw(b'0\r')
		time.sleep(0.3)
		self.sendRaw(b'Spd!\r')
		time.sleep(0.5)
		self.sendRaw(b'\r')
		time.sleep(0.3)
		self.sendRaw(b'KEYBOARD\r9\r')
		time.sleep(0.3)
		self.sendRaw(b'\r')
		time.sleep(0.3)
		return 1
	def isOn(self): #####                         fix 
		return 1		
	def sendRaw(self, data):
		print "send raw: " + data
		if not self._ser == "": 
			try:
				print "try write" + data
				self._ser.write(data.encode())
				print data
				time.sleep(0.5)
				return 1
			except:
				print "write error"
				return 0			
	def readData(self):
		data = ""
		if not self._ser == "":
			while self._ser.inWaiting():
				data = data + self._ser.read()
		return data
	def closeConnection(self):
		try:
			self._ser.close()
		except:
			pass
pass	


try: 


	intros = magneticScaner()
	intros.init()
	intros.powerOnOff()
	intros.initialize()	
	intros.readData()
	intros.startScan()
	intros.readData()
	time.sleep(5)
	intros.stopScan()	
	intros.readData()
		
		
		
		
	print "!11111111111"
	#time.sleep(100)	
		
		
		

	ser = findIntros()
	introsInit(ser)
	print "ser:"
	print ser

	time.sleep(50)

	sendToIntrosBoard(ser, b'\rTIME@\r\r\n')
	introsStartScan(ser)
	time.sleep(5)
	introsStopScan(ser)



	i = 0
	while True:
		i = i + 1
		#if i < 10:
		#	SendToIntros(ser, b'KEYBOARD\r')
		data = ""
		if not ser == "":
			while ser.inWaiting():
				data = data + ser.read()
			if not data == "":
				print data
except KeyboardInterrupt:
	intros.closeConnection()
