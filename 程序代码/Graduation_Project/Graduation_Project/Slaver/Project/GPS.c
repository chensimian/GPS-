#include "GPS.h"
#include "USART.h"
#include "stdio.h"


//GPS��Ϣ����ȡ
void GPS_data_process(float num[3],char str[4])
{
	unsigned char i = 1;
	unsigned char j = 0;
	char num_size[7];
	char str_num[7][15];
	
	if(strstr(gps_data,"$GPGLL") != NULL )	//�����ж����ݵ���Ч��
	{
		for(i = 1;i < 7;i++)
		{
			num_size[i-1] = ((NMAE_find_pos(i+1) - NMAE_find_pos(i)) -1 );	//��������������֮���ж��ٸ��ַ�
			strncpy(str_num[i-1],&gps_data[NMAE_find_pos(i)+1],num_size[i-1]);
		}
		
		num_size[i-1] = 1;
		str_num[i-1][0] = gps_data[NMAE_find_pos(i)+1];
		

		for(i = 0;i <= 4;i=i+2)
		{
			num[j++] = NMAE_str2num(&str_num[i][0]);
		}
		
		j = 0;
		
		for(i = 1;i <= 5;i=i+2)
		{
			str[j++] = *str_num[i];
		}
		str[j] = *str_num[6];		
	}
}


/*
*	�� gps_data[] �ҵ��� num �� ���������±겢����
*
*/
unsigned char NMAE_find_pos(unsigned char num)
{
	unsigned char i = 0;
	while(num)
	{
		if(gps_data[i] == '*' || gps_data[i] < ' ' || gps_data[i] > 'z')
			return 0x00;
		
		if(gps_data[i] == ',')
			num--;
		i++;
	}
	return --i;
}

/*
*	�� buf �ַ���ת�������֣���ȷ���֣�ʡ��С����
*
*/
float NMAE_str2num(char *buf)
{
	signed char i = 0;
	unsigned char j = 0;
	unsigned char k = 0;
	int integer = 0;
	float dec = 0;
	float result = 0;
	char *p = buf;
	
	while(1)//�������ֵĳ���
	{
		if((*p > '9') | (*p < '0'))
		{
			if(*p == '.')
				i = k;
			else
				break;
		}
		p++;
		k++;
	}

	j = i + 1;
	i--;
	p = buf;
	
	while(i >= 0)
	{
		integer += ((*p-'0') * num_pow(10,i));
		i--;
		p++;
	}
	
	for(i=1;j<k;j++)
	{
		p++;
		dec += (*p - '0') / (num_pow(10,i) / 1.0);
		i++;
	}
	
	result = (float)(integer/100) + (((float)(integer%100) + dec) / 60.0);
	return result;
	
}

/*
*	���� num1 �� num2 �η����� num1^num2
*
*/
int num_pow(int num1,int num2)
{
	int result = 1;
	while(num2)
	{
		result *= num1;
		num2--;
	}
	
	return result;
}








