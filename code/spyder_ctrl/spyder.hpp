# pragma once

// Std libraries
# include <array>
# include <exception> 
# include <iostream>
# include <math.h>
# include <optional>
# include <stdexcept>
# include <string>

// Libraries
# include "PCA9685.hpp"
using PiPCA9685::PCA9685;

# include "eigen3/Eigen/Dense"
using Eigen::Vector3f;
using Eigen::Matrix3f;

// Macros
# define SPYDER_LEG_COUNT 6
# define SPYDER_LEG_SERVO_COUNT 3
# define SPYDER_LEG_CHANNELS 4

# define SPYDER_PWM_FREQ 50
# define SPYDER_I2C_DEVICE "/dev/i2c-1"

# define SPYDER_I2C_ADDR_1 0x40
# define SPYDER_I2C_ADDR_2 0x41


// Angles maximum
# define SPYDER_SERVO_ANGLE_MIN     -90.0
# define SPYDER_SERVO_ANGLE_DEFAULT 0.0 
# define SPYDER_SERVO_ANGLE_MAX     90.0 

// Signals maximum
# define SPYDER_SERVO_SIG_MIN       103.0
# define SPYDER_SERVO_SIG_DEFAULT   307.0
# define SPYDER_SERVO_SIG_MAX       511.0

// Types
/// @brief A simple type defining the accuracy in angles
typedef float Angle;

// Positions
static const std::array<Angle, 3> SPYDER_DEFAULT_POS = { SPYDER_SERVO_ANGLE_DEFAULT, SPYDER_SERVO_ANGLE_DEFAULT, SPYDER_SERVO_ANGLE_DEFAULT };
static const std::array<Angle, 3> SPYDER_NICKED_POS = { 0.0, 45.0, -90.0 };

// Helper functions
    Matrix3f rot_matrix_x(Angle angle) {
        return Eigen::Matrix3f({
            { 1, 0, 0 },
            { 0, cos(angle), -sin(angle) },
            { 0, sin(angle), cos(angle) }
        });
    }

    Matrix3f rot_matrix_y(Angle angle) {
        return Eigen::Matrix3f({
            { cos(angle), 0, sin(angle) },
            { 0, 1, 0 },
            { -sin(angle), 0, cos(angle) }
        });
    }

    Matrix3f rot_matrix_z(Angle angle) {
        return Eigen::Matrix3f({
            { cos(angle), -sin(angle), 0 },
            { sin(angle), cos(angle), 0 },
            { 0, 0, 1 }
        });
    }
// 

/// @brief A single leg of the Spyder
class SpyderLeg {
private:
    // Private fields
    PCA9685* pwm;
    uint8_t leg_id, pwm_leg_id;
    std::array<Vector3f, 3> segments;

    std::array<Angle, 3> angles = SPYDER_DEFAULT_POS;    

    // Calibration
    std::array<Angle, 3> offsets = { 0, 0, 0 };
    bool inverted;

    // Private functions
        /// @brief 
        uint16_t get_signal_for_angle(uint8_t servo_id, Angle angle) {
            if (this->inverted) {
                angle = -angle;
            }

            // Apply offset
            angle += this->offsets.at(servo_id);

            if (angle < SPYDER_SERVO_ANGLE_MIN) {
                throw std::runtime_error(std::string("The given angle is too small! Angle: ") 
                    + std::to_string(angle) + ", Offset: " + std::to_string(this->offsets[servo_id]));
            }

            if (angle > SPYDER_SERVO_ANGLE_MAX) {
                throw std::runtime_error(std::string("The given angle is too big! Angle: ") 
                    + std::to_string(angle) + ", Offset: " + std::to_string(this->offsets[servo_id]));
            }

            return (angle - SPYDER_SERVO_ANGLE_MIN) / (SPYDER_SERVO_ANGLE_MAX - SPYDER_SERVO_ANGLE_MIN) * (SPYDER_SERVO_SIG_MAX - SPYDER_SERVO_SIG_MIN) + SPYDER_SERVO_SIG_MIN;
        }
    // 

public:
    SpyderLeg(PCA9685* pwm, uint8_t leg_id, uint8_t pwm_leg_id, std::array<Vector3f, 3> segments, bool inverted = false) 
        : pwm(pwm), leg_id(leg_id), pwm_leg_id(pwm_leg_id), inverted(inverted), segments(segments)
    {

    }

