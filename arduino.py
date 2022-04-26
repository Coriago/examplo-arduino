# from inputs import devices
from inputs import get_gamepad
import math
import threading
from time import sleep
import serial

startMarker = '<'
endMarker = '>'
dataStarted = False
dataBuf = ""
messageComplete = False
semaphore = 0
semaphore_cap = 10

def setupSerial(baudRate, serialPortName):
    global serialPort
    serialPort = serial.Serial(port= serialPortName, baudrate = baudRate, timeout=0, write_timeout=1)
    print("Serial port " + serialPortName + " opened  Baudrate " + str(baudRate))
    waitForArduino()

#========================

def sendToArduino(stringToSend):
    # this adds the start- and end-markers before sending
    global startMarker, endMarker, serialPort
    
    stringWithMarkers = (startMarker)
    stringWithMarkers += stringToSend
    stringWithMarkers += (endMarker)

    serialPort.write(stringWithMarkers.encode('utf-8')) # encode needed for Python3


#================== 301.625mm one rotation
#  

def recvLikeArduino():
    global startMarker, endMarker, serialPort, dataStarted, dataBuf, messageComplete
    while serialPort.inWaiting() > 0 and messageComplete == False:
        try:
            x = serialPort.read().decode("utf-8") # decode needed for Python3
        except:
            x = ""
        if dataStarted == True:
            if x != endMarker:
                dataBuf = dataBuf + x
            else:
                dataStarted = False
                messageComplete = True
        elif x == startMarker:
            dataBuf = ''
            dataStarted = True

    if (messageComplete == True):
        messageComplete = False
        return dataBuf
    else:
        return "XXX"

#==================

def waitForArduino():
    # wait until the Arduino sends 'Arduino is ready' - allows time for Arduino reset
    # it also ensures that any bytes left over from a previous message are discarded
    print("Waiting for Arduino to reset")
    msg = ""
    while msg.find("Arduino is ready") == -1:
        msg = recvLikeArduino()
        if not (msg == 'XXX'):
            print(msg)
    # serialPort.write("<c>".encode('utf-8'))

class XboxController(object):
    MAX_TRIG_VAL = math.pow(2, 8)
    MAX_JOY_VAL = math.pow(2, 15)

    def __init__(self):

        self.LeftJoystickY = 0
        self.LeftJoystickX = 0
        self.RightJoystickY = 0
        self.RightJoystickX = 0
        self.LeftTrigger = 0
        self.RightTrigger = 0
        self.LeftBumper = 0
        self.RightBumper = 0
        self.A = 0
        self.X = 0
        self.Y = 0
        self.B = 0
        self.LeftThumb = 0
        self.RightThumb = 0
        self.Back = 0
        self.Start = 0
        self.LeftDPad = 0
        self.RightDPad = 0
        self.UpDPad = 0
        self.DownDPad = 0

        self._monitor_thread = threading.Thread(target=self._monitor_controller, args=())
        self._monitor_thread.daemon = True
        self._monitor_thread.start()


    def read(self): 
        x = self.LeftJoystickX
        y = self.LeftJoystickY
        a = self.A
        b = self.X # b=1, x=2
        rb = self.RightBumper
        return [x, y, a, b, rb]


    def _monitor_controller(self):
        while True:
            events = get_gamepad()
            for event in events:
                if event.code == 'ABS_Y':
                    self.LeftJoystickY = event.state / XboxController.MAX_JOY_VAL # normalize between -1 and 1
                elif event.code == 'ABS_X':
                    self.LeftJoystickX = event.state / XboxController.MAX_JOY_VAL # normalize between -1 and 1
                elif event.code == 'ABS_RY':
                    self.RightJoystickY = event.state / XboxController.MAX_JOY_VAL # normalize between -1 and 1
                elif event.code == 'ABS_RX':
                    self.RightJoystickX = event.state / XboxController.MAX_JOY_VAL # normalize between -1 and 1
                elif event.code == 'ABS_Z':
                    self.LeftTrigger = event.state / XboxController.MAX_TRIG_VAL # normalize between 0 and 1
                elif event.code == 'ABS_RZ':
                    self.RightTrigger = event.state / XboxController.MAX_TRIG_VAL # normalize between 0 and 1
                elif event.code == 'BTN_TL':
                    self.LeftBumper = event.state
                elif event.code == 'BTN_TR':
                    self.RightBumper = event.state
                elif event.code == 'BTN_SOUTH':
                    self.A = event.state
                elif event.code == 'BTN_NORTH':
                    self.X = event.state
                elif event.code == 'BTN_WEST':
                    self.Y = event.state
                elif event.code == 'BTN_EAST':
                    self.B = event.state
                elif event.code == 'BTN_THUMBL':
                    self.LeftThumb = event.state
                elif event.code == 'BTN_THUMBR':
                    self.RightThumb = event.state
                elif event.code == 'BTN_SELECT':
                    self.Back = event.state
                elif event.code == 'BTN_START':
                    self.Start = event.state
                elif event.code == 'BTN_TRIGGER_HAPPY1':
                    self.LeftDPad = event.state
                elif event.code == 'BTN_TRIGGER_HAPPY2':
                    self.RightDPad = event.state
                elif event.code == 'BTN_TRIGGER_HAPPY3':
                    self.UpDPad = event.state
                elif event.code == 'BTN_TRIGGER_HAPPY4':
                    self.DownDPad = event.state


