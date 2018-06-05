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
from serial.tools import list_ports


try:

	if __name__ == '__main__':
	
		print "Main run...\n"
		
		ports_avaiable = list(list_ports.comports())
		
		for port in ports_avaiable:
			print port[0] + " - " + port[1] + " - " + port[2]
			#if port[1].f
		


		

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
		
		boardDevices.camera1.setZoom(10)
		boardDevices.camera1.setPos(100, 100, 100)
		

		while True:
			
					
			s = boardDevices.mainBoard.readData()
			if not s == "":
				print s
				
			

except KeyboardInterrupt:
	if __name__ == '__main__':
		print "Terminating..."

	raise
	