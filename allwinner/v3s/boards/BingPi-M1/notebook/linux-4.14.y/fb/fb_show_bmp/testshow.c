#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <errno.h>

// 14byte文件头
typedef struct {
    char cfType[2];   // 文件类型，"BM"(0x4D42)
    int  cfSize;      // 文件大小（字节）
    int  cfReserved;  // 保留，值为0
    int  cfoffBits;   // 数据区相对于文件头的偏移量（字节）
} __attribute__((packed)) BITMAPFILEHEADER;
//__attribute__((packed))的作用是告诉编译器取消结构在编译过程中的优化对齐

// 40byte信息头
typedef struct {
    char ciSize[4];           // BITMAPFILEHEADER所占的字节数
    int  ciWidth;             // 宽度
    int  ciHeight;            // 高度
    char ciPlanes[2];         // 目标设备的位平面数，值为1
    int  ciBitCount;          // 每个像素的位数
    char ciCompress[4];       // 压缩说明
    char ciSizeImage[4];      // 用字节表示的图像大小，该数据必须是4的倍数
    char ciXPelsPerMeter[4];  // 目标设备的水平像素数/米
    char ciYPelsPerMeter[4];  // 目标设备的垂直像素数/米
    char ciClrUsed[4];        // 位图使用调色板的颜色数
    char ciClrImportant[4];   // 指定重要的颜色数，当该域的值等于颜色数时（或者等于0时），表示所有颜色都一样重要
} __attribute__((packed)) BITMAPINFOHEADER;

typedef struct
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char reserved;
} __attribute__((packed)) PIXEL;  // 颜色模式RGB

typedef struct
{
    int           fbfd;
    int*          fbp;
    unsigned int  xres;
    unsigned int  yres;
    unsigned int  xres_virtual;
    unsigned int  yres_virtual;
    unsigned int  xoffset;
    unsigned int  yoffset;
    unsigned int  bpp;
    unsigned long line_length;
    unsigned long size;

    struct fb_bitfield red;
    struct fb_bitfield green;
    struct fb_bitfield blue;
} FB_INFO;

typedef struct
{
    unsigned int  width;
    unsigned int  height;
    unsigned int  bpp;
    unsigned long size;
    unsigned int  data_offset;
} IMG_INFO;

FB_INFO  fb_info;
IMG_INFO img_info;

int show_bmp(char* bmpname, int x, int y)
{
    int i, j, k = 0;
    int w, h;              // 图片的宽和高
    int headbuf[2] = {0};  // headbuf[0]  宽   headbuf[1]   高

    // 定义数组存放转换得到的ARGB数据
    int lcdbuf[fb_info.xres * fb_info.yres];
    int bmpfd;
    int ret = 0;

    BITMAPFILEHEADER FileHead;
    BITMAPINFOHEADER InfoHead;

    if (bmpname == NULL)
    {
        printf("img_name is null\n");
        return -1;
    }

    // 打开要显示的bmp图片
    bmpfd = open(bmpname, O_RDWR);
    if (bmpfd == -1)
    {
        perror("打开bmp失败!\n");
        close(bmpfd);
        return -1;
    }

    /* 移位到文件头部 */
    lseek(bmpfd, 0, SEEK_SET);
    ret = read(bmpfd, &FileHead, sizeof(BITMAPFILEHEADER));
    if (ret < 0)
    {
        printf("img read failed\n");
        close(bmpfd);
        return ret;
    }

    // 检测是否是bmp图像
    if (memcmp(FileHead.cfType, "BM", 2) != 0)
    {
        printf("it's not a BMP file[%c%c]\n", FileHead.cfType[0], FileHead.cfType[1]);
        close(bmpfd);
        return ret;
    }
    ret = read(bmpfd, (char*)&InfoHead, sizeof(BITMAPFILEHEADER));
    if (ret < 0)
    {
        printf("read infoheader error!\n");
        close(bmpfd);
        return ret;
    }

    img_info.width       = InfoHead.ciWidth;
    img_info.height      = InfoHead.ciHeight;
    img_info.bpp         = InfoHead.ciBitCount;
    img_info.size        = FileHead.cfSize;
    img_info.data_offset = FileHead.cfoffBits;

    printf("img info w[%d] h[%d] bpp[%d] size[%ld] offset[%d]\n", img_info.width, img_info.height, img_info.bpp, img_info.size, img_info.data_offset);

    lseek(bmpfd, 18, SEEK_SET);
    // 读头信息
    read(bmpfd, headbuf, 8);
    w = headbuf[0];
    h = headbuf[1];

    // 判断图像是否在合理范围内
    if (x + w > fb_info.xres || y + h > fb_info.yres)
    {
        perror("图片超出范围！\n");
        return -1;
    }

    // 定义数组存放读取到的颜色值
    char bmpbuf[w * h * 3];  // BGR数据
                             // 定义一个临时数组
    int  tempbuf[w * h];

    // 跳过54字节的头信息，从55开始读取
    lseek(bmpfd, 28, SEEK_CUR);
    if ((3 * w) % 4 == 0)
    {
        read(bmpfd, bmpbuf, w * h * 3);  // 从55字节开始读取
    }
    else
    {
        for (int k = 0; k < h; k++)
        {
            read(bmpfd, &bmpbuf[k * w * 3], w * 3);
            lseek(bmpfd, 4 - (3 * w) % 4, SEEK_CUR);
        }
    }
    for (i = 0; i < w * h; i++)  // 保证每个像素点都能转换
    {
        lcdbuf[i] = 0x00 << 24 | bmpbuf[3 * i + 2] << 16 | bmpbuf[3 * i + 1] << 8 | bmpbuf[3 * i];
    }

    // 把颠倒的图片翻转过来
    for (i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            tempbuf[(h - j) * w + i] = lcdbuf[j * w + i];
        }
    }
    // 确定坐标
    for (i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            *(fb_info.fbp + fb_info.xres * (i + y) + j + x) = tempbuf[k];
            k++;
        }
    }

    return ret;
}

