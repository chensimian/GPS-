#include "stm32f10x.h"
#include "timer.h"
#include "USART.h"
#include "GPS.h"
#include "spi.h"
#include "gps_convert_xyz.h"
#include "NL24L01.h"

char temp_gpsdata[73]="";//һ�����ڽ��գ�һ�����ڴ�����ֹ���ݱ����ǣ�
char gps_data[73]="$GPGLL,2160.7668,N,11027.7624,E,092321.00,A,A*60";
char i = 0;//���ڼ���
char USART_RX_OK_FLAG = 0;	//����һ֡������ɵı�־
char NRF_RX_OK = 0;//NRF������ɱ�־
unsigned char timecount = 0;//ʱ�����

extern unsigned char NRF_RX_ADDR[5];	//ͨ��0�Ľ��յ�ַ
extern unsigned char NRF_TX_ADDR[5];	//ͨ��0�ķ����ַ

extern unsigned char CHANAL1;		//�ӻ� 1 ͨ��Ƶ����Ƶ�ʣ�
extern unsigned char CHANAL2;		//�ӻ� 2 ͨ��Ƶ����Ƶ�ʣ�
extern unsigned char CHANAL3;	//�ӻ� 3 ͨ��Ƶ����Ƶ�ʣ�
extern unsigned char NRF_CE_IRQ_FLAG;//NRF�жϱ�־

//��ȡ�����GPS��Ϣ
void get_GPS(char buf[]);

int main(void)
{
	char Tx_buf[8]={0,0,0,0,0,0,0,0};
	char tmp_rx_buf[8];
		
//��ʼ��ʱ��	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	timer1_configuration();
//��ʼ�� USART
	USART_config();
	USART_Cmd(USART2,ENABLE);
//��ʼ�� SPI
	SPI_configuration();
	
	//���NRF���������
	if(NRF_Check())
		printf("\r\nNRF connet SUCCES!\r\n");
	else
		printf("\r\nNRF connet ERROR\r\n");
	//��ʼ��NRF
	NRF_mode_init();
	//����ͨ��Ƶ��
	NRF_REG_WRITE_BYTE(RF_CH+NRF_WRITE_REG,CHANAL1);	
	//���üĴ���������ģʽ�������ж�
	NRF_REG_WRITE_BYTE(CONFIG+NRF_WRITE_REG,0x1B);
	//������յȴ�
	GPIO_SetBits(GPIOB,NRF_CE_GPIO);	
	while(1)
	{	
		if(NRF_RX_OK)
		{
			
			NRF_READ_RX_DATA(tmp_rx_buf);	//��ȡNRF������
			NRF_CE_IRQ_FLAG = 1;
			if(strncmp(tmp_rx_buf,"gps",3) == 0)//�ж������Ƿ�Ҫ��ȡGPS��Ϣ
			{
				NRF_REG_WRITE_BYTE(CONFIG+NRF_WRITE_REG,0X1A);//���÷��������������
				
				get_GPS(Tx_buf);
				//������Ч��Ҫ�ز�
				
				//��NRF���ͻ�����д�����ݲ�����
				while(1)
				{
					if(NRF_WRITE_TX_DATA(Tx_buf))
						break;
				}				
				
				//���óɽ���ģʽ
				NRF_REG_WRITE_BYTE(CONFIG+NRF_WRITE_REG,0x1B);
				//������յȴ�
				GPIO_SetBits(GPIOB,NRF_CE_GPIO);
			}
			else
			{
				GPIO_SetBits(GPIOB,NRF_CE_GPIO);//�������ģʽ
			}
			NRF_RX_OK = 0;
		}
	}
}

//�ú����� bug ����Ҫ�����ݵ���Ч�Խ����жϣ���Ч���ز�
//��ȡ�����GPS��Ϣ
void get_GPS(char buf[8])
{
	char ii=0;
	char jj=0;
	int xy[2] = {0,0};
	unsigned char flag=0;
	USART_Cmd(USART1,ENABLE);//ʹ�ܴ��ڽ���GPS��Ϣ
	while(1)
	{
		if(USART_RX_OK_FLAG)//�ж��Ƿ������һ֡GPS����
		{
			USART_RX_OK_FLAG = 0;	//���������ɱ�־��
			strcpy(gps_data,temp_gpsdata);//�������յ�������
			flag = gps2xyz(xy);	//����ī���н����յ� gps ��ת���� xy ����
			printf("x=%d,y=%d\n",xy[0],xy[1]);
			jj = 0;
			//���������뷢������
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


