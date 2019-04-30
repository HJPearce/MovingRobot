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
	this->setPins(output_pins[0],output_pins[1],output_pins[2],output_pins[3]);
	this->magnetometer.initialize();
	this->magnetometer.setCalMode();
	this->magnetometer.calibrate();
}

MovingRobot::setPins(byte left_motor_direction_pin, byte right_motor_direction_pin, byte left_motor_speed_pin, byte Right_motor_speed_pin){
	this->L_MOTOR_DIR_PIN = left_motor_direction_pin;
	this->L_MOTOR_SPEED_PIN = left_motor_speed_pin;
	this->R_MOTOR_DIR_PIN = right_motor_direction_pin;
	this->R_MOTOR_SPEED_PIN right_motor_speed_pin;
	pinMode(left_motor_direction_pin, OUTPUT);
	pinMode(left_motor_speed_pin, OUTPUT);
	pinMode(right_motor_direction_pin, OUTPUT);
	pinMode(right_motor_speed_pin, OUTPUT);
}

MovingRobot::turnTo(int angle) {
	byte set_speed = 255;
	const float SUCCESS_THRESHOLD = 2; //in degrees, just to make the change achievable

	int turn_direction = sign(signmod180(this->get_heading() - angle));

	switch (turn_direction) {
	case 1:
		this->general_move(false, true, set_speed);
	case 0:
		//some kind of error messaging
	case -1:
		this->general_move(true, false, set_speed);
	}
	while (abs(this->get_heading() - angle) > SUCCESS_THRESHOLD) {
		//stop it from freaking out
		delay(3);
	}
	this->brake();
}

MovingRobot::turn(int angle, float setpoint = this->get_heading()) {
	int new_setpoint = signmod180(set_point + angle);
	new_setpoint;
	this->turnTo(new_setpoint);
}

MovingRobot::moveForward(int set_speed) {
	this->general_move(true, true, set_speed);
}

MovingRobot::brake() {
	int wait = 5; //If it under-turns look right here
	digitalWrite(this->L_MOTOR_DIR_PIN, !digitalRead(this->L_MOTOR_DIR_PIN));
	digitalWrite(this->R_MOTOR_DIR_PIN, !digitalRead(this->R_MOTOR_DIR_PIN));
	delay(wait);
	digitalWrite(this->L_MOTOR_SPEED_PIN, 0);
	digitalWrite(this->R_MOTOR_SPEED_PIN, 0);
	}

MovingRobot::general_move(bool L_motor_direction, bool R_motor_direction, int set_speed) {

	if (R_motor_direction) digitalWrite(this->R_MOTOR_DIR_PIN, HIGH);
	else digitalWrite(this->R_MOTOR_DIR_PIN, LOW);
	if (L_motor_direction) digitalWrite(this->L_MOTOR_DIR_PIN, HIGH);
	else digitalWrite(this->L_MOTOR_DIR_PIN, LOW);
	digitalWrite(this->R_MOTOR_SPEED_PIN, set_speed);
	digitalWrite(this->L_MOTOR_SPEED_PIN, set_speed);
}

MovingRobot::get_heading() {
	this->magnetometer.exitStandby();
	this->magnetometer.triggerMeasurement();
	while (!this -> magnetometer.dataReady()) delay(3);
	return signmod180(this->magnetometer.readHeading());
}