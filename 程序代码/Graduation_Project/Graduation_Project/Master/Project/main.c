#include "stm32f10x.h"
#include "timer.h"
#include "USART.h"
#include "spi.h"
#include "NL24L01.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

#define light_speed			2.99792458	// 具体为 299792458m/s.单位：m/s
#define pi				3.14159265358979323846
#define EARTH_R 	6371393	 //地球半径单位：米

extern char NRF_RX_ADDR[5];	//通道0的接收地址
extern char NRF_TX_ADDR[5];	//通道0的发射地址

const unsigned char CHANAL[3] = {30,70,100};	//通信频道

unsigned int timecount = 0;	//用于时间计时
extern unsigned char NRF_CE_IRQ_FLAG;	//NRF24L01的中断标志

unsigned char NRF_Tx_data_OK = 0;	//NRF24L01发送完成的标志

char USART_RX_OK_FLAG = 0;	//接收一帧数据完成的标志


void my_delay(int time);	//延时函数
void measur_distan(unsigned int distance[3]);	//测量主机与各个从机的距离
void get_GPS(double xy[3][2]);	//获取每个从机的GPS信息
void calculat_GPS(unsigned int distance[3],double xy[3][2], double target_GPS[2],char target_lat_lon[2]);//计算主机的GPS信息


int main(void)
{
	unsigned int distance[3] = {0,0,0};
	double xy[3][2] = {{0,0},{0,0},{0,0}};
	double target_GPS[2];
	char target_lat_lon[2] = {0,0};	//最终的经纬度，N、S、W、E	
	
	char *buf = (char* )malloc(32);
	memset(buf,1,32);

	//设置中断优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	// TIM2 初始化
	timer1_configuration();
	// USART 初始化
	USART_config();
	USART_Cmd(USART1,ENABLE);
	USART_Cmd(USART2,ENABLE);
	// SPI 初始化
	SPI_configuration();
	
	//检测NRF24L01的连接情况
	if(NRF_Check())
		printf("\r\nconnet nrf succes\r\n");
	else
		printf("\r\nconnet nrf error\r\n");
	//初始化NRF24L01
	NRF_mode_init();
	
	while(1)
	{
		//测量距离
		measur_distan(distance);
		//获取从机GPS
		get_GPS(xy);
		//计算主机GPS
		calculat_GPS(distance,xy,target_GPS,target_lat_lon);
		//打印相关信息
		printf("\r\n从机1的距离：%d 米\r\n",distance[0]);
		printf("\r\n从机2的距离：%d 米\r\n",distance[1]);
		printf("\r\n从机3的距离：%d 米\r\n",distance[2]);
		printf("\r\n主机的GPS：%c %f , %c %f\r\n",target_lat_lon[0],target_GPS[0],target_lat_lon[1],target_GPS[1]);
		printf("\r\n*******************END*********************\r\n");
		//延时
		my_delay(8000);
	}
}

void my_delay(int time)
{
	int i,j;
	for(i=0;i<time;i++)
	{
		for(j=0;j<5000;j++);
	}
}

void measur_distan(unsigned int distance[3])
{
	char k;
	char buf[8];
	char state = 0;
	unsigned char i = 0;
	unsigned int handle_time = 372065;
	unsigned int time[3] = {0,0,0};

	
	NRF_REG_WRITE_BYTE(CONFIG+NRF_WRITE_REG,0X1A);//配置发射模式基本工作参数
	for(k=0;k<3;k++)
	{
		NRF_REG_WRITE_BYTE(RF_CH+NRF_WRITE_REG,CHANAL[k]);//配置通信频率
		GPIO_ResetBits(GPIOB,NRF_CE_GPIO);//NRF进入待机模式
		
		while(i<250)	//连续测量250个数据
		{
			
			while(1)
			{
				NRF_WRITE_BUF(WR_TX_PLOAD,buf,8);	//向NRF写入数据
			
				TIM_SetCounter(TIM2,0);		//清除时钟
				
				GPIOB->BSRR = NRF_CE_GPIO;	//进入发送模式
				
				TIM2->CR1 |= TIM_CR1_CEN;	//使能时钟
				
				my_delay(1);	
				
				GPIO_ResetBits(GPIOB,NRF_CE_GPIO);	//NRF进入休闲模式
			
				state = NRF_REG_READ_BYTE(STATUS+NRF_READ_REG);	//读取NRF标志位
				NRF_REG_WRITE_BYTE(STATUS+NRF_WRITE_REG,state);//清除标志位
			
				NRF_REG_WRITE_BYTE(FLUSH_TX,0XFF);	//清除NRF发送缓冲区
				if(state&TX_DS)	//判断是否受到从机的应答信号
					break;
			}
			
			//求总的相加值
			time[k] += (timecount*900000 + TIM_GetCounter(TIM2) * 13.88888888889);//计算TOF时间（包括延时）
			
			timecount = 0;//清0时钟计数器
			i++;
		}
		i = 0;
		time[k] = time[k] / 250;	//取传输时间的平均值
		time[k] = abs((time[k] - handle_time))/2;//减去延时时间，计算出TOF（不包括延时）
		distance[k] = time[k] * light_speed / 10; //计算主从机之间的距离。单位：m
	}
	//printf("\r\nEXIT measure distance\r\n");
}

