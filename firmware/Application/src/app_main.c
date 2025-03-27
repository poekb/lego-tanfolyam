#include "api_robotAbstraction.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

const char* WIFI_SSID = "LegoTanfolyam";
const char* WIFI_PASSWORD = "almaalma";

const char* SERVER_IP = "192.168.1.69";

#define SPEED 35
#define DELAY 20

void go200() {
	setMotorSpeed(MOT_L, 10);
	setMotorSpeed(MOT_R, 10);
	int c = getEncoderPosition(MOT_L);
	int t = c + 200;
	while (c < t) {
		c = getEncoderPosition(MOT_L);
		delayMs(10);
	}
}

void turnDeg(float d) {
	float rot = 0;
	if (d < 0) {
		d *= -1;
		setMotorSpeed(MOT_L, -10);
		setMotorSpeed(MOT_R, 10);
	}
	else {
		setMotorSpeed(MOT_L, 10);
		setMotorSpeed(MOT_R, -10);
	}
	while (fabs(rot) < d) {
		delayUs(10000);
		Vec3 gyro;
		getGyroData(&gyro);
		rot += gyro.z * 0.01;
		lcdPrintf(0, 0, "%g", rot);
		espPrintf("%g", rot);
	}
	return;
}
float Kp = 1;
float Ki = 0.0;
float Kd = 0.01;
float ko = 10;
float speed = 15;
float norm = 15;

int app_main() {
	int timer = 0;
	char dir[10] = "\0";
	unsigned int i = 0;
	while (!espRead(dir));
	Color col;
	setMotorSpeed(MOT_L, SPEED);
	setMotorSpeed(MOT_R, SPEED);
	while (1) {
		if (getIrDistance() < 280) {

			setMotorSpeed(MOT_L, 0);
			setMotorSpeed(MOT_R, 0);

			setServoPosition(-90);
			delayMs(1000);
			int left = getIrDistance();
			setServoPosition(90);
			delayMs(1000);
			int right = getIrDistance();
			setServoPosition(0);
			if (right < left) {
				turnDeg(90);
			}
			else {
				turnDeg(-90);
			}
			setMotorSpeed(MOT_L, SPEED);
			setMotorSpeed(MOT_R, SPEED);
		}
		delayMs(10);
		getColorHsv(&col);
		if (col.v < 15) break;
	}
	float integral = 0;
	float lastError = 0;
	while (1) {
		const float target = 30;

		getColorHsv(&col);

		uint16_t input = col.v;

		float error = input - target;
		integral = fmax(fmin(integral + error * 1 / DELAY, 1), -1);
		float deriv = (error - lastError) * DELAY;
		lastError = error;
		float output = error * Kp + integral * Ki + deriv * Kd;

		//float forward = speed * expf(-fabs(error) * norm);
		float forward = ko + speed * (1 - (error * error) / (norm * norm));

		setMotorSpeed(MOT_L, fmax(fmin(output + forward, SPEED), -SPEED));
		setMotorSpeed(MOT_R, fmax(fmin(-output + forward, SPEED), -SPEED));

		if (timer > 0) {
			timer--;
		}
		else if (col.s > 30 && col.h > 150 && col.h < 200) {
			int halt = 0;
			go200();
			switch (dir[i++]) {
			case 'l':
				turnDeg(-90);
				break;
			case 'r':
				turnDeg(70);
				break;
			case 's':
				break;
			case 'h':
			default:
				setMotorSpeed(MOT_L, 0);
				setMotorSpeed(MOT_R, 0);
				halt = 1;
				break;
			}
			if (halt) break;
			go200();
			timer = 200;
		}

		delayMs(DELAY);
	}
	return 0;
}
