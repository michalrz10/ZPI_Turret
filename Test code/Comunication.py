import serial
import struct
import time


class Communication:
    def __init__(self, port):
        self.ser = serial.Serial(port, 9600)
        time.sleep(2)
        print(self.ser.isOpen())

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.ser.close()

    def sendPosition(self, x, y):
        self.ser.write(b'\x00' + struct.pack('f', x) + struct.pack('f', y) + b'\x00')

    def sendShot(self):
        self.ser.write(b'\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    def canShoot(self):
        self.ser.write(b'\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        print(int.from_bytes(self.ser.read(1), 'big'))

    def sendOffset(self, x, y):
        self.ser.write(b'\x03' + struct.pack('f', x) + struct.pack('f', y) + b'\x00')

    def sendDelay(self, delay):
        self.ser.write(b'\x04' + struct.pack('h', delay) + b'\x00\x00\x00\x00\x00\x00\x00')

    def sendStep(self, step):
        self.ser.write(b'\x05' + struct.pack('B', step) + b'\x00\x00\x00\x00\x00\x00\x00\x00')


c = Communication('COM5')
print(c.ser)
c.sendOffset(45, 0)
time.sleep(5)
c.sendDelay(2000)
c.sendPosition(-90, 0)
time.sleep(5)
c.sendDelay(500)
c.sendStep(8)
c.sendPosition(0, 0)
