from inputs import get_gamepad
from dataclasses import dataclass


@dataclass
class Gamepad:
    '''
    Convenient function to store the current state (ouput from the gamepad)
    '''

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
        '''
        Gets the data from the gamepad and transfroms it into the
        state
        '''

        # maps user's action via gamepad to overall state
        abs_resp_to_state = {"ABS_Y": "LStickY", "ABS_X": "LStickX",
                             "ABS_Z": "RStickY", "ABS_RZ": "RStickX",
                             "ABS_HAT0Y": "DPadX", "ABS_HAT0Y": "DPadY"}

        bin_resp_to_state = {"BTN_BASE5": "LStickBtn", "BTN_BASE6": "RStickBtn",
                             "BTN_TRIGGER": "triangle", "BTN_THUMB": "circle",
                             "BTN_THUMB": "cross", "BTN_TOP": "square",
                             "BTN_BASE": "LBumper", "BTN_BASE": "RBumper",
                             "BTN_TOP2": "LTrigger", "BTN_PINKIE": "RTrigger",
                             "BTN_BASE3": "select", "BTN_BASE3": "start"}

        for event in get_gamepad():
            if event.ev_type == "Key":  # category of binary respond values
                setattr(self, bin_resp_to_state[event.code], event.state)
            elif event.ev_type == "Absolute":  # category of analog respond values
                setattr(self, abs_resp_to_state[event.code], event.state)

        if verbose:
            print(self)

        return self


if __name__ == '__main__':
    gamepad = Gamepad()
    gamepad.read_data(verbose=True)

