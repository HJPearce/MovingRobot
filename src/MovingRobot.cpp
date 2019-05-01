#include "Arduino.h"
#include "MovingRobot.h"
#include "MAG3110.h"

int sign(float value) {
	if (value > 0) return 1;
	if (value == 0) return 0;
	return -1;
}

int sign(int value) {
	if (value > 0) return 1;
	if (value == 0) return 0;
	return -1;
}

float signmod180(float value) {
	while (value < -180) {
		value += 360;
	}
	while (value > 180) {
		value -= 360;
	}
	return value;
}

void initialisePins(byte *pinList, byte mode) {
	for (int ii = 0; ii < sizeof(pinList) / sizeof(*pinList); ii++) {
		pinMode(pinList[ii], mode);
	}
}

MovingRobot::MovingRobot(){}

MovingRobot::Begin(){
	/*
Output order:
  L_MOTOR_DIR_PIN
  L_MOTOR_SPEED_PIN
  R_MOTOR_DIR_PIN
  R_MOTOR_SPEED_PIN
 */
	byte output_pins[] = { 11, 13, 10, 12};
	setPins(output_pins[0],output_pins[1],output_pins[2],output_pins[3]);
	magnetometer.initialize();
	magnetometer.setCalMode();
	magnetometer.calibrate();
	int start_pos = get_heading();
	general_move(true, false, 255);
	delay(50);
	while (abs(start_pos - get_heading()) > 3) delay(5);
	brake();
	while (magnetometer.isCalibrating()) delay(3);
}

MovingRobot::setPins(byte left_motor_direction_pin, byte right_motor_direction_pin, byte left_motor_speed_pin, byte Right_motor_speed_pin){
	L_MOTOR_DIR_PIN = left_motor_direction_pin;
	L_MOTOR_SPEED_PIN = left_motor_speed_pin;
	R_MOTOR_DIR_PIN = right_motor_direction_pin;
	R_MOTOR_SPEED_PIN right_motor_speed_pin;
	pinMode(left_motor_direction_pin, OUTPUT);
	pinMode(left_motor_speed_pin, OUTPUT);
	pinMode(right_motor_direction_pin, OUTPUT);
	pinMode(right_motor_speed_pin, OUTPUT);
}

MovingRobot::turnTo(int angle, byte set_speed) {
	const float SUCCESS_THRESHOLD = 2; //in degrees, just to make the change achievable

	int turn_direction = sign(signmod180(get_heading() - angle));

	switch (turn_direction) {
	case 1:
		general_move(false, true, set_speed);
	case 0:
		//some kind of error messaging
	case -1:
		general_move(true, false, set_speed);
	}
	while (abs(get_heading() - angle) > SUCCESS_THRESHOLD) {
		//stop it from freaking out
		delay(3);
	}
	brake();
}

MovingRobot::turn(int angle, float setpoint = get_heading(), int set_speed = 255) {
	int new_setpoint = signmod180(set_point + angle);
	new_setpoint;
	turnTo(new_setpoint, set_speed);
}

MovingRobot::moveForward(int set_speed) {
	general_move(true, true, set_speed);
}

MovingRobot::brake() {
	int wait = 5; //If it under-turns look right here
	digitalWrite(L_MOTOR_DIR_PIN, !digitalRead(L_MOTOR_DIR_PIN));
	digitalWrite(R_MOTOR_DIR_PIN, !digitalRead(R_MOTOR_DIR_PIN));
	delay(wait);
	digitalWrite(L_MOTOR_SPEED_PIN, 0);
	digitalWrite(R_MOTOR_SPEED_PIN, 0);
	}

MovingRobot::general_move(bool L_motor_direction, bool R_motor_direction, int set_speed) {

	if (R_motor_direction) digitalWrite(R_MOTOR_DIR_PIN, HIGH);
	else digitalWrite(R_MOTOR_DIR_PIN, LOW);
	if (L_motor_direction) digitalWrite(L_MOTOR_DIR_PIN, HIGH);
	else digitalWrite(L_MOTOR_DIR_PIN, LOW);
	digitalWrite(R_MOTOR_SPEED_PIN, set_speed);
	digitalWrite(L_MOTOR_SPEED_PIN, set_speed);
}

MovingRobot::get_heading() {
	if (!magnetometer.activeMode){
		magnetometer.exitStandby();
	}
	magnetometer.triggerMeasurement();
	while (!this -> magnetometer.dataReady()) delay(3);
	return signmod180(magnetometer.readHeading());
}
