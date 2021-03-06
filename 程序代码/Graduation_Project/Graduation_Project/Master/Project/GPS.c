#include "GPS.h"
#include "USART.h"

//void GPS_Init(void)
//{
//	void USART_config(void);
//	
//	USART_Cmd(USART1,ENABLE);
//	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
//	
//	

//}

void GPS_data_process(float num[3],char str[4])
{
	unsigned char i = 1;
	unsigned char j = 0;
	char num_size[7];
	char str_num[7][15];
//	int num[3];
//	char str[4];
	
	
	if(strstr(gps_data,"$GPGLL") != NULL )	//粗略判断数据的有效性
	{
//		printf("data vaild!\n");
		for(i = 1;i < 7;i++)
		{
			num_size[i-1] = ((NMAE_find_pos(i+1) - NMAE_find_pos(i)) -1 );	//计算两个‘，’之间有多少个字符
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
*	在 gps_data[] 找到第 num 个 ‘，’的下标并返回
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
*	将 buf 字符串转换成数字，省略小数点
*
*/
float NMAE_str2num(char *buf)
{
	signed char i = 0;
//	unsigned char j = 0;
	int result = 0;
	char *p = buf;
	
	while(1)
	{
		if(*p == '.')
			break;
		
		p++;
		i++;
	}
	i--;
	p = buf;
	
	while(i >= 0)
	{
		result += ((*p-'0') * num_pow(10,i));
		i--;
		p++;
	}
	
	return ((float)(result)/100.0 + (float)(result%100)/100.0);;
}

/*
*	计算 num1 的 num2 次方，即 num1^num2
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








