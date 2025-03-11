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
int app_one()
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


float Kp = 1;
float Ki = 0.0;
float Kd = 0.01;
float ko = 10;
float speed = 15;
float norm = 15;


float SAMPLE_DELAY_MS = 33;

int app_main()
{
	uint16_t target = 30;
	float integral = 0;
	char buff[10];
	float val = 0;
	float lastError = 0;
	char type;
	while (1) {

		if (espRead(buff)) {
			sscanf(buff, "%c%f", &type, &val);
			switch (type) {
			case 'p':
				Kp = val;
				break;
			case 'i':
				Ki = val;
				break;
			case 'd':
				Kd = val;
				break;
			case 'o':
				ko = val;
				break;
			case 'e':
				speed = val;
				break;
			case 'n':
				norm = val;
			}
		}
		Color color;
		getColorHsv(&color);

		espPrintf("Value: %d", color.v);
		espPrintf("Hue: %d", color.h);
		espPrintf("Sat: %d", color.s);



		uint16_t input = color.v;
		//espPrintf("D: %d", input);

		float error = input - target;
		integral = fmax(fmin(integral + error * 1 / SAMPLE_DELAY_MS, 1), -1);
		float deriv = (error - lastError) * SAMPLE_DELAY_MS;
		lastError = error;
		float output = error * Kp + integral * Ki + deriv * Kd;

		//float forward = speed * expf(-fabs(error) * norm);
		float forward = ko + speed * (1 - (error * error) / (norm * norm));

		if (color.s > 30 && color.h < 200 && color.h > 100) {
			setMotorSpeed(MOT_L, speed);
			setMotorSpeed(MOT_R, speed);

		}
		else {
			setMotorSpeed(MOT_L, fmax(fmin(-output + forward, SPEED), -SPEED));
			setMotorSpeed(MOT_R, fmax(fmin(output + forward, SPEED), -SPEED));
		}

		delayMs(SAMPLE_DELAY_MS);
	}

}
