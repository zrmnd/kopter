#coding=UTF8
import serial
import base64
import time
from serial.tools import list_ports




	
	
cmdPlatformInit = "UART\r"
cmdPlatformFwd = "M1L!\r"
cmdPlatformRev = "M1R!\r"
cmdPlatformStop = "M1SS\rM1BB\r"


cmdUartS = "$INE01,77,2,115200,\n"
cmdUartSendBegin = "$INE02,77,2,"
cmdUartSendEnd = ",\n"
#cmdUartSendRxEnd = "$INE09,77,3,\n"



def findIntros():
	ports_avaiable = list(list_ports.comports())
	t_port = ""
	for port in ports_avaiable:
		s = port[2]
		if not s.find('502') == -1:
			t_port = port[0]
	try:	
		ser = serial.Serial(t_port)
		ser.baudrate = 9600
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
				time.sleep(0.1)
			print "sended: " + cmd
			return 1
		except:
			return 0
			
			
def SendToIntros(ser, cmd):	
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



			
def introsStartScan(ser):			
	pass		


def introsStopScan(ser):
	pass
			

	





cmds = []
cmds.append(b"$PEE01,53,1,9600,FF\n")
cmds.append(b"$PEE01,54,2,115200,0F\n")
cmds.append(b"$PEE02,55,1,dG92YXJpc2NoIQ==,\n")
cmds.append(b"$INE03,56,1,0,\n")
cmds.append(b"$INE03,57,1,180,\n")
cmds.append(b"$INE03,58,2,90,\n")
cmds.append(b"$C1E05,59,1,aGV4QQ==,\n")
cmds.append(b"$INE06,60,BAT 12.5V,11\n")
cmds.append(b"$INE07,61,2,1,\n")
cmds.append(b"$INE07,61,2,0,\n")
cmds.append(b"$INE07,61,4,1,\n")
cmds.append(b"$INE07,61,4,0,AA\n")
cmds.append(b"$INE08,56,\n")
cmds.append(b"$INE09,56,\n")

cmd1 = b"$INE08,77,1,\n"
cmd2 = b"$INE09,77,1,\n"

cmdPlatformInit = b"UART\r"
cmdPlatformFwd = b"M1L!\r"
cmdPlatformRev = b"M1R!\r"
cmdPlatformStop = b"M1SS\rM1BB\r"

s = "TTFTUw1NMUJCDQ=="
encoded = base64.b64encode(b'data to be encoded')
data = base64.b64decode("TTFTUw1NMUJCDQ==")
print encoded
print data
print base64.b64decode(s)



ports_avaiable = list(list_ports.comports())
		
for port in ports_avaiable:
	print port[0] + " - " + port[1] + " - " + port[2]

	
	
	
ser = findIntros()
sendToIntrosBoard(ser, b'\r\r\nKEYBOARD\r7\r\r\n')
sendToIntrosBoard(ser, b'\r\nWORDS\r\r\n')
	



#sendToIntrosBoard(ser, u'\x4B\x45\x59\x42\x4F\x41\x52\x44\r\n\n'.encode('cp1251'))
#sendToIntrosBoard(ser, u'\n\n7\n\n\r'.encode('cp1251'))
	
#time.sleep(1)
#introsPwrOnOff(ser)

#SendToIntros(ser, b'\x1B')   #KEYBOARD
#SendToIntros(ser, b'KEYBOARD\r')
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
	
	