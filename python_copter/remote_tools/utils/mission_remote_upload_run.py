import socket
import time


IP_COPTER = "192.168.1.66"
PORT_SUPERVISOR = 4356
PORT_FTP = 21
PASS_FTP = "saigon"
USER_FTP = "service"





#sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
if 1:
	try:
		sock = socket.socket()
		#sock.connect( ("192.168.8.34", 4355) )
		sock.connect( ("192.168.1.66", 4355) )
		sock.send('$PUI12,77,15.1,\n')
		data = sock.recv(16384)
		#udata = data.decode("utf-8")
		print("Data: " +data)		
		sock.close()
	except: 
		print "Error!"
	time.sleep(1)
