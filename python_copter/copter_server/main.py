#coding=CP1251
import socket
import multiprocessing 
import time
import errno
from multiprocessing import Process, Manager, Value
from multiprocessing.managers import BaseManager
import os
from ctypes import c_bool
import sys
import glob
import os.path
from cmdAgent import comandAgent
import boardDevices

#449 line - wait ser IO error 

INTERPRETER_PORT = 4355
SUPERVISOR_PORT = 4358
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




def IsStopAllCmd(cmd):
	if not cmd == "":
		if cmd[0] == "$" and cmd[3:6] == "C01": # stop all cmd
			# add here checksum check
			print "StopAllCmd\n"
			
			return True
		else:
			return False

def IsStopCmd(cmd):
	if not cmd == "":
		if cmd[0] == "$" and cmd[3:6] == "C01": # stop all cmd
			# add here checksum check
			print "StopCmd\n"
			return True
		else:
			return False
 



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
 
 
 
 
 
 

def srv_interpreter_thread(cmdAgent, flagStopThread, flagDropConnection): #receive comands over TCP-IP, handle them and send answers
	print "srv_interpreter_thread start\n"
	srv_interpreter = socket.socket()	
	srv_interpreter.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	srv_interpreter.bind( ("", INTERPRETER_PORT) )
	srv_interpreter.listen(3)
	srv_interpreter.setblocking(0)
	cmdAgent.setReady()
	
	newclient = ""
	client = ""

	while flagStopThread.value == 0:
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
				while tmp and (flagStopThread.value == 0):
					data = data + tmp
					tmp = client.recv(1024)					
		except socket.error, e: # no data
			if e.args[0] == errno.EAGAIN or e.args[0] == errno.EWOULDBLOCK:
				#print "No data\n" 
				pass
			else: 
				flagDropConnection.value = 1						
			time.sleep(0.1)				
		if not data == "": # data was received
			if cmdAgent.isReady() == 1:
				cmdAgent.runCmd(data)
				print "cmdAgent runCmd by interpreter: " + data
			else: 
				if IsStopCmd(data):
					print "Stop operation cmd received"
					cmdAgent.stop()
			data = ""
		
		while cmdAgent.hasData() and (flagStopThread.value == 0):
			tmp = cmdAgent.fetchData()
			print "hasData: " + tmp
			try:
				client.send(tmp)
			except socket.error: 
				print "send error, drop connection"
				#flagDropConnection.value = 1
			
			
		time.sleep(0.1)
		
	try:
		client.close()				
	except socket.error:
		pass
	
	try:
		srv_interpreter.stop()
	except:
		pass
	print "srv_interpreter_thread terminated"






def srv_supervisor_thread(cmdAgent, flagStopThread, flagDropConnection): #receive control comands over TCP-IP and execute them
	print "srv_supervisor_thread start"
	srv_supervisor = socket.socket()
	srv_supervisor.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	srv_supervisor.bind( ("", SUPERVISOR_PORT) )
	srv_supervisor.listen(10)
	srv_supervisor.setblocking(0)
	while flagStopThread.value == 0:	  
		# try accept client
		try:
			client, addr = srv_supervisor.accept()
			# клиенты есть
			print "Accepted by supervisor...\n"
			#client.setblocking(0)
			client.settimeout(1)
			#parse(client, addr)
			try:
				data = ""
				tmp = ""
				tmp = client.recv(1024)
				while tmp:
					data = data + tmp
					tmp = client.recv(1024)
					
			except socket.error, e: # no data or connection dropped
				if e.args[0] == errno.EAGAIN or e.args[0] == errno.EWOULDBLOCK:
						#print "No data\n" 
						pass		
				time.sleep(0.1) 
			if not data == "": # if data no empty, run executing
				print "Data received by supervisor..."
				if IsStopAllCmd(data):
					flagDropConnection.value = 1
										
				
				print("Supervisor received cmd: " + data)
				client.send("Your data: " + data)
			client.close()
				
		except socket.error: # no clients
			pass 
			
		time.sleep(0.1)
	try:
		srv_supervisor.stop()
	except:
		pass
	print "srv_supervisor_thread terminated"


	

	
	