lastcmd = ""
msgcmd = "e"
threshold = 0.1
setupSerial(115200, "/dev/ttyACM0")
if __name__ == '__main__':
    joy = XboxController()
    while True:

        leftY = round(joy.LeftJoystickY, 3)
        leftX = round(joy.LeftJoystickX, 3)
        rightX = round(joy.RightJoystickX, 3)

        # Deadzone
        if abs(leftY) < threshold:
            leftY = 0
        if abs(leftX) < threshold:
            leftX = 0
        if abs(rightX) < threshold:
            rightX = 0

        den = max(abs(leftY) + abs(leftX) + abs(rightX), 1)
        # m1 = int(((leftY + leftX + rightX) / den) * -255)
        # m2 = int(((leftY - leftX + rightX) / den) * -255)
        # m3 = int(((leftY + leftX - rightX) / den) * -255)
        # m4 = int(((leftY - leftX - rightX) / den) * -255)

        m1 = round(float(((leftY + leftX + rightX) / den) * -4.6), 2)
        m2 = round(float(((leftY - leftX + rightX) / den) * -4.6), 2)
        m3 = round(float(((leftY + leftX - rightX) / den) * -4.6), 2)
        m4 = round(float(((leftY - leftX - rightX) / den) * -4.6), 2)

        # if leftY == 0 and leftX == 0 and rightX == 0:
        #     msgcmd = "e"
        # else:
        
        msgcmd = f"f,{m1},{m2},{m3},{m4}"

        # Tank Diff Drive
        # leftSide = int(round(joy.LeftJoystickY, 2) * -255.0)
        # rightSide = int(round(joy.RightJoystickY, 2) * -255.0)

        # if abs(leftSide) < threshold:
        #     leftSide = 0
        # if abs(rightSide) < threshold:
        #     rightSide = 0

        # if leftSide == 0 and rightSide == 0:
        #     msgcmd = "e"
        # else:
        #     msgcmd = f"g,{rightSide},{rightSide},{leftSide},{leftSide}"
        
        if semaphore < semaphore_cap and msgcmd != lastcmd:
                print(msgcmd)
                sendToArduino(msgcmd)
                semaphore += 1
                lastcmd = msgcmd
        arduinoReply = recvLikeArduino()
        if not (arduinoReply == 'XXX'):
            if not (arduinoReply == 'c'):
                print(arduinoReply)
            if arduinoReply == "c":
                semaphore -= 1
        sleep(0.01)