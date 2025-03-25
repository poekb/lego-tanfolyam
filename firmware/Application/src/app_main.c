/*
 * app_main.c
 */
#include "api_robotAbstraction.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

const char* WIFI_SSID = "LegoTanfolyam";
const char* WIFI_PASSWORD = "almaalma";

const char* SERVER_IP = "192.168.1.69";

#define SPEED 35

int app_main() {
	while (1) {
		setMotorSpeed(MOT_L, SPEED);
		setMotorSpeed(MOT_R, SPEED);

		if (getIrDistance() < 280) {

			setMotorSpeed(MOT_L, 0);
			setMotorSpeed(MOT_R, 0);

			setServoPosition(-90);
			delayMs(1000);
			int left = getIrDistance();
			setServoPosition(90);
			delayMs(1000);
			float rot = 0;
			float target = 90;
			if (getIrDistance() < left) {
				setMotorSpeed(MOT_L, 5);
				setMotorSpeed(MOT_R, -5);
			}
			else {
				setMotorSpeed(MOT_L, -5);
				setMotorSpeed(MOT_R, 5);
			}
			setServoPosition(0);
			while (fabs(rot) < target) {
				delayUs(10000);
				Vec3 gyro;
				getGyroData(&gyro);
				rot += gyro.z * 0.01;
				lcdPrintf(0, 0, "%g", rot);
				espPrintf("%g", rot);
			}

			setMotorSpeed(MOT_L, SPEED);
			setMotorSpeed(MOT_R, SPEED);
		}

		delayMs(10);
	}



}
