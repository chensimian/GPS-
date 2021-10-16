#ifndef _GPS_H_
#define _GPS_H_

#include "stm32f10x.h"
#include "string.h"

extern char gps_data[73];

int num_pow(int num1,int num2);
float NMAE_str2num(char buf[]);
unsigned char NMAE_find_pos(unsigned char num);
void GPS_data_process(float num[3],char str[4]);

#endif

