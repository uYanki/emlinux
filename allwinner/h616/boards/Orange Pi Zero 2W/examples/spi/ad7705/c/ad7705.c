#include "ad7705.h"

//  AD7705          OPI
//
//   VCC   ------  3V3
//   GND   ------  GND
//   SCK   ------  SPI1_SCK
//   DOUT  ------  SPI1_MISO
//   DIN   ------  SPI1_MOSI
//   CS    ------  SPI1_CS1
//   RST   ------  NC
//   DRDY  ------  NC

// DRDY: falling edge => 校准/转换完成

//-----------------------------------------------------------------------------
//

#define _ch1_GAIN_BIPOLAR_BUF (GAIN_1 | UNIPOLAR | BUF_EN)
#define _ch2_GAIN_BIPOLAR_BUF (GAIN_1 | UNIPOLAR | BUF_EN)

// 通信寄存器位定义
enum {
    /* 寄存器选择  RS2 RS1 RS0  */
    REG_COMM     = 0x00, /* 通信寄存器 */
    REG_SETUP    = 0x10, /* 设置寄存器 */
    REG_CLOCK    = 0x20, /* 时钟寄存器 */
    REG_DATA     = 0x30, /* 数据寄存器 */
    REG_ZERO_CH1 = 0x60, /* CH1 偏移寄存器 */
    REG_FULL_CH1 = 0x70, /* CH1 满量程寄存器 */
    REG_ZERO_CH2 = 0x61, /* CH2 偏移寄存器 */
    REG_FULL_CH2 = 0x71, /* CH2 满量程寄存器 */

    /* 读写操作 */
    WRITE = 0x00, /* 写操作 */
    READ  = 0x08, /* 读操作 */

    /* 通道 */
    CH_1 = 0, /* AIN1+  AIN1- */
    CH_2 = 1, /* AIN2+  AIN2- */
    CH_3 = 2, /* AIN1-  AIN1- */
    CH_4 = 3  /* AIN1-  AIN2- */
};

// 设置寄存器位定义
enum {
    /* 模式 */
    MD_NORMAL   = (0 << 6),  // 正常模式
    MD_CAL_SELF = (1 << 6),  // 自校准模式
    MD_CAL_ZERO = (2 << 6),  // 校准0刻度模式
    MD_CAL_FULL = (3 << 6),  // 校准满刻度模式

    /* 增益 */
    GAIN_1   = (0 << 3),
    GAIN_2   = (1 << 3),
    GAIN_4   = (2 << 3),
    GAIN_8   = (3 << 3),
    GAIN_16  = (4 << 3),
    GAIN_32  = (5 << 3),
    GAIN_64  = (6 << 3),
    GAIN_128 = (7 << 3),

    /* 极性 */
    // 无论双极性还是单极性都不改变任何输入信号的状态，
    // 它只改变输出数据的代码和转换函数上的校准点
    BIPOLAR  = (0 << 2),  // 双极性输入
    UNIPOLAR = (1 << 2),  // 单极性输入

    /* 内部缓冲器 */
    BUF_NO = (0 << 1),  // 输入无缓冲
    BUF_EN = (1 << 1),  // 输入有缓冲

    FSYNC_0 = 0,
    FSYNC_1 = 1 /* 不启用 */
};

// 时钟寄存器位定义
enum {
    CLKDIS_0 = 0x00, /* 时钟输出使能 （当外接晶振时，必须使能才能振荡） */
    CLKDIS_1 = 0x10, /* 时钟禁止 （当外部提供时钟时，设置该位可以禁止MCK_OUT引脚输出时钟以省电 */

    /*
        2.4576MHz（CLKDIV=0）或 4.9152MHz （CLKDIV=1），CLK 应置 “0”。
        1MHz     （CLKDIV=0）或 2MHz      （CLKDIV=1），CLK 应置 “1”。
    */
    CLK_4_9152M = 0x08,
    CLK_2_4576M = 0x00,
    CLK_1M      = 0x04,
    CLK_2M      = 0x0C,

    FS_50HZ  = 0x00,
    FS_60HZ  = 0x01,
    FS_250HZ = 0x02,
    FS_500HZ = 0x04,

    /*
        提高TM7705 精度的方法
            当使用主时钟为 2.4576MHz 时，强烈建议将时钟寄存器设为 84H,此时数据输出更新率为10Hz,即每0.1S 输出一个新数据。
            当使用主时钟为 1MHz 时，强烈建议将时钟寄存器设为80H, 此时数据输出更新率为4Hz, 即每0.25S 输出一个新数据
    */
    ZERO_0 = 0x00,
    ZERO_1 = 0x80
};

//-----------------------------------------------------------------------------
//

static void     ad7705_sync_spi(int fd);
static uint32_t ad7705_read_reg(int fd, uint8_t reg);
static void     ad7705_write_reg(int fd, uint8_t reg, uint32_t val);

void ad7705_init(int fd)
{
    // resets the AD7705 so that it expects a write to the communication register
    ad7705_sync_spi(fd);

    // write clock register
    spi_write_byte(fd, REG_CLOCK | WRITE | CH_1);

    // spi_write_byte(fd, CLKDIS_0 | CLK_4_9152M | FS_50HZ);
    spi_write_byte(fd, CLKDIS_0 | CLK_4_9152M | FS_500HZ);

    // intiates a self calibration and then after that starts converting
    ad7705_calib_self(fd, 1);
}

