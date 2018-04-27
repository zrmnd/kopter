#coding=CP1251
import socket
import multiprocessing 
import time
import errno
import os
from ctypes import c_bool
import sys
import glob
import os.path


KEYS_HANDLER_PORT = 4353  
INTERPRETER_PORT = 4355
SUPERVISOR_PORT = 4356
IPAUTOCONF_PORT = 4351


class ModuleIP(object):
	def __init__(self):
		self._mutex = multiprocessing.Lock()
		self._ip = ""
		self._ipTime = 0
		
	def incTime(self):
		self._mutex.acquire()
		self._ipTime = self._ipTime + 1
		self._mutex.release()
	def setIp(self, ip):
		self._mutex.acquire()
		self._ipTime = 0
		self._ip = ip
		self._mutex.release()
	def get(self):
		self._mutex.acquire()
		return (self._ip, self._ipTime)
		self._mutex.release()
pass





def ip_autoconf_sender(flagStopThread, missionEditorIp, pultIp, missionIp):
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
	while flagStopThread.value == 0:
		sock.sendto('$CPA00,COPTER V1.0,\n',('192.168.8.255', IPAUTOCONF_PORT))
		missionEditorIp.incTime()
		pultIp.incTime()
		missionIp.incTime()
		time.sleep(1)
	pass
	sock.close()

	
def ip_autoconf_receiver(flagStopThread, missionEditorIp, missionIp, pultIp):
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
	sock.bind(('0.0.0.0', IPAUTOCONF_PORT))
	while flagStopThread.value == 0:
		message, sender_addr = sock.recvfrom(128)
		print "IP Autoconf package received, ", message
		if message[0:6] == "$MEA00":
			ip, port = sender_addr
			missionEditorIp.setIp(ip)
			print "Mission Editor IP is", ip, "was updated"
		elif message[0:6] == "$MSA00":
			ip, port = sender_addr
			missionIp.setIp(ip)
			print "Mission IP is", ip, "was updated"
		elif message[0:6] == "$PUA00":
			ip, port = sender_addr
			pultIp.setIp(ip)
			print "Pult IP is", ip, "was updated"
		elif message[0:6] == "$CPA00":
			pass
		#if not message[0:6] == '$CPA00':
			#print message, sender_addr
	pass
	sock.close() 
 
 
 
 
 
 

	

# todo: 
# > IPAutoconf parsing
# > Input comands byte-by-byte receiving and parsing, sending answers to pult




	

	
	



try:

	if __name__ == '__main__':
	
		print "Main run...\n"
		
		
		pultIp = ModuleIP()
		missionEditorIp = ModuleIP()
		missionIp = ModuleIP()
		
		
		
		

		print "srv starting\n"
		srv_interpreter = socket.socket()	
		srv_interpreter.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		srv_interpreter.bind( ("", KEYS_HANDLER_PORT) )
		srv_interpreter.listen(3)
		srv_interpreter.setblocking(0)
		
		newclient = ""
		client = ""
		

		while True:
			# try connect client
			try:
				newclient, addr = srv_interpreter.accept()
				print "Accepted new connection"
				try:
					if not client == "":
						client.close()
						print "Old connecion dropped"
				except:
					pass
				client = newclient
				client.setblocking(0) 
			except:
				pass
				
			 
					
			# try receive data
			data = ""
			try:
				if not client == "":
					tmp = client.recv(1024)
					while tmp:
						data = data + tmp
						tmp = client.recv(1024)					
			except socket.error, e: # no data
				if e.args[0] == errno.EAGAIN or e.args[0] == errno.EWOULDBLOCK:
					#print "No data\n" 
					pass					
				time.sleep(0.1)				
			if not data == "": # data was received
				print "Data from pult ESP8266 received: " + data
				data = ""

				try:
					client.send(tmp)
				except socket.error: 
					print "send error, drop connection"
					
				
				
			time.sleep(0.1)
			

except KeyboardInterrupt:
	if __name__ == '__main__':
		print "Terminating..."
		
		try:
			client.close()				
		except socket.error:
			pass
	raise
	