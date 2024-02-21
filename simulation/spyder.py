from euclid import Vector3, Matrix3, Matrix4
import math
import matplotlib.pyplot as plt
    

class SpyderFoot():
    def __init__(self, lengths : list[float]) -> None:
        # Constants
        self.lengths = lengths
        
        # Angles
        self.phis = [ 0.0, 0.0, 0.0 ]
        
        # Position
        self.vecs = self.neutral_vectors()
        self.pos = self.endpos()
        
        # Anchor
        self.anchor_length = 0.0    # X
        self.anchor_height = 0.0    # Z
        self.recovery_height = 0.0   # Z
        
        # Helper variables
        self.radius = 0.0
        self.radius23 = 0.0
        self.alpha = 0.0
        self.gamma = 0.0
        self.eta = 0.0
    
        
    def neutral_vectors(self) -> list[Vector3]:
        return [
            Vector3(self.lengths[0]),
            Vector3(self.lengths[1]),
            Vector3(self.lengths[2])
        ]
    
    
    def endpos(self) -> Vector3:
        return self.vecs[0] + self.vecs[1] + self.vecs[2]
    
    
    # Anchors
    def set_anchor_length(self, length : float):
        self.anchor_length = length
        return self
    

    def set_anchor_height(self, height : float):
        self.anchor_height = height
        return self
    
    
    def set_recovery_height(self, height : float):
        self.recovery_height = height
        return self
        
        
    def anchor_point(self) -> Vector3:
        return Vector3(self.anchor_length, 0.0, self.anchor_height)
    

    def recalc_from_ang(self, phis : list[float]) -> Vector3:
        self.phis = phis
        
        nvecs = self.neutral_vectors()
        self.vecs = [
            Matrix4.new_rotatez(self.phis[0]) * nvecs[0],
            Matrix4.new_rotatez(self.phis[0]) * Matrix4.new_rotatey(-(self.phis[1])) * nvecs[1],
            Matrix4.new_rotatez(self.phis[0]) * Matrix4.new_rotatey(-(self.phis[1] + self.phis[2])) * nvecs[2]
        ]
        self.pos = self.endpos()
        return self.pos
    
    
    def recalc_from_pos(self, pos : Vector3) -> list[float]:
        # Useful unpacking
        x, y, z = pos
        l1, l2, l3 = self.lengths
        
        # Extract first angle from top view
        phi1 = math.atan2(y, x)
        
        radius = math.sqrt(x ** 2 + y ** 2)
        radius23 = (pos - Matrix4.new_rotatez(phi1) * self.neutral_vectors()[0]).magnitude()
        
        # Check if angles are valid
        if (l2 + l3) < radius23:
            raise ValueError(f"The given radius {radius} is out of range!")
        
        # Update pos
        self.pos = pos
        self.radius = radius

        
        # Calculate helper gamma angle from triangle with reverse law of cosines
        a = l3
        b = l2
        c = radius23
        
        self.gamma = math.acos((a**2 + b**2 - c**2) / 2 / a / b)
        self.alpha = math.asin(a * math.sin(self.gamma) / c)
        self.eta = math.asin(z / c)
        
        phi3 = self.gamma - math.pi
        phi2 = self.alpha + self.eta
        
        phis = [ phi1, phi2, phi3 ]
        self.phis = phis
        return phis
    
    
    def create_path(self, num_iter : int, iter_dist : float, angle : float) -> list[list[float]]:
        iter_vec = Vector3(math.cos(angle), math.sin(angle), 0.0) * iter_dist
        angles = [ ]
        
        for i in range(0, num_iter):
            point = self.anchor_point() + iter_vec*i
            self.recalc_from_pos(point)
            angles.append(self.phis)
            
        last_point = self.anchor_point() + iter_vec*num_iter
        self.recalc_from_pos(last_point)
        angles.append(self.phis)
        
        for i in range(0, num_iter):
            point = last_point - iter_vec*i + Vector3(0.0, 0.0, self.recovery_height)
            self.recalc_from_pos(point)
            angles.append(self.phis)
        
        last_point = self.anchor_point() + Vector3(0.0, 0.0, self.recovery_height)
        self.recalc_from_pos(last_point)
        angles.append(self.phis)
        
        for i in range(0, num_iter):
            point = last_point - iter_vec*i
            self.recalc_from_pos(point)
            angles.append(self.phis)
        
        last_point = self.anchor_point() + Vector3(0.0, 0.0, self.recovery_height) - iter_vec*num_iter
        self.recalc_from_pos(last_point)
        angles.append(self.phis)
        
        for i in range(0, num_iter):
            point = last_point + iter_vec*i - Vector3(0.0, 0.0, self.recovery_height)
            self.recalc_from_pos(point)
            angles.append(self.phis)

        return angles
        

