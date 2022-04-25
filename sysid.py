from time import sleep, time
import serial

startMarker = '<'
endMarker = '>'
dataStarted = False
dataBuf = ""
messageComplete = False

def setupSerial(baudRate, serialPortName):
    global serialPort
    serialPort = serial.Serial(port= serialPortName, baudrate = baudRate, timeout=0, write_timeout=1)
    print("Serial port " + serialPortName + " opened  Baudrate " + str(baudRate))
    waitForArduino()

#==================

def sendToArduino(stringToSend):
    # this adds the start- and end-markers before sending
    global startMarker, endMarker, serialPort
    
    stringWithMarkers = (startMarker)
    stringWithMarkers += stringToSend
    stringWithMarkers += (endMarker)

    serialPort.write(stringWithMarkers.encode('utf-8')) # encode needed for Python3

#==================

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

time_effort_tables = [
    { "time": 0.9, "effort": 200 },
    { "time": 0.4, "effort": 250 },
    { "time": 0.7, "effort": 0 },
    { "time": 0.6, "effort": 70 },
    { "time": 0.3, "effort": 150 },
    { "time": 0.2, "effort": 200 },
    { "time": 0.6, "effort": 40 },
    { "time": 1, "effort": 0 }
]
count = 0
setupSerial(115200, "/dev/ttyACM0")
if __name__ == '__main__':
    start = time()
    while count != len(time_effort_tables):
        now = time()
        step = time_effort_tables[count]
        if now - start >= step["time"]:
            effort = step["effort"]
            print(f"Sending {effort} at {now - start}")
            sendToArduino(f"g,{effort},{effort},{effort},{effort}")
            count += 1
            start = time()
        
        arduinoReply = recvLikeArduino()
        if not (arduinoReply == 'XXX'):
            print(arduinoReply)
        sleep(0.001)
        
        