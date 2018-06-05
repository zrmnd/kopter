import socket
import time
import threading
from enum import Enum

class ModuleIP(object):
    def __init__(self):
        self._mutex = threading.RLock()
        self._ip = ""
        self._ipTime = 0
        
    def incTime(self):
        with self._mutex:
            self._ipTime = self._ipTime + 1
            
    def setIp(self, ip):
        with self._mutex:
            self._ipTime = 0
            self._ip = ip
            
    def get(self):
        with self._mutex:
            return (self._ip, self._ipTime)
pass



# класс для передачи в потоки интерпретации текста команд и получения ответов
class comandAgent(object):
    def __init__(self):
        self._mutex = threading.RLock()
        self._comand = ""
        self._answer = []
        self._flagReady = 1
        self._flagStop = 0
        
    def runCmd(self, cmd):
        with self._mutex:
            if self._flagReady == 1 and self._flagStop == 0:
                self._comand = cmd
                self._flagReady = 0
                self._answer.clear()
                return True
            else:
                return False
            
    def isReady(self):
        with self._mutex:
            if self._flagReady == 1:
                return True
            else: 
                return False

    def hasData(self):
        with self._mutex:
            if self._answer.len() > 0:
                return True
            else:
                return False

    def fetchData(self):
        with self._mutex:
            if self._answer.len() > 0:
                return self._answer.pop(0)
            else:
                return ""

    def stop(self):
        with self._mutex:
            self._flagStop = 1

    def setReady(self):
        with self._mutex:
            self._flagReady = 1
            self._flagStop = 0


    def addAnswer(self, answer):
        with self._mutex:
            self._answer.append(answer)

    def getCmd(self):
        with self._mutex:
            return self._comand
pass

class CopterState(Enum):
    IDLE = 0
    MISSION_RUNNING = 1
    MANUAL_MODE = 2
pass









mainBoardAgent = comandAgent()
lidarControllerAgent = comandAgent()
cameraControllerAgent = comandAgent()
introsControllerAgent = comandAgent()
flyingPlatformAgent = comandAgent()
cableWalkerAgent = comandAgent()

commonAgent = cableWalkerAgent

state = CopterState.IDLE



def StopAll():
    mainBoardAgent.stop()
    lidarControllerAgent.stop()
    cameraControllerAgent.stop()
    introsControllerAgent.stop()
    flyingPlatformAgent.stop()
    cableWalkerAgent.stop()

    state = CopterState.IDLE

    #flagEmergencyStop = 1
    #queue.clearQueue()
    # send stop to all peripherals
pass

# сюда вынесена логика опроса активного объекта
# commonAgent - ссылка на активный объект, который выполняет команду
def handle_cmd():
    while flagRunning == 1:
        if commonAgent.isStopped == True:


        while commonAgent.isReady == True:
            pass

        if commonAgent.isReady == False: # из другого потока вызвали runCmd
            flagUnknownCmd = False
            cmd = comandAgent.getCmd()
            if cmd[0] == '$' and cmd[3] == 'I':
                if cmd[3:5] == "I0":
                    i = cmd.find(',', 0)
                    i = cmd.find(',', i+1)
                    i2 = cmd.find(',',i+1)
                    if cmd[i,i2+1] == '1':
                        currentAgent = cameraControllerAgent
                    elif cmd[i,i2+1] == '2':
                        commonAgent = lidarControllerAgent
            elif cmd[3:5] == "I1":
                commonAgent = mainBoardAgent
            elif cmd[3:5] == "I2":
                commonAgent = introsControllerAgent
            elif cmd[3:5] == "I3":
                commonAgent = flyingPlatformAgent
            elif cmd[3:5] == "I4":
                commonAgent = cableWalkerAgent
            else:
                flagUnknownCmd = True
            if flagUnknownCmd == False:
                commonAgent.runCmd(cmd)
pass



ipAutoconfPort = 4351
interpreterPort = 3166
supervisorPort = 3167

pultIp = ModuleIP()
missionEditorIp = ModuleIP()
missionIp = ModuleIP()


flagRunning = 1
flagKeepConnection = 0 # флаг, устанавливающийся при подключении клиента к интерпретатору. При отключении сбрасывается (со стороны супервайзера - тоже)
#flagEmergencyStop = 0
#flagReadyForComands = 1





def ip_autoconf_sender():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    while flagRunning == 1:
        sock.sendto('$CPA00,COPTER V1.0,\n',('192.168.8.255', ipAutoconfPort))
        missionEditorIp.incTime()
        pultIp.incTime()
        missionIp.incTime()
        time.sleep(1)
    pass
    sock.close()


def ip_autoconf_receiver():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    sock.bind(('0.0.0.0', ipAutoconfPort))
    while flagRunning == 1:
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
   


def srv_interpreter(): #receive comands over TCP-IP and put it into comands queue
    while flagRunning == 1:

        pass


def srv_supervisor(): #receive control comands over TCP-IP and execute them
    while flagRunning == 1:
        
        pass


def hardware_handler(): #handle 
    while flagRunning == 1:
        
        pass



def main_board_handler(): #control power supply, wheeled platform, RF-module 
    while flagRunning == 1:
        
        pass



def lidar_controller_handler(): #control power and hanging 
    while flagRunning == 1:
        
        pass

def supercam_controller_handler(): #control zoom and moving 
    while flagRunning == 1:
        
        pass

def intros_controller_handler(): #control zoom and moving 
    while flagRunning == 1:
        
        pass




try:
    th1 = threading.Thread(name='ip_autoconf_sender_th', target=ip_autoconf_sender, args=())
    th2 = threading.Thread(name='ip_autoconf_receiver_th', target=ip_autoconf_receiver, args=())
    th3 = threading.Thread(name='comands_executor', target=comands_executor, args=()) 
    th4 = threading.Thread(name='srv_interpreter', target=srv_interpreter, args=())
    th5 = threading.Thread(name='srv_supervisor', target=srv_supervisor, args=())
    th6 = threading.Thread(name='main_board_handler', target=main_board_handler, args=())
    th7 = threading.Thread(name='lidar_controller_handler', target=lidar_controller_handler, args=())
    th8 = threading.Thread(name='supercam_controller_handler', target=supercam_controller_handler, args=())
    th9 = threading.Thread(name='srv_supervisor', target=srv_supervisor, args=())
    th1.start()
    th2.start()
    th3.start()
    th4.start()
    th5.start()
    th6.start()

    oldTime = 0
    while True:
        
        
        ip, ipTime = missionIp.get()
        if not oldTime == ipTime:
            print ip, ipTime
            oldTime = ipTime

        pass

except KeyboardInterrupt:
    print "Terminating..."
    flagRunning = 0
    th1.join()
    th2.join()
   



