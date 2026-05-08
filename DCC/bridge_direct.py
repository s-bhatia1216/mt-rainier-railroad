import serial
import time

arduino = serial.Serial('COM4', 115200, timeout=1)
dcc = serial.Serial('COM3', 115200, timeout=1)

time.sleep(1)

print("Transparent bridge running...")

while True:
    if arduino.in_waiting:
        data = arduino.readline()   # raw bytes
        print("Forwarding:", data)

        dcc.write(data)             # send exactly as-is
        dcc.flush()