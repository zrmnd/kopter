# coding=UTF-8
import socket
import multiprocessing 
#import threading
import time
import errno
from multiprocessing import Process, Manager, Value
from multiprocessing.managers import BaseManager
import os
from ctypes import c_bool
import serial
import sys
import glob
from serial.tools import list_ports



INTERPRETER_PORT = 4354
SUPERVISOR_PORT = 4358





# класс длЯ передачи в потоки интерпретации текста команд и получениЯ ответов
class comandAgent(object):
    def __init__(self):
        self._mutex = multiprocessing.Lock()
        self._comand = ""
        self._answer = []
        self._flagReady = 1
        self._flagStop = 0
        
    def runCmd(self, cmd):
		self._mutex.acquire()
		if self._flagReady == 1 and self._flagStop == 0:
			self._comand = cmd
			self._flagReady = 0
			del self._answer[:]
			res = True
			#print "Cmd to executing: " + cmd
		else:
			res = False
		self._mutex.release()
		return res
            
    def isReady(self):
		if self._flagReady == 1:
			return True
		else: 
			return False

    def hasData(self):
		if len(self._answer) > 0:
			return True
		else:
			return False

    def fetchData(self):
		self._mutex.acquire()
		if len(self._answer) > 0:
			res = self._answer.pop(0)
		else:
			res = ""
		self._mutex.release()
		return res

    def stop(self):
		self._mutex.acquire()
		self._flagStop = 1
		self._mutex.release()

    def setReady(self):
		self._mutex.acquire()
		self._flagReady = 1
		self._flagStop = 0
		self._mutex.release()

    def addAnswer(self, answer):
		self._mutex.acquire()
		self._answer.append(answer)
		self._mutex.release()
    def getCmd(self):
        return self._comand
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
 



def srv_interpreter_thread(cmdAgent, flagStopThread, flagDropConnection): #receive comands over TCP-IP, handle them and send answers
	print "srv_interpreter_thread start\n"
	srv_interpreter = socket.socket()	
	srv_interpreter.bind( ("", INTERPRETER_PORT) )
	srv_interpreter.listen(3)
	srv_interpreter.setblocking(0)
	cmdAgent.setReady()
	
	newclient = ""
	client = ""

	while flagStopThread.value == 0:
		# пробуем получить клиента:
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
			client.setblocking(0) # снимаем блокировку и тут тоже
		except:
			pass
			
		
				
		# попытка получить данные
		data = ""
		try:
			if not client == "":
				tmp = client.recv(1024)
				while tmp and (flagStopThread.value == 0):
					data = data + tmp
					tmp = client.recv(1024)					
		except socket.error, e: # данных нет
			if e.args[0] == errno.EAGAIN or e.args[0] == errno.EWOULDBLOCK:
				#print "No data\n" 
				pass
			else: 
				flagDropConnection.value = 1						
			time.sleep(0.1)				
		if not data == "": # данные принЯты
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
	srv_supervisor.bind( ("", SUPERVISOR_PORT) )
	srv_supervisor.listen(10)
	srv_supervisor.setblocking(0)
	while flagStopThread.value == 0:      
		# получает клиентов:
		try:
			client, addr = srv_supervisor.accept()
			# клиенты есть
			print "Accepted by supervisor...\n"
			#client.setblocking(0) # снимаем блокировку и тут тоже
			client.settimeout(1)
			#parse(client, addr)
			try:
				data = ""
				tmp = ""
				tmp = client.recv(1024)
				while tmp:
					data = data + tmp
					tmp = client.recv(1024)
					
			except socket.error, e: # данных нет или соединение дропнулось
				if e.args[0] == errno.EAGAIN or e.args[0] == errno.EWOULDBLOCK:
						#print "No data\n" 
						pass		
				time.sleep(0.1) 
			if not data == "": # если data не пуст, запускаем обработку команды супервайзера
				print "Data received by supervisor..."
				if IsStopAllCmd(data):
					flagDropConnection.value = 1
										
				# проверяем и выполняем команды
				print("Supervisor received cmd: " + data)
				client.send("Your data: " + data)
			client.close()
				
		except socket.error: # клиентов нет
			pass 
			
		time.sleep(0.1)
	try:
		srv_supervisor.stop()
	except:
		pass
	print "srv_supervisor_thread terminated"


	
	
	

	
	

	
def main_board_handler():
	ports = []
	try:
		tmp = serial.Serial(port = 'COM2', baudrate=2400, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=0)
		print("connected to: " + ser.portstr)
		while ser.inWaiting():
			 data = ser.read()
	except serial.serialutil.SerialException:
		pass
	
	
def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob ('/dev/tty[A-Za-z]*') # http://qaru.site/questions/114632/listing-available-com-ports-with-python
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
			s = serial.Serial(port)
			result.append(port)
			print port 
			s.close()
            
        except (OSError, serial.SerialException):
            pass
    return result	
	
	



try:

	if __name__ == '__main__':
	
		print "Main run...\n"
		
		ports_avaiable = list(list_ports.comports())
		
		for port in ports_avaiable:
			print port[0] + " - " + port[1] + " - " + port[2]
		
		#serial_ports()
		#list = serial_ports()
		#print list
		
		#main_board_handler()
		
		# делаем ProxyObjects
		BaseManager.register('comandAgent', comandAgent)
		manager = BaseManager()
		manager.start()
		interpreterComandAgent = manager.comandAgent()
		supervisorComandAgent = manager.comandAgent()
		
		# делаем SharedMemory
		flagStopInterpreter = Value('i', 0)
		flagStopSupervisor = Value('i', 0)
		flagDropInterpreterConnection = Value('i', 0)
		
		
		
		
		print "Starting threads..."
		p4 = Process(target=srv_interpreter_thread, args=(interpreterComandAgent, flagStopInterpreter, flagDropInterpreterConnection))
		p5 = Process(target=srv_supervisor_thread, args=(supervisorComandAgent, flagStopSupervisor, flagDropInterpreterConnection))
		p4.start()
		p5.start()
		


		while True:
			# проверка на то, что объекту послали команду на выполнение 
			if interpreterComandAgent.isReady() == False:
				interpreterComandAgent.addAnswer("$CPA02,57,\n")
				#time.sleep(2)
				#interpreterComandAgent.addAnswer("Answer2")
				time.sleep(1)
				interpreterComandAgent.setReady()
			else: 
				pass
			time.sleep(0.1)
			pass

except KeyboardInterrupt:
	if __name__ == '__main__':
		print "Terminating..."
		flagStopInterpreter = True
		flagStopSupervisor = True
		
		p4.join()
		p5.join()
	raise
	