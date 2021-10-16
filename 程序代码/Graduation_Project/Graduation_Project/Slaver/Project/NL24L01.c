#include "NL24L01.h"
#include "spi.h"
#include "stdio.h"

char NRF_RX_ADDR[5] = {0X01,0X02,0X03,0X04,0x05};	//ͨ��0�Ľ��յ�ַ
char NRF_TX_ADDR[5] = {0X01,0X02,0X03,0X04,0x05};	//ͨ��0�ķ����ַ

unsigned char CHANAL1 = 30;		//�ӻ� 1 ͨ��Ƶ����Ƶ�ʣ�
unsigned char CHANAL2 = 70;		//�ӻ� 2 ͨ��Ƶ����Ƶ�ʣ�
unsigned char CHANAL3 = 100;	//�ӻ� 3 ͨ��Ƶ����Ƶ�ʣ�

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


//���� NRF ���������ģʽ
//����ͨ��Ƶ��
//ͨ���ı�Ƶ�����벻ͬ�Ĵӻ�ͨ��
void NRF_RX_MODE(unsigned char chanal)
{
	GPIO_ResetBits(GPIOB,NRF_CE_GPIO);
	
	NRF_WRITE_BUF(RX_ADDR_P0+NRF_WRITE_REG,NRF_RX_ADDR,5);//����ͨ��0�ĵ�ַ
	
	NRF_REG_WRITE_BYTE(EN_AA+NRF_WRITE_REG,0X01);//ʹ��ͨ��0���Զ�Ӧ��
	
	NRF_REG_WRITE_BYTE(EN_RXADDR+NRF_WRITE_REG,0X01);//ʹ��ͨ��0�Ľ��յ�ַ
	
	NRF_REG_WRITE_BYTE(RF_CH+NRF_WRITE_REG,chanal);	//����RFͨ��Ƶ��
	
	NRF_REG_WRITE_BYTE(RX_PW_P0+NRF_WRITE_REG,32);//ͨ��0����Ч���ݿ��Ϊ32byte
	
	NRF_REG_WRITE_BYTE(RF_SETUP+NRF_WRITE_REG,0X0f);//����TX�������
	
	NRF_REG_WRITE_BYTE(CONFIG+NRF_WRITE_REG,0x0f);//���üĴ���������ģʽ�������ж�
	
	GPIO_SetBits(GPIOB,NRF_CE_GPIO);//���� CE �������ģʽ
}

//���� NRF �����뷢��ģʽ
//����ͨ��Ƶ��
//ͨ���ı�Ƶ�����벻ͬ�Ĵӻ�ͨ��

void NRF_TX_MODE(unsigned char chanal)
{
	GPIO_ResetBits(GPIOB,NRF_CE_GPIO);
	
	NRF_WRITE_BUF(TX_ADDR+NRF_WRITE_REG,NRF_TX_ADDR,5);//���÷����ַ
	
	NRF_WRITE_BUF(RX_ADDR_P0+NRF_WRITE_REG,NRF_RX_ADDR,5);//���ý��յ�ַ�������Զ�Ӧ��ģʽ
	
	NRF_REG_WRITE_BYTE(EN_AA+NRF_WRITE_REG,0X01);//ʹ���Զ�Ӧ��
	
	NRF_REG_WRITE_BYTE(EN_RXADDR+NRF_WRITE_REG,0X01);//ʹ�ܽ��յ�ַ
	
	NRF_REG_WRITE_BYTE(SETUP_RETR+NRF_WRITE_REG,0X00);//��ʹ���Զ��ط�
	
	NRF_REG_WRITE_BYTE(RF_CH+NRF_WRITE_REG,chanal);//����ͨ��Ƶ��
	
	NRF_REG_WRITE_BYTE(RF_SETUP+NRF_WRITE_REG,0X0f);//���÷������
	
	NRF_REG_WRITE_BYTE(RX_PW_P0+NRF_WRITE_REG,32);//������Ч���ݿ��
	
	NRF_REG_WRITE_BYTE(CONFIG+NRF_WRITE_REG,0X0E);//���û�����������
	
}
//NRF��������
int NRF_WRITE_TX_DATA(char *buf)
{
	unsigned char state;
	int ret_val = 0;
	GPIO_ResetBits(GPIOB,NRF_CE_GPIO);//�������ģʽ
	
	NRF_WRITE_BUF(WR_TX_PLOAD,buf,8);

	GPIO_SetBits(GPIOB,NRF_CE_GPIO);	//���뷢��ģʽ

//	while(NRF_CE_IRQ_FLAG == 0);
//	NRF_CE_IRQ_FLAG = 0;
	delay(10);
	
	GPIO_ResetBits(GPIOB,NRF_CE_GPIO);
	
	state = NRF_REG_READ_BYTE(STATUS+NRF_READ_REG);
	
	if(state&TX_DS)
	{
		ret_val = 1;
	}
	
	NRF_REG_WRITE_BYTE(STATUS+NRF_WRITE_REG,state);//�����־
	
	NRF_REG_WRITE_BYTE(FLUSH_TX,0XFF);
	return ret_val;
}


//�����������е�����
char NRF_READ_RX_DATA(char *buf)
{
	unsigned char state;
	
	GPIO_ResetBits(GPIOB,NRF_CE_GPIO);//��������ģʽ
	
	state = NRF_REG_READ_BYTE(STATUS+NRF_READ_REG);
	
	NRF_REG_WRITE_BYTE(STATUS+NRF_WRITE_REG,state);//�����־
	
	if(state&RX_DR)
	{
		NRF_READ_BUF(RD_RX_PLOAD,buf,8);
		NRF_REG_WRITE_BYTE(FLUSH_RX,0XFF);
		return 1;
	}
	else
		return 0;
}

//��Ҫ����NRF��MCU�Ƿ���������
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
		return SUCCESS ;        //MCU��NRF�ɹ����� 
	else
		return ERROR ;        //MCU��NRF����������
}

//NRF�ĳ�ʼ��
void NRF_mode_init(void)
{
	GPIO_ResetBits(GPIOB,NRF_CE_GPIO);
	
	NRF_WRITE_BUF(RX_ADDR_P0+NRF_WRITE_REG,NRF_RX_ADDR,5);//����ͨ��0�ĵ�ַ
	
	NRF_REG_WRITE_BYTE(EN_AA+NRF_WRITE_REG,0X01);//ʹ��ͨ��0���Զ�Ӧ��
	
	NRF_REG_WRITE_BYTE(EN_RXADDR+NRF_WRITE_REG,0X01);//ʹ��ͨ���Ľ��յ�ַ
		
	NRF_REG_WRITE_BYTE(RX_PW_P0+NRF_WRITE_REG,8);//ͨ��0����Ч���ݿ��Ϊ32byte
	
	NRF_REG_WRITE_BYTE(RF_SETUP+NRF_WRITE_REG,0X0f);//����TX�������
	

	NRF_WRITE_BUF(TX_ADDR+NRF_WRITE_REG,NRF_TX_ADDR,5);//���÷����ַ
	
	NRF_REG_WRITE_BYTE(SETUP_RETR+NRF_WRITE_REG,0X00);//��ʹ���Զ��ط�
}









