import serial
import time

ser = serial.Serial('/dev/ttyS0', baudrate=9600, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,bytesize=serial.EIGHTBITS)

ser.isOpen()

print("Connected to: " + ser.portstr)
while True:
    input = raw_input("Enter angle for servo: ")
    
    if int(float(input)) >= 0:
        sign = 1
    else:
        sign = 0
    
    modSum = abs(int(float(input))) % 256
    divSum = abs(int(float(input))) / 256

    checkSum = ((sign + divSum + modSum)%256)
    ser.write(str(chr(0xFF) + chr(0xFF) + chr(sign) + chr(divSum) + chr(modSum) + chr(checkSum)))