int show_picture(char* img_name, int xpos, int ypos)
{
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    long screen_size = 0;
    int  rgb_type    = 0;

    if (fb_info.fbfd <= -1)
    {
        printf("fb open fialed\n");
        return -1;
    }

    /*******************第2步：获取设备的硬件信息********************/
    if (ioctl(fb_info.fbfd, FBIOGET_FSCREENINFO, &finfo))
    {
        printf("fb ioctl fialed\n");
        return -1;
    }

    if (ioctl(fb_info.fbfd, FBIOGET_VSCREENINFO, &vinfo))
    {
        printf("fb ioctl fialed\n");
        return -1;
    }

    fb_info.xres         = vinfo.xres;
    fb_info.yres         = vinfo.yres;
    fb_info.xres_virtual = vinfo.xres_virtual;
    fb_info.yres_virtual = vinfo.yres_virtual;
    fb_info.xoffset      = vinfo.xoffset;
    fb_info.yoffset      = vinfo.yoffset;
    fb_info.bpp          = vinfo.bits_per_pixel;
    fb_info.line_length  = finfo.line_length;
    fb_info.size         = finfo.smem_len;

    memcpy(&fb_info.red, &vinfo.red, sizeof(struct fb_bitfield));
    memcpy(&fb_info.green, &vinfo.green, sizeof(struct fb_bitfield));
    memcpy(&fb_info.blue, &vinfo.blue, sizeof(struct fb_bitfield));

    printf("fb info x[%d] y[%d] x_v[%d] y_v[%d] xoffset[%d] yoffset[%d] bpp[%d] line_length[%ld] size[%ld]\n", fb_info.xres, fb_info.yres, fb_info.xres_virtual, fb_info.yres_virtual, fb_info.xoffset, fb_info.yoffset, fb_info.bpp, fb_info.line_length, fb_info.size);

    printf("fb info red off[%d] len[%d] msb[%d]\n", fb_info.red.offset, fb_info.red.length, fb_info.red.msb_right);
    printf("fb info green off[%d] len[%d] msb[%d]\n", fb_info.green.offset, fb_info.green.length, fb_info.green.msb_right);
    printf("fb info blue off[%d] len[%d] msb[%d]\n", fb_info.blue.offset, fb_info.blue.length, fb_info.blue.msb_right);

    if (fb_info.bpp != 16 && fb_info.bpp != 24 && fb_info.bpp != 32)
    {
        printf("fb bpp is not 16,24 or 32\n");
        return -1;
    }

    if (fb_info.red.length > 8 || fb_info.green.length > 8 || fb_info.blue.length > 8)
    {
        printf("fb red|green|blue length is invalid\n");
        return -1;
    }
    /*************************第3步：进行mmap***********************/
    // 内存映射
    fb_info.fbp = mmap(0, fb_info.size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_info.fbfd, 0);
    // fb_info.fbp = mmap(NULL, 800 * 480 * 4, PROT_READ |PROT_WRITE,MAP_SHARED, lcdfd, 0);
    if (fb_info.fbp == (int*)-1)
    {
        printf("mmap fialed\n");
        return -1;
    }

    /* 获取RGB的颜色颜色格式，比如RGB8888、RGB656 */
    rgb_type = vinfo.bits_per_pixel / 8;

    /* 屏幕的像素点 */
    screen_size = vinfo.xres * vinfo.yres * rgb_type;

    printf("vinfo.bits_per_pixel = %d\n", vinfo.bits_per_pixel);
    printf("rgb_type = %d\n", rgb_type);
    printf("screen_size = %d\n", screen_size);

    /********************第4步：进行lcd相关的操作********************/
    /* 刷黑屏幕 */
    memset(fb_info.fbp, 0xff, screen_size);

    show_bmp(img_name, xpos, ypos);

    /***********************第五步：释放显存************************/
    // 删除映射
    munmap(fb_info.fbp, fb_info.size);

    return 0;
}

int main(int argc, char** argv)
{
    char img_name[64];

    if (argc != 4)
    {
        printf("please input 4 args:\n");
        printf("./imageshow xx.bmp xpos ypos\n");
        return 0;
    }
    int x = atoi(argv[2]);  // atoi()将字符型转换成整形
    int y = atoi(argv[3]);

    snprintf(img_name, sizeof(img_name), "%s", argv[1]);
    printf("img_name = %s\n", img_name);

    /************************第1步：打开设备**************************/
    fb_info.fbfd = open("/dev/fb0", O_RDWR);
    if (!fb_info.fbfd)
    {
        printf("Error: cannot open framebuffer device(/dev/fb0).\n");
        return -1;
    }
    show_picture(img_name, x, y);

    /***********************第六步：关闭文件************************/
    close(fb_info.fbfd);

    return 0;
}