class Spyder():
    def __init__(self, a : float, lengths : list[float]):
        self.a = a
        self.h = math.sqrt(3) / 2.0 * a
        
        self.feet = [ 
            SpyderFoot(lengths),
            SpyderFoot(lengths),
            SpyderFoot(lengths),
            SpyderFoot(lengths),
            SpyderFoot(lengths),
            SpyderFoot(lengths)
        ]

        self.const_lines = [ ]
        self.pos = [ 0.0, 0.0 ]
        
        self.hex = [ ]
        self.foot_lines = [
            [ ], [ ], [ ], [ ], [ ], [ ]
        ]
    

    def set_anchor_length(self, length : float):
        for i in range(0, 6):
            self.feet[i].set_anchor_length(length)

            
    def set_anchor_height(self, height : float):
        for i in range(0, 6):
            self.feet[i].set_anchor_height(height)
            
    
    def set_recovery_height(self, height : float):
        for i in range(0, 6):
            self.feet[i].set_recovery_height(height)
        
    
    def set_pos(self, pos : list[float]):
        self.pos = pos
    
    
    def pos_x(self, x : float) -> float:
        return x + self.pos[0]
    
    
    def pos_y(self, y : float) -> float:
        return y + self.pos[1]
    
    
    def foot_conn_point(self, index : int) -> list[float]:
        p1 = self.hex[index]
        p2 = self.hex[(index + 1) % 6]
        return [
            (p1[0] + p2[0]) / 2,
            (p1[1] + p2[1]) / 2
        ]
        
    
    def draw_const(self, pos : list[float]):
        self.set_pos(pos)
        
        hex_p1 = [ self.a, 0.0 ]
        hex_p2 = [ self.a/2.0, self.h ]
        hex_p3 = [ -self.a/2.0, self.h ]
        hex_p4 = [ -self.a, 0.0 ] 
        hex_p5 = [ -self.a/2.0, -self.h ]
        hex_p6 = [ self.a/2.0, -self.h ]
        
        self.hex = [ hex_p1, hex_p2, hex_p3, hex_p4, hex_p5, hex_p6 ]

        # TODO: Unwrap
        self.const_lines = [
            plt.plot(
                [ self.pos_x(hex_p1[0]), self.pos_x(hex_p2[0]) ],
                [ self.pos_y(hex_p1[1]), self.pos_y(hex_p2[1]) ],
                "black"
            ),
            plt.plot(
                [ self.pos_x(hex_p2[0]), self.pos_x(hex_p3[0]) ],
                [ self.pos_y(hex_p2[1]), self.pos_y(hex_p3[1]) ],
                "black"
            )[0],
            plt.plot(
                [ self.pos_x(hex_p3[0]), self.pos_x(hex_p4[0]) ],
                [ self.pos_y(hex_p3[1]), self.pos_y(hex_p4[1]) ],
                "black"
            )[0],
            plt.plot(
                [ self.pos_x(hex_p4[0]), self.pos_x(hex_p5[0]) ],
                [ self.pos_y(hex_p4[1]), self.pos_y(hex_p5[1]) ],
                "black"
            )[0],
            plt.plot(
                [ self.pos_x(hex_p5[0]), self.pos_x(hex_p6[0]) ],
                [ self.pos_y(hex_p5[1]), self.pos_y(hex_p6[1]) ],
                "black"
            )[0],
            plt.plot(
                [ self.pos_x(hex_p6[0]), self.pos_x(hex_p1[0]) ],
                [ self.pos_y(hex_p6[1]), self.pos_y(hex_p1[1]) ],
                "black"
            )[0]
        ]
        
    def draw_foot(self, index : int):
        rmat = Matrix4.new_rotatez(math.pi / 3 * index + math.pi / 6)
        
        f0 = self.foot_conn_point(index)
        f1, f2, f3 = self.feet[index].vecs
        
        f1 = rmat * f1
        f2 = rmat * f2
        f3 = rmat * f3
        
        # TODO: Unwrap
        self.foot_lines[index] = [
            plt.plot(
                [ self.pos_x(f0[0]), self.pos_x(f0[0] + f1[0]) ], 
                [ self.pos_y(f0[1]), self.pos_y(f0[1] + f1[1]) ], 
                "red"
            )[0],
            plt.plot(
                [ self.pos_x(f0[0] + f1[0]), self.pos_x(f0[0] + f1[0] + f2[0]) ], 
                [ self.pos_y(f0[1] + f1[1]), self.pos_y(f0[1] + f1[1] + f2[1]) ], 
                "green"
            )[0],
            plt.plot(
                [ self.pos_x(f0[0] + f1[0] + f2[0]), self.pos_x(f0[0] + f1[0] + f2[0] + f3[0]) ], 
                [ self.pos_y(f0[1] + f1[1] + f2[1]), self.pos_y(f0[1] + f1[1] + f2[1] + f3[1]) ], 
                "blue"
            )[0]
        ]
    
    
    def draw_feet(self):
        for i in range(0, 6):
            self.draw_foot(i)

        
    def update_foot_by_ang(self, index : int, phis : list[float]):
        # Update foot
        self.feet[index].recalc_from_ang(phis)
        
        a1, a2, a3 = self.foot_lines[index]
        
        rmat = Matrix4.new_rotatez(math.pi / 3 * index + math.pi / 6)
        
        f0 = self.foot_conn_point(index)
        f1, f2, f3 = self.feet[index].vecs
        
        f1 = rmat * f1
        f2 = rmat * f2
        f3 = rmat * f3
        
        a1.set_data(
            [ self.pos_x(f0[0]), self.pos_x(f0[0] + f1[0]) ], 
            [ self.pos_y(f0[1]), self.pos_y(f0[1] + f1[1]) ]
        )
        
        a2.set_data(
            [ self.pos_x(f0[0] + f1[0]), self.pos_x(f0[0] + f1[0] + f2[0]) ], 
            [ self.pos_y(f0[1] + f1[1]), self.pos_y(f0[1] + f1[1] + f2[1]) ]
        )
        
        a3.set_data(
            [ self.pos_x(f0[0] + f1[0] + f2[0]), self.pos_x(f0[0] + f1[0] + f2[0] + f3[0]) ], 
            [ self.pos_y(f0[1] + f1[1] + f2[1]), self.pos_y(f0[1] + f1[1] + f2[1] + f3[1]) ]
        )
    
        
    def create_paths(self, num_iter : int, iter_dist : float, angle : float) -> list[list[list[float]]]:
        return [ self.feet[i].create_path(num_iter, iter_dist, angle - math.pi / 3 * i - math.pi / 6) for i in range(0, 6) ]
        