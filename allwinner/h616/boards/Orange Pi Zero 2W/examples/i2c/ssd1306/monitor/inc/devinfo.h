#ifndef __CPU_INFO_H__
#define __CPU_INFO_H__

typedef struct {
    char         name[20];
    unsigned int user;
    unsigned int nice;
    unsigned int system;
    unsigned int idle;
} cpu_occupy_t;

float         GetCpuUsage();
unsigned long GetMemFree();
unsigned long GetMemTotal();
float         GetCpuTemp();
long long     GetAvailDisk();
float         CpuCalcOccupy(cpu_occupy_t* o, cpu_occupy_t* n);
void          CpuGetOccupy(cpu_occupy_t* cpust);

#endif
