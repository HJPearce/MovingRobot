/* MovingRobot.h - Uses the arduino motor shield & an I2C connected magne
   Created by Hayden Pearce, April 29th 2019
   pls no stealing x.
*/
#include "Arduino.h"
#include "MAG3110.h"
#ifndef MovingRobot_h
#define MovingRobot_h

class MovingRobot {
public:
	MovingRobot();
	void Begin();
	void turn(int angle, float set_point);
	void moveForward(int set_speed);
	void brake();
	float get_heading();
	void general_move(bool L_motor_direction, bool R_motor_direction, int set_speed);
private:
	void turnTo(int heading);
	MAG3110 magnetometer = MAG3110();
	byte R_MOTOR_DIR_PIN;
	byte R_MOTOR_SPEED_PIN;
	byte L_MOTOR_DIR_PIN;
	byte L_MOTOR_SPEED_PIN;
}
int sign(float value);
int sign(int value);
float signmod180(float value);
void initialisePins(byte * pinList, byte mode);
#endif
