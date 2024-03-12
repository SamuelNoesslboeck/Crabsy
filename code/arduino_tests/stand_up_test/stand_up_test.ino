#include <Adafruit_PWMServoDriver.h>

// Servo tick values
# define SERVO_MIN 103
# define SERVO_MAX 511

// Configuration
static const auto LEG_INV_ORI = (bool[]) {
  true, true, true, false, false, false
};

static const auto SERVO_OFFSET = (int[]) {
  0, 0, 0,
  0, 0, 0,
  0, 0, 0,
  10, 15, 0,
  15, -10, 0,
  20, 0, 0,
};

// Positions
static const int* LEG_POSITION_NEUTRAL = (int[3]){ 0, 0, 0 };
static const int* LEG_POSITION_NICKED = (int[3]){ -90, 45, 0 };
static const int* LEG_POSITION_STANDING = (int[3]){ -45, -45, 0 };

// Servo boards
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x40);

uint16_t get_tick(uint8_t foot_id, uint8_t servo_id, int angle) {
  angle += SERVO_OFFSET[foot_id*3 + servo_id];
  
  if (LEG_INV_ORI[foot_id]) {
    angle = -angle;
  }
  
  return map(angle, -90, 90, SERVO_MIN, SERVO_MAX);
}

void set_foot(uint8_t foot_id, int* phis) {
  if (foot_id < 3) {
    for (uint8_t i = 0; i < 3; i++) {
      pwm1.setPWM(i + foot_id*4, 0, get_tick(foot_id, i, phis[i]));
    }
  } else {
    for (uint8_t i = 0; i < 3; i++) {
      pwm2.setPWM(i + (foot_id-3)*4, 0, get_tick(foot_id, i, phis[i]));
    }
  }
}

// Simple routine to bring all foots into neutral position
void neutral_position() {
  for (uint8_t i = 0; i < 6; i++) {
    set_foot(i, LEG_POSITION_NEUTRAL); 
  }
}

// A routine for letting the spider 
void stand_up() {
  for (uint8_t i = 0; i < 6; i++) {
    set_foot(i, LEG_POSITION_NICKED); 
  }

  delay(1000);

  for (uint8_t i = 0; i < 6; i++) {
    set_foot(i, LEG_POSITION_STANDING); 
  }
}

void setup() {
  // PWM Boards setup
  pwm1.begin();
  pwm1.setPWMFreq(50); 

  pwm2.begin();
  pwm2.setPWMFreq(50);

  // Spyder movement code
  neutral_position(); 

  delay(1000);

  stand_up();
}

void loop() {

}