void get_GPS(double xy[3][2])
{
	char k = 0;
	char i = 0;
	char j = 0;
	char state;
	char Tx_buf[8]="gps";
	char Rx_buf[8]={0,0,0,0,0,0,0,0};
	int tmp_xy[3][2]={{0,0},{0,0},{0,0}};
	

	for(k=0;k<3;k++)
	{
		//发送读取信号
		NRF_REG_WRITE_BYTE(CONFIG+NRF_WRITE_REG,0X1A);//配置发射基本工作参数
		NRF_REG_WRITE_BYTE(RF_CH+NRF_WRITE_REG,CHANAL[k]);//配置通信频率
		
		while(1)
		{
			
			NRF_WRITE_BUF(WR_TX_PLOAD,Tx_buf,8);//向NRF发送缓冲区写入数据
			
			GPIO_SetBits(GPIOB,NRF_CE_GPIO);	//进入发送模式
			
			my_delay(5);
			
			GPIO_ResetBits(GPIOB,NRF_CE_GPIO);	//进入休闲模式
		
			state = NRF_REG_READ_BYTE(STATUS+NRF_READ_REG);//读取标志位
			NRF_REG_WRITE_BYTE(STATUS+NRF_WRITE_REG,state);//清除标志位
		
			NRF_REG_WRITE_BYTE(FLUSH_TX,0XFF);//清除发送缓冲区
			if(state&TX_DS)
				break;
		}
		NRF_CE_IRQ_FLAG = 0;
		//读取信息	
		NRF_REG_WRITE_BYTE(CONFIG+NRF_WRITE_REG,0x1B);//配置寄存器，接收模式、启用中断
		GPIO_SetBits(GPIOB,NRF_CE_GPIO);
		NRF_READ_RX_DATA(Rx_buf);//等待并接收从机的GPS信息
		
		//存储GPS信息
		for(i=0,j=0;i<2;i++)
		{
			tmp_xy[k][i] += Rx_buf[j++] << 0;
			tmp_xy[k][i] += Rx_buf[j++] << 8;
			tmp_xy[k][i] += Rx_buf[j++] << 16;
			tmp_xy[k][i] += Rx_buf[j++] << 24;
			xy[k][i] = (float)(tmp_xy[k][i]) / 10000000.0;
		}
	}
	//printf("\r\nEXIT get_GPS\r\n");
}

void calculat_GPS(unsigned int distance[3],double xy[3][2],double target_GPS[2],char target_lat_lon[2])
{
	double target_xy[2] = {0,0};
	//计算 x 坐标
	target_xy[0] = (xy[0][0]+xy[1][0]+xy[2][0])/3;
	//计算 y 坐标
	target_xy[1] = (xy[0][1]+xy[1][1]+xy[2][1])/3;
	//计算 GPS
	target_GPS[0] = target_xy[0] / pi*180;	//经度
	target_GPS[1] = (2 * (atanf(expf(target_xy[1])) - pi/4)) / pi*180;//纬度
	
	//计算南北纬和东西经
	if(target_xy[0] < 0)
	{
		target_lat_lon[0] = 'W';
	}
	else
	{
		target_lat_lon[0] = 'E';
	}
	if(target_xy[1] < 0)
	{
		target_lat_lon[1] = 'S';
	}
	else
	{
		target_lat_lon[1] = 'N';
	}
//	printf("\r\nEXIT calculat GPS\r\n");
}
