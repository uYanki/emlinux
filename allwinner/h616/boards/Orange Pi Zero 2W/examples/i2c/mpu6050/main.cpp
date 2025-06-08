// g++ -o app *.cpp -lwiringPi -lpthread

#include <iostream>  // cout
#include <stdlib.h>
#include <wiringPi.h>
#include <cmath>
#include "mpu6050.h"
#include <signal.h>  // signal

using namespace std;

MPU6050 mpu6050(ADDRESS_AD0_LOW);

#define RAD_TO_DEG 57.2958

void fin(int sig)
{
    cout << "stop" << endl;
    exit(0);
}

int main()
{
    // Listen to Ctrl-C using the function you want to activate
    signal(SIGINT, fin);

    mpu6050.defaultInitialize();

    cout << "Who am I: " << hex << (int)mpu6050.whoAmI() << dec << endl;

    if (mpu6050.isConnectionOK())
    {
        cout << "Connection: OK" << endl;
    }
    else
    {
        cout << "Connection: KO" << endl;
        exit(-1);
    }

#if 1

    cout << "getXGyroOffset: " << mpu6050.getXGyroOffset() << endl;
    cout << "getYGyroOffset: " << mpu6050.getYGyroOffset() << endl;
    cout << "getZGyroOffset: " << mpu6050.getZGyroOffset() << endl;

    cout << "getXAccelOffset: " << mpu6050.getXAccelOffset() << endl;
    cout << "getYAccelOffset: " << mpu6050.getYAccelOffset() << endl;
    cout << "getZAccelOffset: " << mpu6050.getZAccelOffset() << endl;

#else

    mpu6050.setXGyroOffset(102);
    mpu6050.setYGyroOffset(-52);
    mpu6050.setZGyroOffset(52);

    mpu6050.setXAccelOffset(-1752);
    mpu6050.setYAccelOffset(1776);
    mpu6050.setZAccelOffset(1936);

#endif

    // delay(1000);

    double angleXZ, angleYZ;
    double temp;
    double accelX, accelY, accelZ;
    double gyroX, gyroY, gyroZ;
    int    compteur = 0;

    for (;;)
    {
#if 1

        {
            // 掉线检测

            static int lose = 0;

            if (mpu6050.whoAmI() != WHO_AM_I_DEFAULT)
            {
                if (lose++ == 10)
                {
                    break;
                }
            }
            else
            {
                lose = 0;
            }
        }

#endif

        system("clear");

        cout << compteur++ << " | ";

#if 0  // temp

        temp = (double)mpu6050.getTempDeg();
        cout << temp << endl;

#endif
#if 0  // accle,gyro

        accelX = (double)mpu6050.getAccelXMS();
        accelY = (double)mpu6050.getAccelYMS();
        accelZ = (double)mpu6050.getAccelZMS();
        gyroX  = (double)mpu6050.getGyroXDegS();
        gyroY  = (double)mpu6050.getGyroYDegS();
        gyroZ  = (double)mpu6050.getGyroZDegS();

        cout << accelX << " | " << accelY << " | " << accelZ << endl;
        cout << gyroX << " | " << gyroY << " | " << gyroZ << endl;

#endif
#if 1  // angle

        accelX = (double)mpu6050.getAccelXMS();
        accelY = (double)mpu6050.getAccelYMS();
        accelZ = (double)mpu6050.getAccelZMS();

        angleXZ = atan2(accelX, accelZ) * RAD_TO_DEG;  // pitch
        angleYZ = atan2(accelY, accelZ) * RAD_TO_DEG;  // roll
        cout << angleXZ << " | " << angleYZ << endl;

#endif

        if ((angleXZ == 0 && angleYZ == 0) || (angleXZ == -135 && angleYZ == -135))
        {
            // 有时候线不稳导致模块复位, 就重新初始化
            mpu6050.defaultInitialize();
        }

        delay(100);
    }
}
