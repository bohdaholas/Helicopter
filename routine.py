import numpy as np
from typing import Any
from ahrs.filters import Mahony
from scipy.spatial.transform import Rotation as R

from gamepad import Gamepad



class Routine:

    def __init__(self) -> None:
        self.ornt_filter = Mahony()
        self.prev_gamepad_state = None
        self.cur_gamepad_state = Gamepad()
        # starting quaterinon, the system will need some time to stabilise
        self.orientation = np.array([1., 0., 0., 0.])
        self.prev_orientation

    def __call__(self, *args: Any, **kwds: Any) -> Any:

        while True:
            self.cur_gamepad_state.read_data()

        



while True:

    cur_gamepad_state.read_data()
    gyr = np.array()
    acc = np.array()
    q = ornt_filter.updateIMU(q, gyr, acc)
    
    
    prev_gamepad_state = cur_gamepad_state





r = R.from_quat(q)
r.as_euler('xyz', degrees=True) # returns np.array




