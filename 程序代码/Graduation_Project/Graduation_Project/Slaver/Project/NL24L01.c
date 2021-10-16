#include "NL24L01.h"
#include "spi.h"
#include "stdio.h"

char NRF_RX_ADDR[5] = {0X01,0X02,0X03,0X04,0x05};	//通道0的接收地址
char NRF_TX_ADDR[5] = {0X01,0X02,0X03,0X04,0x05};	//通道0的发射地址

unsigned char CHANAL1 = 30;		//从机 1 通信频道（频率）
unsigned char CHANAL2 = 70;		//从机 2 通信频道（频率）
unsigned char CHANAL3 = 100;	//从机 3 通信频道（频率）

unsigned char NRF_CE_IRQ_FLAG = 0;


void delay(int time)
{
	int i,j;
	for(i=0;i<time;i++)
	{
		for(j=0;j<5000;j++);
	}
}

char NRF_TRANFER_BYTE(char data)
{
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1,data);
	
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == RESET);	
	return SPI_I2S_ReceiveData(SPI1);
}

void NRF_REG_WRITE_BYTE(unsigned char reg,unsigned char data)
{
	GPIO_ResetBits(GPIOB,SPI1_CS_GPIO);
	
	NRF_TRANFER_BYTE(reg);
	
	NRF_TRANFER_BYTE(data);
	
	GPIO_SetBits(GPIOB,SPI1_CS_GPIO);
}

unsigned char NRF_REG_READ_BYTE(unsigned char reg)
{
	unsigned char reg_val;
	
	GPIO_ResetBits(GPIOB,SPI1_CS_GPIO);
	
	NRF_TRANFER_BYTE(reg);
	
	reg_val = NRF_TRANFER_BYTE(0XFF);
	
	GPIO_SetBits(GPIOB,SPI1_CS_GPIO);
	
	return reg_val;
}

void NRF_WRITE_BUF(unsigned char reg,char *write_buf,unsigned char size_buf)
{
	unsigned char i = 0;
	GPIO_ResetBits(GPIOB,SPI1_CS_GPIO);
	
	NRF_TRANFER_BYTE(reg);
	
	for(i=0;i < size_buf;i++)
	{
		NRF_TRANFER_BYTE(*write_buf);
		write_buf++;
	}
	
	GPIO_SetBits(GPIOB,SPI1_CS_GPIO);
	
}

void NRF_READ_BUF(unsigned char reg,char *read_buf,unsigned char size_buf)
{
	unsigned char i = 0;
	GPIO_ResetBits(GPIOB,SPI1_CS_GPIO);
	
	NRF_TRANFER_BYTE(reg);
	
	for(i=0;i < size_buf;i++)
	{
		*read_buf = NRF_TRANFER_BYTE(0xff);
		read_buf++;
	}
	
	GPIO_SetBits(GPIOB,SPI1_CS_GPIO);
	
}


//配置 NRF 并进入接收模式
//传入通信频道
//通过改变频道来与不同的从机通信
void NRF_RX_MODE(unsigned char chanal)
{
	GPIO_ResetBits(GPIOB,NRF_CE_GPIO);
	
	NRF_WRITE_BUF(RX_ADDR_P0+NRF_WRITE_REG,NRF_RX_ADDR,5);//配置通道0的地址
	
	NRF_REG_WRITE_BYTE(EN_AA+NRF_WRITE_REG,0X01);//使能通道0的自动应答
	
	NRF_REG_WRITE_BYTE(EN_RXADDR+NRF_WRITE_REG,0X01);//使能通道0的接收地址
	
	NRF_REG_WRITE_BYTE(RF_CH+NRF_WRITE_REG,chanal);	//设置RF通信频率
	
	NRF_REG_WRITE_BYTE(RX_PW_P0+NRF_WRITE_REG,32);//通道0的有效数据宽度为32byte
	
	NRF_REG_WRITE_BYTE(RF_SETUP+NRF_WRITE_REG,0X0f);//设置TX发射参数
	
	NRF_REG_WRITE_BYTE(CONFIG+NRF_WRITE_REG,0x0f);//配置寄存器，接收模式、启用中断
	
	GPIO_SetBits(GPIOB,NRF_CE_GPIO);//拉高 CE 进入接收模式
}

//配置 NRF 并进入发射模式
//传入通信频道
//通过改变频道来与不同的从机通信

