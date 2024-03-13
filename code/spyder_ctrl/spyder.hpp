# pragma once

// Std libraries
# include <array>
# include <exception> 
# include <optional>

// Libraries
# include "PCA9685.hpp"

using PiPCA9685::PCA9685;

// Macros
# define SPYDER_LEG_COUNT 6
# define SPYDER_LEG_SERVO_COUNT 3
# define SPYDER_LEG_CHANNELS 4

# define SPYDER_PWM_FREQ 50
# define SPYDER_I2C_DEVICE "/dev/i2c-1"

// Angles maximum
# define SPYDER_SERVO_ANGLE_MIN     -90.0
# define SPYDER_SERVO_ANGLE_DEFAULT 0.0 
# define SPYDER_SERVO_ANGLE_MAX     90.0 

// Signals maximum
# define SPYDER_SERVO_SIG_MIN       103.0
# define SPYDER_SERVO_SIG_DEFAULT   307.0
# define SPYDER_SERVO_SIG_MAX       511.0

// Types
typedef float Angle;

// Constants
static const std::array<Angle, 3> SPYDER_DEFAULT_POS = { SPYDER_SERVO_ANGLE_DEFAULT, SPYDER_SERVO_ANGLE_DEFAULT, SPYDER_SERVO_ANGLE_DEFAULT };

class SpyderLeg {
private:
    // Private fields
    PCA9685* pwm;
    uint8_t leg_id, pwm_leg_id;

    std::array<Angle, 3> angles;    
    std::array<Angle, 3> offsets;

    // Private functions
    uint16_t get_signal_for_angle(uint8_t servo_id, Angle angle) {
        // Apply offset
        angle += this->offsets[servo_id];

        if (angle < SPYDER_SERVO_ANGLE_MIN) {
            throw std::exception("The given angle is too small!");
        }

        if (angle > SPYDER_SERVO_ANGLE_MAX) {
            throw std::exception("The given angle is too big!");
        }

        return (angle - SPYDER_SERVO_ANGLE_MIN) / (SPYDER_SERVO_ANGLE_MAX - SPYDER_SERVO_ANGLE_MIN) * (SPYDER_SERVO_SIG_MAX - SPYDER_SERVO_SIG_MIN) + SPYDER_SERVO_SIG_MIN;
    }

public:
    SpyderLeg(PCA9685* pwm, uint8_t leg_id, uint8_t pwm_leg_id, std::array<Angle, 3> offsets = { 0, 0, 0 }) 
        : pwm(pwm), leg_id(leg_id), pwm_leg_id(pwm_leg_id), angles(SPYDER_DEFAULT_POS), offsets(offsets)
    {

    }

    void set_angle(uint8_t servo_id, Angle angle) {
        if (servo_id > SPYDER_LEG_SERVO_COUNT) {
            throw std::exception("The given servo_id is out of range")
        }

        this->angles[servo_id] = angle;
        pwm->set_pwm(pwm_leg_id + servo_id, 0, get_signal_for_angle(servo_id, angle));
    }

    void set_angles(std::array<Angle, 3> angles) {
        this->angles = angles;
        for (uint8_t i = 0; i < SPYDER_LEG_COUNT; i++) {
            pwm->set_pwm(pwm_leg_id*SPYDER_LEG_CHANNELS + i, 0, get_signal_for_angle(i, angles[i]));
        }
    }

    // Positions
    void goto_default_position() {
        this->set_angles()
    }
};

class Spyder {
private:
    PCA9685 pwm1, pwm2;

    std::array<SpyderLeg, 6> legs;

public:
    Spyder() 
        : pwm1(SPYDER_I2C_DEVICE, 0x40), pwm2(SPYDER_I2C_DEVICE, 0x41),
            legs({
                SpyderLeg::SpyderLeg(&pwm1, 0, 0),
                SpyderLeg::SpyderLeg(&pwm1, 1, 1),
                SpyderLeg::SpyderLeg(&pwm1, 2, 2),
                SpyderLeg::SpyderLeg(&pwm2, 3, 0),
                SpyderLeg::SpyderLeg(&pwm2, 4, 1),
                SpyderLeg::SpyderLeg(&pwm2, 5, 2),
            })
    {
        pwm1.set_pwm_freq(SPYDER_PWM_FREQ);
        pwm2.set_pwm_freq(SPYDER_PWM_FREQ);
    }

    // Positions
    void goto_default_position() {
        for (uint8_t i = 0; i < SPYDER_LEG_COUNT; i++) {
            this->legs[i].goto_default_position();
        }
    }
};