static void ad7705_sync_spi(int fd)
{
    extern uint8_t  spi_bits;
    extern uint32_t spi_speed;
    extern uint16_t spi_delay;

    int     ret;
    uint8_t tx[5] = {0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t rx[5] = {0};

    // Sync SPI: 同步时序
    struct spi_ioc_transfer tr = {
        .tx_buf        = (unsigned long)tx,
        .rx_buf        = (unsigned long)rx,
        .len           = 5,
        .delay_usecs   = spi_delay,
        .speed_hz      = spi_speed,
        .bits_per_word = spi_bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        pabort("can't send spi message");
    }
}

static uint32_t ad7705_read_reg(int fd, uint8_t reg)
{
    uint8_t  bits;
    uint32_t data = 0;

    switch (reg)
    {
        case REG_COMM:   // 通信寄存器
        case REG_SETUP:  // 设置寄存器 8bit
        case REG_CLOCK:  // 时钟寄存器 8bit
            bits = 8;
            break;

        case REG_ZERO_CH1:  // CH1 偏移寄存器 24bit
        case REG_FULL_CH1:  // CH1 满量程寄存器 24bit
        case REG_ZERO_CH2:  // CH2 偏移寄存器 24bit
        case REG_FULL_CH2:  // CH2 满量程寄存器 24bit
            bits = 24;
            break;

        case REG_DATA:  // 数据寄存器 16bit
        default:
            return 0xFFFFFFFF;
    }

    spi_write_byte(fd, reg | READ);

    do {
        data <<= 8;
        data |= spi_read_byte(fd);
    } while (bits -= 8);

    return data;
}

static void ad7705_write_reg(int fd, uint8_t reg, uint32_t val)
{
    uint8_t bits;

    switch (reg)
    {
        case REG_COMM:   // 通信寄存器
        case REG_SETUP:  // 设置寄存器 8bit
        case REG_CLOCK:  // 时钟寄存器 8bit
            bits = 8;
            break;

        case REG_ZERO_CH1:  // CH1 偏移寄存器 24bit
        case REG_FULL_CH1:  // CH1 满量程寄存器 24bit
        case REG_ZERO_CH2:  // CH2 偏移寄存器 24bit
        case REG_FULL_CH2:  // CH2 满量程寄存器 24bit
            bits = 24;
            break;

        case REG_DATA:  // 数据寄存器 16bit
        default:
            return;
    }

    // tell the AD7705 that the next write will be the setup register
    spi_write_byte(fd, reg | WRITE);

    do {
        spi_write_byte(fd, val & 0xFF);
        val >>= 8;
    } while (bits -= 8);
}

int ad7705_read_adc(int fd, uint8_t ch)
{
    // tell the AD7705 to read the data register (16 bits)

    if (ch == 1)
    {
        spi_write_byte(fd, 0x38);
    }
    else if (ch == 2)
    {
        spi_write_byte(fd, 0x39);
    }
    else
    {
        return 0;
    }

    // read the data register by performing two 8 bit reads
    int value = spi_read_word(fd);

    return value;
}

// 启动自校准. 内部自动短接AIN+ AIN-校准0位，内部短接到 Vref。
// 校准满位。此函数执行过程较长，约 180ms
void ad7705_calib_self(int fd, uint8_t ch)
{
    if (ch == 1)
    {
        spi_write_byte(fd, REG_SETUP | WRITE | CH_1);
        spi_write_byte(fd, MD_CAL_SELF | _ch1_GAIN_BIPOLAR_BUF | FSYNC_0);
    }
    else if (ch == 2)
    {
        spi_write_byte(fd, REG_SETUP | WRITE | CH_2);
        spi_write_byte(fd, MD_CAL_SELF | _ch2_GAIN_BIPOLAR_BUF | FSYNC_0);
    }
}

// 请将AIN+ AIN-短接后再启动系统校准零位。
// 校准结果 ad7705_read_reg(REG_ZERO_CH1) 和 ad7705_read_reg(REG_ZERO_CH2)
// 校准应该由主程序控制并保存校准参数。
void ad7705_sytem_calib_zero(int fd, uint8_t ch)
{
    if (ch == 1)
    {
        spi_write_byte(fd, REG_SETUP | WRITE | CH_1);
        spi_write_byte(fd, MD_CAL_ZERO | _ch1_GAIN_BIPOLAR_BUF | FSYNC_0);  // 启动自校准
    }
    else if (ch == 2)
    {
        spi_write_byte(fd, REG_SETUP | WRITE | CH_2);
        spi_write_byte(fd, MD_CAL_ZERO | _ch2_GAIN_BIPOLAR_BUF | FSYNC_0);
    }
}

// 请将AIN+ AIN-接最大输入电压源后再启动系统校准满位。
// 校准结果 ad7705_read_reg(REG_FULL_CH1) 和 ad7705_read_reg(REG_FULL_CH2)
// 校准应该由主程序控制并保存校准参数。
void ad7705_sytem_calib_full(int fd, uint8_t ch)
{
    if (ch == 1)
    {
        spi_write_byte(fd, REG_SETUP | WRITE | CH_1);
        spi_write_byte(fd, MD_CAL_FULL | _ch1_GAIN_BIPOLAR_BUF | FSYNC_0);
    }
    else if (ch == 2)
    {
        spi_write_byte(fd, REG_SETUP | WRITE | CH_2);
        spi_write_byte(fd, MD_CAL_FULL | _ch2_GAIN_BIPOLAR_BUF | FSYNC_0);
    }
}

bool ad7705_is_ready(int fd)
{
    int d = 0;

    // tell the AD7705 to send back the communications register
    spi_write_byte(fd, 0x08);
    // we get the communications register
    d = spi_read_byte(fd);
    // fprintf(stderr,"DRDY = %d\n",d);
    // loop while /DRDY is high

    return (d & 0x80) ? false : true;
}