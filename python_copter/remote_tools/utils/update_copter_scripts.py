import socket
import time
from ftplib import FTP
import ftplib
from remote_connection_config import IP_COPTER, PORT_SUPERVISOR, PORT_FTP, PASS_FTP, USER_FTP, dir_src, files_to_upload




print "connecting to " + IP_COPTER + ":" + str(PORT_FTP)+ "..."
ftp = FTP()
ftp.connect(IP_COPTER, PORT_FTP)
print(ftp.login(USER_FTP, PASS_FTP))
ftp.cwd('\\')
data = ftp.retrlines('LIST')
print(data)
try: 
	ftp.delete('upload_finished')
	print "deleted flag file \'upload_finished\'"
except:
	pass
for filename in files_to_upload:
	try: 
		ftp.delete(filename+'.bak')
		print "deleted " + filename + ".bak"
	except:
		pass		
	try:
		ftp.rename(filename, filename+'.bak')
		print "renamed " + filename + " to " + filename + ".bak"
	except: 
		pass
	try:
		with open(dir_src+filename, 'rb') as fobj:
			ftp.storbinary('STOR ' + filename + '.tmp', fobj, 1024)
		print "uploaded " + filename + '.tmp'
		ftp.rename(filename + '.tmp', filename)
		print "renamed " + filename + '.tmp' + " to " + filename
	except:
		pass
try:
	with open(dir_src+files_to_upload[0], 'rb') as fobj:
		ftp.storbinary('STOR ' + 'upload_finished', fobj, 1024)
	print "writed upload_finished flag"
except:
	pass

ftp.close()
print "finish"
