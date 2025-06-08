
#include <iostream>
#include <stdlib.h>
#include <wiringPi.h>
#include <signal.h>
#include "mpu6050.h"

using namespace std;

MPU6050 mpu6050(ADDRESS_AD0_LOW);

int main()
{
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

#if 0

    mpu6050.setXGyroOffset(102);
    mpu6050.setYGyroOffset(-52);
    mpu6050.setZGyroOffset(52);

    mpu6050.setXAccelOffset(-1752);
    mpu6050.setYAccelOffset(1776);
    mpu6050.setZAccelOffset(1936);

    delay(3000);

#endif

    double giroX;
    double giroY;
    double giroZ;

    for (;;)
    {
        // Mesure rotation

        giroX = 0.0;
        giroY = 0.0;
        giroZ = 0.0;

        int nbBoucle = 10;

        for (int compteur = 0; compteur != nbBoucle; compteur++)
        {
            giroX += mpu6050.getGyroXDegS();
            giroY += mpu6050.getGyroYDegS();
            giroZ += mpu6050.getGyroZDegS();
        }

        giroX /= nbBoucle;
        giroY /= nbBoucle;
        giroZ /= nbBoucle;

        // Affichage
        system("clear");
        if (giroX < 0)
        {
            cout << "Tangage négatif (descendre): " << giroX << endl;  // 负俯仰（向下）
        }
        if (giroX > 0)
        {
            cout << "Tangage positif (monter): " << giroX << endl;  // 正俯仰（上升）
        }
        if (giroY < 0)
        {
            cout << "Roulis négatif (baisser à gauche): " << giroY << endl;  // 负滚转（向左下降）
        }
        if (giroY > 0)
        {
            cout << "Roulis positif (baisser à droite): " << giroY << endl;  // 正滚转（向右下降）
        }
        if (giroZ < 0)
        {
            cout << "Lacet négatif (tourner à droite): " << giroZ << endl;  // 负偏航（右转）
        }
        if (giroZ > 0)
        {
            cout << "Lacet positif (tourner à gauche): " << giroZ << endl;  // 正偏航（左转）
        }
    }
}
