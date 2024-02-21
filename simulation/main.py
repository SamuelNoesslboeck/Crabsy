from euclid import Vector3
import math
import matplotlib.pyplot as plt
from time import sleep

from spyder import Spyder, SpyderFoot

if __name__ == "__main__":
    plt.ion()
    
    fig = plt.figure()
    
    spyder = Spyder(100.0, [30.0, 50.0, 100.0])
    spyder.set_anchor_length(120.0)
    spyder.set_anchor_height(-50.0)
    spyder.set_recovery_height(-20.0)
    
    spyder.draw_const([ 0, 0 ])
    spyder.draw_feet()
    
    paths = spyder.create_paths(10, 2.5, math.pi * 3 / 4)
    path_len = len(paths[0])
    n = 0
    
    while True:
        # sleep(0.1)
        # manual implementation for shifting
        spyder.update_foot_by_ang(0, paths[0][(n + path_len // 6 * 1) % path_len])
        spyder.update_foot_by_ang(1, paths[1][(n + path_len // 6 * 5) % path_len])
        spyder.update_foot_by_ang(2, paths[2][(n + path_len // 6 * 2) % path_len])
        spyder.update_foot_by_ang(3, paths[3][(n + path_len // 6 * 4) % path_len])
        spyder.update_foot_by_ang(4, paths[4][(n + path_len // 6 * 0) % path_len])
        spyder.update_foot_by_ang(5, paths[5][(n + path_len // 6 * 3) % path_len])
        
        fig.canvas.draw()
        fig.canvas.flush_events()
        
        n += 1