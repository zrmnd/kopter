import socket
import time
from ftplib import FTP
import ftplib
from remote_connection_config import IP_COPTER, PORT_SUPERVISOR, PORT_FTP, PASS_FTP, USER_FTP, dir_src, files_to_upload
import os
import sys
from datetime import datetime, date


dir = os.path.realpath(os.path.dirname(sys.argv[0]))+'\\'

print "connecting to " + IP_COPTER + ":" + str(PORT_FTP)+ "..."
ftp = FTP()
ftp.connect(IP_COPTER, PORT_FTP)
print(ftp.login(USER_FTP, PASS_FTP))
ftp.cwd('\\')
data = ftp.retrlines('LIST')
print(data)
try: 
	os.remove(dir+'today_log.txt')
	print "old log removed"
except:
	pass
try:
	date_str = str(date.today()) 
	fn = 'launch_{0}.txt'.format(date_str)
	print fn
	with open(dir+'today_log.txt', 'wb') as f:
		print ftp.retrbinary('RETR ' + fn, f.write)	
	print "new log downloaded"
except:
	pass
ftp.close()
print "finish"
