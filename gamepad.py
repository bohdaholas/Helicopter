from inputs import get_gamepad
from dataclasses import dataclass


@dataclass
class Gamepad:
    triangle: int = 0
    circle: int = 0
    cross: int = 0
    square: int = 0
    LBumper: int = 0
    RBumper: int = 0
    LTrigger: int = 0
    RTrigger: int = 0
    select: int = 0
    start: int = 0
    LStickBtn: int = 0
    LStickX: int = 128
    LStickY: int = 128
    RStickBtn: int = 0
    RStickX: int = 128
    RStickY: int = 128
    DPadX: int = 0
    DPadY: int = 0

    def read_data(self, verbose=False):
        for event in get_gamepad():
            if event.ev_type == "Key":  # category of binary respond values
                if event.code == "BTN_BASE5":
                    self.LStickBtn = event.state
                elif event.code == "BTN_BASE6":
                    self.RStickBtn = event.state
                elif event.code == "BTN_TRIGGER":
                    self.triangle = event.state
                elif event.code == "BTN_THUMB":
                    self.circle = event.state
                elif event.code == "BTN_THUMB2":
                    self.cross = event.state
                elif event.code == "BTN_TOP":
                    self.square = event.state
                elif event.code == "BTN_BASE":
                    self.LBumper = event.state
                elif event.code == "BTN_BASE2":
                    self.RBumper = event.state
                elif event.code == "BTN_TOP2":
                    self.LTrigger = event.state
                elif event.code == "BTN_PINKIE":
                    self.RTrigger = event.state
                elif event.code == "BTN_BASE3":
                    self.select = event.state
                elif event.code == "BTN_BASE4":
                    self.start = event.state
            elif event.ev_type == "Absolute":  # category of analog respond values
                if event.code == "ABS_Y":
                    self.LStickY = event.state
                elif event.code == "ABS_X":
                    self.LStickX = event.state
                elif event.code == "ABS_Z":
                    self.RStickY = event.state
                elif event.code == "ABS_RZ":
                    self.RStickX = event.state
                elif event.code == "ABS_HAT0Y":
                    self.DPadX = event.state
                elif event.code == "ABS_HAT0X":
                    self.DPadY = event.state
        if verbose:
            print(self)
        return self


if __name__ == '__main__':
    gamepad = Gamepad()
    gamepad.read_data(verbose=True)
