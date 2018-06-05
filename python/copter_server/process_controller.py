#coding=UTF-8
import sys
import subprocess
from datetime import datetime, date
import os

#args = sys.argv[1:]
date_str = str(date.today())  
#args = ['C:/Python36-64/python', '-u', 'learn.py', model_name]
dir = os.path.realpath(os.path.dirname(sys.argv[0]))+'\\'
#args = ['python', '-u', dir+'out_test.py']

args = ['python', '-u', dir+'main.py']



def write(string):
	with open(dir+'launch_{0}.txt'.format(date_str), 'a') as log:
		out = str(datetime.now()) + ' ' + string.strip()
		log.write(out + '\n')
		log.close()
	print(string)


try:
	while True:
		p = subprocess.Popen(args, stdout=subprocess.PIPE,stderr=subprocess.STDOUT, bufsize=1, universal_newlines=True)   # , shell = True , stderr=subprocess.STDOUT  , stdout=subprocess.PIPE
		write('Launched ' + ' '.join(args))
		while not p.poll():	
			line = p.stdout.readline() #blocking call
			if not line=='':
				s = line 
				write(s)
				if (not s.find('roken') == -1) or (not s.find('pipe') == -1) or (not s.find('Pipe') == -1) or (not s.find('PIPE') == -1) or (not s.find('Errno') == -1) or (not s.find('erminat') == -1):
					#p.send_signal(subprocess.signal.CTRL_C_EVENT) #this line terminate own process
					try:
						p.wait(3)
						p.terminate()
						p.kill()
					except:
						pass
					print "restart!\r\n"
					break
					# подождать завершения с таймаутом, Popen.terminate() Popen.kill()
		try:	
			p.stdout.close()
			p.terminate()
			p.kill()
		except:
			pass
		write('Process terminated with exit code ' + str(p.returncode))
except KeyboardInterrupt:
	write('Launcher terminating...')
	#p.send_signal(subprocess.signal.CTRL_C_EVENT) #this line terminate own process
	try:
		p.wait(3)
		p.terminate()
		p.kill()
		print "kill!"
	except:
		pass
	raise