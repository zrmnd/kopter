import serial
import base64
from serial.tools import list_ports

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


s = "dG92YXJpc2NoIQ=="
encoded = base64.b64encode(b'data to be encoded')
data = base64.b64decode(encoded)
print encoded
print data
print base64.b64decode(s)



ports_avaiable = list(list_ports.comports())
		
for port in ports_avaiable:
	print port[0] + " - " + port[1] + " - " + port[2]

	
try: 	
	with serial.Serial('COM5', 19200, timeout=1) as ser:
		ser.write(cmds[12].encode())
	#ser.write("test\n\r\0".encode())
except: 
	print "not connected to COM5"