#include "stm32f10x.h"
#include "timer.h"
#include "USART.h"
#include "GPS.h"
#include "spi.h"
#include "gps_convert_xyz.h"
#include "NL24L01.h"

char temp_gpsdata[73]="";//一个用于接收，一个用于处理，防止数据被覆盖；
char gps_data[73]="$GPGLL,2160.7668,N,11027.7624,E,092321.00,A,A*60";
char i = 0;//串口计数
char USART_RX_OK_FLAG = 0;	//接收一帧数据完成的标志
char NRF_RX_OK = 0;//NRF接收完成标志
unsigned char timecount = 0;//时间计数

extern unsigned char NRF_RX_ADDR[5];	//通道0的接收地址
extern unsigned char NRF_TX_ADDR[5];	//通道0的发射地址

extern unsigned char CHANAL1;		//从机 1 通信频道（频率）
extern unsigned char CHANAL2;		//从机 2 通信频道（频率）
extern unsigned char CHANAL3;	//从机 3 通信频道（频率）
extern unsigned char NRF_CE_IRQ_FLAG;//NRF中断标志

//获取自身的GPS信息
void get_GPS(char buf[]);

int main(void)
{
	char Tx_buf[8]={0,0,0,0,0,0,0,0};
	char tmp_rx_buf[8];
		
//初始化时钟	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	timer1_configuration();
//初始化 USART
	USART_config();
	USART_Cmd(USART2,ENABLE);
//初始化 SPI
	SPI_configuration();
	
	//检查NRF的连接情况
	if(NRF_Check())
		printf("\r\nNRF connet SUCCES!\r\n");
	else
		printf("\r\nNRF connet ERROR\r\n");
	//初始化NRF
	NRF_mode_init();
	//配置通信频率
	NRF_REG_WRITE_BYTE(RF_CH+NRF_WRITE_REG,CHANAL1);	
	//配置寄存器，接收模式、启用中断
	NRF_REG_WRITE_BYTE(CONFIG+NRF_WRITE_REG,0x1B);
	//进入接收等待
	GPIO_SetBits(GPIOB,NRF_CE_GPIO);	
	while(1)
	{	
		if(NRF_RX_OK)
		{
			
			NRF_READ_RX_DATA(tmp_rx_buf);	//读取NRF缓冲区
			NRF_CE_IRQ_FLAG = 1;
			if(strncmp(tmp_rx_buf,"gps",3) == 0)//判断主机是否要读取GPS信息
			{
				NRF_REG_WRITE_BYTE(CONFIG+NRF_WRITE_REG,0X1A);//配置发射基本工作参数
				
				get_GPS(Tx_buf);
				//数据无效需要重测
				
				//向NRF发送缓冲区写入数据并发射
				while(1)
				{
					if(NRF_WRITE_TX_DATA(Tx_buf))
						break;
				}				
				
				//配置成接收模式
				NRF_REG_WRITE_BYTE(CONFIG+NRF_WRITE_REG,0x1B);
				//进入接收等待
				GPIO_SetBits(GPIOB,NRF_CE_GPIO);
			}
			else
			{
				GPIO_SetBits(GPIOB,NRF_CE_GPIO);//进入接收模式
			}
			NRF_RX_OK = 0;
		}
	}
}

//该函数的 bug ，需要对数据的有效性进行判断，无效就重测
//获取自身的GPS信息
void get_GPS(char buf[8])
{
	char ii=0;
	char jj=0;
	int xy[2] = {0,0};
	unsigned char flag=0;
	USART_Cmd(USART1,ENABLE);//使能串口接收GPS信息
	while(1)
	{
		if(USART_RX_OK_FLAG)//判断是否接收完一帧GPS数据
		{
			USART_RX_OK_FLAG = 0;	//清除接收完成标志；
			strcpy(gps_data,temp_gpsdata);//拷贝接收到的数据
			flag = gps2xyz(xy);	//利用墨卡托将接收的 gps 并转换成 xy 坐标
			printf("x=%d,y=%d\n",xy[0],xy[1]);
			jj = 0;
			//将数据填入发送数组
			for(ii=0;ii<2;ii++)
			{
				buf[jj++] = xy[ii] >> 0;
				buf[jj++] = xy[ii] >> 8;
				buf[jj++] = xy[ii] >> 16;
				buf[jj++] = xy[ii] >> 24;
			}
			printf("\r\ngps data %s\r\n",gps_data);
			if(strstr(gps_data,"$GPGLL") != NULL)
			{
//				if(flag==1)
					break;
			}
		}
	}
	USART_Cmd(USART1,DISABLE);
}


