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

void turnDeg(int deg) {


}

float Kp = 0.3;
float Ki = 0.1;
float Kd = 0.0;
float ko = 1;
float eps = 10;
int app_main()
{
	uint16_t target = 400;
	float integral = 0;
	char buff[10];
	float val = 0;
	float lastError = 0;
	char type;
	int timer = 0;
	int look = 0;
	int dir = 1;
	while (1) {
		timer += 1;
		if (timer == 15){
			look += 30 * dir;

		}else if(timer == 30){
			dir *= -1;
			look += 30 * dir;
			timer = 0;
		}

		setServoPosition(look);

		/*
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
				eps = val;
			}
		}
	*/
		uint16_t input = getIrDistance();
		//espPrintf("D: %d", input);

		float error = input - target;
		integral = fmax(fmin(integral + error * 0.016, 1), -1);
		float deriv = (error - lastError) * 30;
		lastError = error;
		float output = 30; //error * Kp + integral * Ki + deriv * Kd;



		setMotorSpeed(MOT_L, fmax(fmin(ko * output, SPEED), -SPEED));
		setMotorSpeed(MOT_R, fmax(fmin(ko * output, SPEED), -SPEED));

		if ((error) < 0) {
			setMotorSpeed(MOT_L, 0);
			setMotorSpeed(MOT_R, 0);
			setServoPosition(-90);
			delayMs(1000);
			uint16_t left = getIrDistance();
			setServoPosition(90);
			delayMs(1000);
			if (left > getIrDistance()) {
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
			}
			else {
				setMotorSpeed(MOT_L, -SPEED / 4);
				setMotorSpeed(MOT_R, SPEED / 4);

				float angle = 0;
				while (angle < 85)
				{
					Vec3 gyro;
					getGyroData(&gyro);
					angle += gyro.z * 0.01;
					delayUs(10000);
				}
				setMotorSpeed(MOT_L, 0);
				setMotorSpeed(MOT_R, 0);
			}
			setServoPosition(0);
			delayMs(1000);
		}


		delayMs(33);
	}

}
