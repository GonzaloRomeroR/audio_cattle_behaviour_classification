import serial
import random
import time

ser = serial.Serial()
ser.baudrate = 115200
ser.port = "COM6"
ser.timeout = 10

print("Opening serial communication")
ser.open()

print("Writing bytes")
values = bytearray([4, 9, 62, 144, 56, 30, 147, 3, 210, 89, 111, 78, 184, 151, 17, 129])
ser.write(values)

total = 0

print("Reading bytes")
while True:
    print(ser.read(1))


ser.close()
