import serial
from gamepad import Gamepad

arduino_usb_port = '/dev/ttyUSB1'  # port of the raspberry pi


if __name__ == '__main__':
    ser = serial.Serial(arduino_usb_port, 9600, timeout=.02)
    ser.flush()
    gamepad = Gamepad()
    while True:
        # transmit gamepad data from raspberry pi to arduino uno
        gamepad = gamepad.read_data()
        # ser.write(f"{gamepad.LStickX} {gamepad.LStickY} {gamepad.RStickX} {gamepad.RStickY}\n".encode())
        gamepad_data = bytearray([gamepad.LStickX, gamepad.LStickY, gamepad.RStickX, gamepad.RStickY])
        ser.write(gamepad_data)
        # arduino response
        line = ser.readline().decode('utf-8').rstrip()
        if line:
            print(line)