void NRF_TX_MODE(unsigned char chanal)
{
	GPIO_ResetBits(GPIOB,NRF_CE_GPIO);
	
	NRF_WRITE_BUF(TX_ADDR+NRF_WRITE_REG,NRF_TX_ADDR,5);//配置发射地址
	
	NRF_WRITE_BUF(RX_ADDR_P0+NRF_WRITE_REG,NRF_RX_ADDR,5);//配置接收地址，用在自动应答模式
	
	NRF_REG_WRITE_BYTE(EN_AA+NRF_WRITE_REG,0X01);//使能自动应答
	
	NRF_REG_WRITE_BYTE(EN_RXADDR+NRF_WRITE_REG,0X01);//使能接收地址
	
	NRF_REG_WRITE_BYTE(SETUP_RETR+NRF_WRITE_REG,0X00);//不使用自动重发
	
	NRF_REG_WRITE_BYTE(RF_CH+NRF_WRITE_REG,chanal);//配置通信频率
	
	NRF_REG_WRITE_BYTE(RF_SETUP+NRF_WRITE_REG,0X0f);//配置发射参数
	
	NRF_REG_WRITE_BYTE(RX_PW_P0+NRF_WRITE_REG,32);//配置有效数据宽度
	
	NRF_REG_WRITE_BYTE(CONFIG+NRF_WRITE_REG,0X0E);//配置基本工作参数
	
}
//NRF发送数据
int NRF_WRITE_TX_DATA(char *buf)
{
	unsigned char state;
	int ret_val = 0;
	GPIO_ResetBits(GPIOB,NRF_CE_GPIO);//进入待机模式
	
	NRF_WRITE_BUF(WR_TX_PLOAD,buf,8);

	GPIO_SetBits(GPIOB,NRF_CE_GPIO);	//进入发送模式

//	while(NRF_CE_IRQ_FLAG == 0);
//	NRF_CE_IRQ_FLAG = 0;
	delay(10);
	
	GPIO_ResetBits(GPIOB,NRF_CE_GPIO);
	
	state = NRF_REG_READ_BYTE(STATUS+NRF_READ_REG);
	
	if(state&TX_DS)
	{
		ret_val = 1;
	}
	
	NRF_REG_WRITE_BYTE(STATUS+NRF_WRITE_REG,state);//清除标志
	
	NRF_REG_WRITE_BYTE(FLUSH_TX,0XFF);
	return ret_val;
}


//读出缓冲区中的数据
char NRF_READ_RX_DATA(char *buf)
{
	unsigned char state;
	
	GPIO_ResetBits(GPIOB,NRF_CE_GPIO);//接入休闲模式
	
	state = NRF_REG_READ_BYTE(STATUS+NRF_READ_REG);
	
	NRF_REG_WRITE_BYTE(STATUS+NRF_WRITE_REG,state);//清除标志
	
	if(state&RX_DR)
	{
		NRF_READ_BUF(RD_RX_PLOAD,buf,8);
		NRF_REG_WRITE_BYTE(FLUSH_RX,0XFF);
		return 1;
	}
	else
		return 0;
}

//主要用于NRF与MCU是否正常连接
u8 NRF_Check(void)
{
	char buf[5]={2,2,2,2,2};
	char buf1[5];
	u8 i; 
	
	NRF_WRITE_BUF(NRF_WRITE_REG+TX_ADDR,buf,5);

	NRF_READ_BUF(TX_ADDR,buf1,5); 
	 
	for(i=0;i<5;i++)
	{
		if(buf1[i] != 2)
		break;
	} 
	       
	if(i==5)
		return SUCCESS ;        //MCU与NRF成功连接 
	else
		return ERROR ;        //MCU与NRF不正常连接
}

//NRF的初始化
void NRF_mode_init(void)
{
	GPIO_ResetBits(GPIOB,NRF_CE_GPIO);
	
	NRF_WRITE_BUF(RX_ADDR_P0+NRF_WRITE_REG,NRF_RX_ADDR,5);//配置通道0的地址
	
	NRF_REG_WRITE_BYTE(EN_AA+NRF_WRITE_REG,0X01);//使能通道0的自动应答
	
	NRF_REG_WRITE_BYTE(EN_RXADDR+NRF_WRITE_REG,0X01);//使能通道的接收地址
		
	NRF_REG_WRITE_BYTE(RX_PW_P0+NRF_WRITE_REG,8);//通道0的有效数据宽度为32byte
	
	NRF_REG_WRITE_BYTE(RF_SETUP+NRF_WRITE_REG,0X0f);//设置TX发射参数
	

	NRF_WRITE_BUF(TX_ADDR+NRF_WRITE_REG,NRF_TX_ADDR,5);//配置发射地址
	
	NRF_REG_WRITE_BYTE(SETUP_RETR+NRF_WRITE_REG,0X00);//不使用自动重发
}









