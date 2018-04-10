import socket
import time








#sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
if 1:
	try:
		sock = socket.socket()
		#sock.connect( ("192.168.8.34", 4355) )
		sock.connect( ("localhost", 4355) )
		sock.send('$PUI23,77,1,3,2,15.1,\n') #12 
		data = sock.recv(16384)
		#udata = data.decode("utf-8")
		print("Data: " +data)		
		sock.close()
	except: 
		print "Error!"
	time.sleep(1)
