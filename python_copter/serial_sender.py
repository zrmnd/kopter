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




ser = findIntros()
introsInit(ser)

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

