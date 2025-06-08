
// make && ./app

#include <unistd.h>

// rst  -> 3v3 or 27
// sda  -> spi.cs1  (16)
// mosi -> spi.mosi (11)
// miso -> spi.miso (12)
// scl  -> spi.sclk (14)
// irq  -> x

#define DEMO 2
// 0: SPI Test
// 1: rc552 in loop
// 2: rc552 in timer

#if DEMO == 0  // SPI 读写测试

// 注: 如果读写失败, 大概率是没有 SPi 读写权限, 可尝试进入 root 模式再调用, 还不行就重启试试

#include "wiringPi.h"
#include "wiringPiSPI.h"
#include <stdint.h>
#include <stdio.h>

#define SPI_CH     1
#define SPI_CS     1

#define SPI_CS_PIN 16

#ifdef SPI_CS_PIN
#define SPI_Select()   digitalWrite(SPI_CS_PIN, LOW)
#define SPI_Unselect() digitalWrite(SPI_CS_PIN, HIGH)
#else
#define SPI_Select()
#define SPI_Unselect()
#endif

uint8_t spi_r(uint8_t addr)
{
    addr &= 0x7E;
    addr |= 0x80;  // read
    SPI_Select();
    wiringPiSPIDataRW(SPI_CH, &addr, 1);
    SPI_Unselect();
    return addr;
}

void spi_w(uint8_t addr, uint8_t data)
{
    addr &= 0x7E;
    uint8_t buff[2] = {addr, data};
    SPI_Select();
    wiringPiSPIDataRW(SPI_CH, buff, 2);
    SPI_Unselect();
}

int main()
{
    wiringPiSetup();
    // wiringPiSetupGpio();

#ifdef SPI_CS_PIN
    pinMode(SPI_CS_PIN, OUTPUT);
    pullUpDnControl(SPI_CS_PIN, PUD_UP);
    SPI_Unselect();
#endif

    wiringPiSPISetupMode(SPI_CH, SPI_CS, 1e6, 0);

    uint8_t reg = 0;
    while (1)
    {
        spi_w(reg, 0x44);
        printf("\r0x%02X. 0x%02X", reg, spi_r(reg));
        fflush(stdout);
        reg += 1;
        delay(10);
    }

    return 0;
}

#elif DEMO == 1

#include "mfrc522.h"

int main()
{
    MFRC522 mfrc;

    mfrc.PCD_Init();

    while (1)
    {
        // Look for a card
        if (!mfrc.PICC_IsNewCardPresent())
        {
            continue;
        }

        if (!mfrc.PICC_ReadCardSerial())
        {
            continue;
        }

        // Print UID
        for (byte i = 0; i < mfrc.uid.size; ++i)
        {
            if (mfrc.uid.uidByte[i] < 0x10)
            {
                printf(" 0");
                printf("%X", mfrc.uid.uidByte[i]);
            }
            else
            {
                printf(" ");
                printf("%X", mfrc.uid.uidByte[i]);
            }
        }

        printf("\n");
        delay(1000);
    }

    return 0;
}

#elif DEMO == 2

#include <iostream>
#include <signal.h>
#include <unistd.h>
#include "mfrc522.h"

MFRC522 mfrc;

void alarmWakeup(int sig)
{
    if (mfrc.PICC_IsNewCardPresent())
    {
        mfrc.PICC_ReadCardSerial();

        // display uid

        for (byte i = 0; i < mfrc.uid.size; ++i)
        {
            if (mfrc.uid.uidByte[i] < 0x10)
            {
                cout << " 0";
                cout << hex << (int)mfrc.uid.uidByte[i];
            }
            else
            {
                cout << " ";
                cout << hex << (int)mfrc.uid.uidByte[i];
            }
        }
        cout << endl;
    }
}

int main()
{
    mfrc.PCD_Init();

    // 周期调用
    signal(SIGALRM, alarmWakeup);
    ualarm(50000, 50000);

    while (1)
    {
    }
}

#endif