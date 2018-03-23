import socket
import time


s = "wewwfwef f SER=0000601 LOCATION "
i = s.find("SWER")
print i
s = s[i+4:]
i = s.find(' ')
s = s[:i]
print s 







#sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
if 1:
	try:
		sock = socket.socket()
		sock.connect( ("192.168.8.34", 4355) )
		sock.send('$PUI12,77,15.1,\n')
		data = sock.recv(16384)
		#udata = data.decode("utf-8")
		print("Data: " +data)		
		sock.close()
	except: 
		print "Error!"
	time.sleep(1)
