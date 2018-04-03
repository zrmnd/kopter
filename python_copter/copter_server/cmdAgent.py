#coding=UTF-8
import multiprocessing 

# class for exchange commands and answers for server and executor 
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