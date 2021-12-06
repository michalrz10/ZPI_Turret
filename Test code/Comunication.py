import serial
import struct
import time
import matplotlib.pyplot as plt
import math


class Communication:
    def __init__(self, port):
        self.ser = serial.Serial(port, 115200)
        time.sleep(2)
        print(self.ser.isOpen())

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.ser.close()

    def sendPosition(self, x, y):
        self.ser.write(b'\x00' + struct.pack('f', x) + struct.pack('f', y) + b'\x00\x00\x00\x00\x00\x00\x00')

    def sendShot(self):
        self.ser.write(b'\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    def canShoot(self):
        self.ser.write(b'\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        return self.ser.read(1) == b'\xff'

    def sendOffset(self, x, y):
        self.ser.write(b'\x03' + struct.pack('f', x) + struct.pack('f', y) + b'\x00\x00\x00\x00\x00\x00\x00')

    def sendDelay(self, delay):
        self.ser.write(b'\x04' + struct.pack('h', delay) + b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    def sendStep(self, step):
        self.ser.write(b'\x05' + struct.pack('B', step) + b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    def sendConfig(self):
        self.ser.write(b'\x06\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        return self.ser.read(1) == b'\xff'

if __name__ == "__main__":
    port = input('Podaj port serial: ')
    c = Communication(port)
    while True:
        inp = input('-> ').split(' ')
        if inp[0] == 'P' or inp[0] == 'p':
            if len(inp)>=3:
                x=float(inp[1])
                y=float(inp[2])
                c.sendPosition(x, y)
        elif inp[0] == 'S' or inp[0] == 's':
            print('s')
            c.sendShot()
        elif inp[0] == 'O' or inp[0] == 'o':
            if len(inp) >= 3:
                x = float(inp[1])
                y = float(inp[2])
                c.sendOffset(x, y)
        elif inp[0] == 'D' or inp[0] == 'd':
            if len(inp)>=2:
                x = int(inp[1])
                c.sendDelay(x)
        elif inp[0] == 'St' or inp[0] == 'st':
            if len(inp) >= 2:
                x = int(inp[1])
                c.sendStep(x)
        elif inp[0] == 'C' or inp[0] == 'c':
            c.sendConfig()