try:

	if __name__ == '__main__':
	
		print "Main run...\n"
		
		#ports_avaiable = list(list_ports.comports())
		
		#for port in ports_avaiable:
			#print port[0] + " - " + port[1] + " - " + port[2]
			#if port[1].f
		
		#serial_ports()
		#list = serial_ports()
		#print list
		
		#main_board_handler()
		

		
		# create ProxyObjects
		BaseManager.register('comandAgent', comandAgent)
		BaseManager.register('ModuleIP', ModuleIP)
		manager = BaseManager()
		manager.start()
		interpreterComandAgent = manager.comandAgent()
		supervisorComandAgent = manager.comandAgent()
		pultIp = manager.ModuleIP()
		missionEditorIp = manager.ModuleIP()
		missionIp = manager.ModuleIP()
		
		# create SharedMemory
		flagStopInterpreter = Value('i', 0)
		flagStopSupervisor = Value('i', 0)
		flagDropInterpreterConnection = Value('i', 0)
		flagStopIpTranslator = Value('i',0)
		

		
		
		print "Starting threads..."
		p4 = Process(target=srv_interpreter_thread, args=(interpreterComandAgent, flagStopInterpreter, flagDropInterpreterConnection))
		p5 = Process(target=srv_supervisor_thread, args=(supervisorComandAgent, flagStopSupervisor, flagDropInterpreterConnection))
		p1 = Process(target=ip_autoconf_sender, args=(flagStopIpTranslator, missionEditorIp, pultIp, missionIp))
		p1.start()
		p4.start()
		p5.start()
		
		
		

		boardDevices.intros.init()
		print boardDevices.intros.readData()
		boardDevices.intros.initialize()
		print boardDevices.intros.readData()
		if boardDevices.intros.startScan() == 1:
			print "scan started"
			print boardDevices.intros.readData()
			time.sleep(10)
			boardDevices.intros.stopScan()
			print "scan stopped"
			print boardDevices.intros.readData()
			
		#sendToMain(ser, "$INE08,77,3,\n".encode())
		#sendToMain(ser, "$INE09,77,3,\n".encode())
		

		while True:
			#check for updates and terminate this script if update was performed for restarting by process_controller.py 
			if os.path.isfile(os.path.realpath(os.path.dirname(sys.argv[0]))+'\\upload_finished') == True:
				print "try update"
				try:
					os.remove('upload_finished')
					break
				except:
					print "Error updating"
					
			s = boardDevices.mainBoard.readData()
			if not s == "":
				print s
				
			if interpreterComandAgent.isReady() == False: #if server received data and run Execution: #$PUI12,64,435.432,<LF>
				cmd = interpreterComandAgent.getCmd() 
				interpreterComandAgent.addAnswer("$CPA01,77,OK\n")
				if cmd[0:6] == "$PUI12":  # CWheeledPlatform.gotoPos
					if boardDevices.wheeledPlatform.stop() == 1:
						print "Stop ok"
					else:
						print "Stop error"
					sub = cmd[7:]
					i = sub.find(',')
					sub = sub[i+1:]
					i = sub.find(',')
					sub = sub[:i]
					try:
						f = float(sub)
					except:
						f = 0
						print "argument " + sub + "error"
					print "received: " + cmd
					print "Wheel to: " + str(f) 
					s = boardDevices.wheeledPlatform.readData()
					if not s == "":
						print s
					boardDevices.wheeledPlatform.run(f)										
					
				elif cmd[0:6] == "$PUI23":   #intros start scan 
					print "Intros scan start cmd"
					if boardDevices.intros.isScanningRun() == 1:
						print "Error: scan is already performed"
					else:
						if boardDevices.intros.startScan() == 1:
							print "Scan started"
					pass
					
				elif cmd[0:6] == "$PUI24": #intros stop scan
					print "Intros scan stop cmd"
					if boardDevices.intros.isScanningRun() == 0:
						print "Error: scan is not performed"
					else:
						if boardDevices.intros.stopScan() == 1:
							print "Scan stopped"
					pass
					
				elif cmd[0:6] == "$PUI03": # cam set pos $PUI03,58,1,23456.342,2336.43,3243.3,<LF>
					print "Camera set pos cmd"
					args = cmd.split(',')
					# args[0] = $MEI03; args[1] = 58; args[2] = 1; args[3] = 23456.342; args[4] = 2336.43; args[5] = 3243.3; args[6] = \n
					try:
						x = float(args[3])
						y = float(args[4])
						z = float(args[5])
						if args[2] == 1:   #camera1
							boardDevices.camera1.setPos(x, y, z)
						elif args[2] == 2:   #lidar 
							boardDevices.lidar.setPos(x, 0, 0)
					except:
						pass
				elif cmd[0:6] == "$PUI06": # cam set zoom
					print "Camera set zoom cmd"
					args = cmd.split(',')
					if args[2] == '1':
						try: 
							zoom = int(args[3])
							boardDevices.camera1.setZoom(zoom)
						except:
							pass
					
				interpreterComandAgent.addAnswer("$CPA02,77,OK\n")
			
				#interpreterComandAgent.addAnswer("$CPA02,57,OK\n")
				#time.sleep(2)
				#interpreterComandAgent.addAnswer("Answer2")
				#time.sleep(1)
				interpreterComandAgent.setReady()
			else: 
				pass
			time.sleep(0.1)
			pass
		print "Updating..."
		flagStopInterpreter.value = 1
		flagStopSupervisor.value = 1
		flagStopIpTranslator.value = 1 
		p4.join()
		p5.join()

except KeyboardInterrupt:
	if __name__ == '__main__':
		print "Terminating..."
		flagStopInterpreter.value = 1
		flagStopSupervisor.value = 1
		flagStopIpTranslator.value = 1 
		p4.join()
		p5.join()
	raise
	