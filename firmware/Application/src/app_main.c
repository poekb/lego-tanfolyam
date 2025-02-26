/*
 * app_main.c
 */
#include "api_robotAbstraction.h"
#include <string.h>
#include <stdio.h>

const char* WIFI_SSID = "LegoTanfolyam";
const char* WIFI_PASSWORD = "almaalma";

const char* SERVER_IP = "192.168.1.69";
#define SPEED 20
int app_main()
{
	char data[50];
	while (!espRead(data));
	int x;
	sscanf(data, "start %d", &x);

	for (int i = 1; i < x * 4 + 1; i++) {
		setMotorSpeed(MOT_L, SPEED);
		setMotorSpeed(MOT_R, SPEED);

		delayMs(2000);

		setMotorSpeed(MOT_L, SPEED / 4);
		setMotorSpeed(MOT_R, -SPEED / 4);

		float angle = 0;
		while (angle > -85)
		{
			Vec3 gyro;
			getGyroData(&gyro);
			angle += gyro.z * 0.01;
			delayUs(10000);
		}
		setMotorSpeed(MOT_L, 0);
		setMotorSpeed(MOT_R, 0);

		lcdPrintf(0, 0, "Kr: %d", i / 4);
		lcdPrintf(1, 0, "Fd: %d", i);
		espPrintf("Fd: %d Kr: %d", i, i / 4);
	}
	setMotorSpeed(MOT_L, 0);
	setMotorSpeed(MOT_R, 0);
	return 0;
}
