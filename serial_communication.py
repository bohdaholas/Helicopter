import serial
from gamepad import Gamepad

pi_usb_port = '/dev/ttyUSB0'  # the port where arduino uno is connected to raspberry pi


if __name__ == '__main__':
    ser = serial.Serial(pi_usb_port, 9600, timeout=.02)
    ser.flush()
    gamepad = Gamepad()
    while True:
        # transmit gamepad data from raspberry pi to arduino uno
        gamepad = gamepad.read_data()
        gamepad_data = bytearray([gamepad.LStickX, gamepad.LStickY, gamepad.RStickX, gamepad.RStickY])
        ser.write(gamepad_data)
        # arduino response
        # line = ser.readline().decode('utf-8').rstrip()
        # if line:
        #     print(line)