    // ID
        uint8_t id() {
            return this->leg_id;
        }
    // 

    // Angles
        Angle angle_of(uint8_t servo_id) {
            return this->angles.at(servo_id);
        }

        std::array<Angle, 3> angles() {
            return this->angles;
        }

        void set_angle(uint8_t servo_id, Angle angle) {
            this->angles.at(servo_id) = angle;
            pwm->set_pwm(pwm_leg_id*SPYDER_LEG_CHANNELS + servo_id, 0, get_signal_for_angle(servo_id, angle));
        }

        void set_angles(std::array<Angle, 3> angles) {
            this->angles = angles;
            for (uint8_t i = 0; i < SPYDER_LEG_SERVO_COUNT; i++) {
                std::cout << "Set servo angle of servo: " << i << " to " << angles[i] << std::endl;
                pwm->set_pwm(pwm_leg_id*SPYDER_LEG_CHANNELS + i, 0, get_signal_for_angle(i, angles[i]));
            }
        }
    // 

    // Forward kinematics
        Vector3f endpoint() {
            auto matrix_1 = rot_matrix_z(angles[0]);
            auto matrix_2 = rot_matrix_x(angles[1]);
            auto matrix_3 = rot_matrix_x(angles[2]);
            
            return
                matrix_1 * segments[0] +
                matrix_1 * matrix_2 * segments[1] +
                matrix_1 * matrix_2 * matrix_3 * segments[2];
        }
    // 

    // Positions
        void goto_default_position() {
            this->set_angles(SPYDER_DEFAULT_POS);
        }

        void goto_nicked_position() {
            this->set_angles(SPYDER_NICKED_POS);
        }
    // 
};

class Spyder {
private:
    PCA9685 pwm1, pwm2;

    std::array<SpyderLeg, 6> legs;

    // Positions
    Vector3f leg_anchor_pos;

public:
    Spyder(Vector3f leg_anchor_pos, std::array<std::array<Vector3f, 3>, 6> segment_list) 
        : pwm1(SPYDER_I2C_DEVICE, SPYDER_I2C_ADDR_1), pwm2(SPYDER_I2C_DEVICE, SPYDER_I2C_ADDR_2), leg_anchor_pos(leg_anchor_pos),
            legs({
                SpyderLeg(&pwm1, 0, 0, segment_list[0]),
                SpyderLeg(&pwm1, 1, 1, segment_list[1]),
                SpyderLeg(&pwm1, 2, 2, segment_list[2]),
                SpyderLeg(&pwm2, 3, 0, segment_list[3]),
                SpyderLeg(&pwm2, 4, 1, segment_list[4]),
                SpyderLeg(&pwm2, 5, 2, segment_list[5])
            })
    {
        this->pwm1.set_pwm_freq(SPYDER_PWM_FREQ);
        this->pwm2.set_pwm_freq(SPYDER_PWM_FREQ);
    }

    // Legs
        SpyderLeg* leg(uint8_t leg_id) {
            return &this->legs.at(leg_id);
        }
    // 

    // Forward kinematics
        Vector3f endpoint_of(uint8_t leg_id) {
            return rot_matrix_z(M_PI / 3 * leg_id) * (this->legs.at(leg_id).endpoint() + this->leg_anchor_pos);
        }
    // 

    // Positions
    void goto_default_position() {
        for (uint8_t i = 0; i < SPYDER_LEG_COUNT; i++) {
            this->legs[i].goto_default_position();
        }
    }
};