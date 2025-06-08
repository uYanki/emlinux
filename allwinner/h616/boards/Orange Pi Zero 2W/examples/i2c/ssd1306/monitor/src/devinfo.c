#include <stdio.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/statfs.h>
#include "devinfo.h"

#define TEMP_FILE_PATH "/sys/class/thermal/thermal_zone0/temp"
#define MAX_SIZE       32

float CpuCalcOccupy(cpu_occupy_t* o, cpu_occupy_t* n)
{
    unsigned long od, nd;
    unsigned long id, sd;
    float         cpu_use = 0;

    od = (unsigned long)(o->user + o->nice + o->system + o->idle);  // 第一次 (用户+优先级+系统+空闲) 的时间
    nd = (unsigned long)(n->user + n->nice + n->system + n->idle);  // 第二次（用户+优先级+系统+空闲）的时间

    id = (unsigned long)(n->user - o->user);      // 用户时间差
    sd = (unsigned long)(n->system - o->system);  // 系统时间差

    if ((nd - od) != 0)
    {
        cpu_use = ((sd + id) * 100.0) / (nd - od);  // (用户+系统) * 100 / 时间差
    }
    else
    {
        cpu_use = 0;
    }
    return cpu_use;
}

void CpuGetOccupy(cpu_occupy_t* cpust)
{
    FILE*         fd;
    char          buff[256]    = "";
    cpu_occupy_t* cpu_occupy_t = NULL;
    cpu_occupy_t               = cpust;

    fd = fopen("/proc/stat", "r");

    if (fd != NULL)
    {
        fgets(buff, sizeof(buff), fd);
        sscanf(buff, "%s %u %u %u %u",
               cpu_occupy_t->name,
               &cpu_occupy_t->user,
               &cpu_occupy_t->nice,
               &cpu_occupy_t->system,
               &cpu_occupy_t->idle);
        fclose(fd);
    }
}

float GetCpuUsage()
{
    cpu_occupy_t cpu_stat1;
    cpu_occupy_t cpu_stat2;
    float        cpu;

    // 获取CPU使用情况
    CpuGetOccupy((cpu_occupy_t*)&cpu_stat1);
    sleep(1);
    CpuGetOccupy((cpu_occupy_t*)&cpu_stat2);

    // 计算CPU使用率
    cpu = CpuCalcOccupy((cpu_occupy_t*)&cpu_stat1, (cpu_occupy_t*)&cpu_stat2);
    return cpu;
}

unsigned long GetMemTotal()
{
    struct sysinfo s_info;

    if (sysinfo(&s_info) == 0)
    {
        return s_info.totalram;
    }
    else
    {
        return -1;
    }
}

unsigned long GetMemFree()
{
    struct sysinfo s_info;

    if (sysinfo(&s_info) == 0)
    {
        return s_info.freeram;
    }
    else
    {
        return -1;
    }
}

float GetCpuTemp()
{
    FILE*  fp;
    char   buf[MAX_SIZE] = "";
    double tempVal       = 0.0;

    fp = fopen(TEMP_FILE_PATH, "r");
    if (fp == NULL)
    {
        printf("Fail to Read Temp...\n");
    }
    else
    {
        fread(buf, 1, MAX_SIZE, fp);
        tempVal = atof(buf) / 1000;
        fclose(fp);
    }

    return tempVal;
}

long long GetAvailDisk()
{
    struct statfs diskInfo;

    if (statfs("/opt", &diskInfo) == -1)
    {
        return 0;
    }

    unsigned long long availableDisk = diskInfo.f_bavail * diskInfo.f_bsize;

    return availableDisk >> 20;
}
