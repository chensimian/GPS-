#include "stm32f10x.h"
#include "timer.h"
#include "USART.h"
#include "spi.h"
#include "NL24L01.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

#define light_speed			2.99792458	// ����Ϊ 299792458m/s.��λ��m/s
#define pi				3.14159265358979323846
#define EARTH_R 	6371393	 //����뾶��λ����

extern char NRF_RX_ADDR[5];	//ͨ��0�Ľ��յ�ַ
extern char NRF_TX_ADDR[5];	//ͨ��0�ķ����ַ

const unsigned char CHANAL[3] = {30,70,100};	//ͨ��Ƶ��

unsigned int timecount = 0;	//����ʱ���ʱ
extern unsigned char NRF_CE_IRQ_FLAG;	//NRF24L01���жϱ�־

unsigned char NRF_Tx_data_OK = 0;	//NRF24L01������ɵı�־

char USART_RX_OK_FLAG = 0;	//����һ֡������ɵı�־


void my_delay(int time);	//��ʱ����
void measur_distan(unsigned int distance[3]);	//��������������ӻ��ľ���
void get_GPS(double xy[3][2]);	//��ȡÿ���ӻ���GPS��Ϣ
void calculat_GPS(unsigned int distance[3],double xy[3][2], double target_GPS[2],char target_lat_lon[2]);//����������GPS��Ϣ


int main(void)
{
	unsigned int distance[3] = {0,0,0};
	double xy[3][2] = {{0,0},{0,0},{0,0}};
	double target_GPS[2];
	char target_lat_lon[2] = {0,0};	//���յľ�γ�ȣ�N��S��W��E	
	
	char *buf = (char* )malloc(32);
	memset(buf,1,32);

	//�����ж����ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	// TIM2 ��ʼ��
	timer1_configuration();
	// USART ��ʼ��
	USART_config();
	USART_Cmd(USART1,ENABLE);
	USART_Cmd(USART2,ENABLE);
	// SPI ��ʼ��
	SPI_configuration();
	
	//���NRF24L01���������
	if(NRF_Check())
		printf("\r\nconnet nrf succes\r\n");
	else
		printf("\r\nconnet nrf error\r\n");
	//��ʼ��NRF24L01
	NRF_mode_init();
	
	while(1)
	{
		//��������
		measur_distan(distance);
		//��ȡ�ӻ�GPS
		get_GPS(xy);
		//��������GPS
		calculat_GPS(distance,xy,target_GPS,target_lat_lon);
		//��ӡ�����Ϣ
		printf("\r\n�ӻ�1�ľ��룺%d ��\r\n",distance[0]);
		printf("\r\n�ӻ�2�ľ��룺%d ��\r\n",distance[1]);
		printf("\r\n�ӻ�3�ľ��룺%d ��\r\n",distance[2]);
		printf("\r\n������GPS��%c %f , %c %f\r\n",target_lat_lon[0],target_GPS[0],target_lat_lon[1],target_GPS[1]);
		printf("\r\n*******************END*********************\r\n");
		//��ʱ
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

	
	NRF_REG_WRITE_BYTE(CONFIG+NRF_WRITE_REG,0X1A);//���÷���ģʽ������������
	for(k=0;k<3;k++)
	{
		NRF_REG_WRITE_BYTE(RF_CH+NRF_WRITE_REG,CHANAL[k]);//����ͨ��Ƶ��
		GPIO_ResetBits(GPIOB,NRF_CE_GPIO);//NRF�������ģʽ
		
		while(i<250)	//��������250������
		{
			
			while(1)
			{
				NRF_WRITE_BUF(WR_TX_PLOAD,buf,8);	//��NRFд������
			
				TIM_SetCounter(TIM2,0);		//���ʱ��
				
				GPIOB->BSRR = NRF_CE_GPIO;	//���뷢��ģʽ
				
				TIM2->CR1 |= TIM_CR1_CEN;	//ʹ��ʱ��
				
				my_delay(1);	
				
				GPIO_ResetBits(GPIOB,NRF_CE_GPIO);	//NRF��������ģʽ
			
				state = NRF_REG_READ_BYTE(STATUS+NRF_READ_REG);	//��ȡNRF��־λ
				NRF_REG_WRITE_BYTE(STATUS+NRF_WRITE_REG,state);//�����־λ
			
				NRF_REG_WRITE_BYTE(FLUSH_TX,0XFF);	//���NRF���ͻ�����
				if(state&TX_DS)	//�ж��Ƿ��ܵ��ӻ���Ӧ���ź�
					break;
			}
			
			//���ܵ����ֵ
			time[k] += (timecount*900000 + TIM_GetCounter(TIM2) * 13.88888888889);//����TOFʱ�䣨������ʱ��
			
			timecount = 0;//��0ʱ�Ӽ�����
			i++;
		}
		i = 0;
		time[k] = time[k] / 250;	//ȡ����ʱ���ƽ��ֵ
		time[k] = abs((time[k] - handle_time))/2;//��ȥ��ʱʱ�䣬�����TOF����������ʱ��
		distance[k] = time[k] * light_speed / 10; //�������ӻ�֮��ľ��롣��λ��m
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
		//���Ͷ�ȡ�ź�
		NRF_REG_WRITE_BYTE(CONFIG+NRF_WRITE_REG,0X1A);//���÷��������������
		NRF_REG_WRITE_BYTE(RF_CH+NRF_WRITE_REG,CHANAL[k]);//����ͨ��Ƶ��
		
		while(1)
		{
			
			NRF_WRITE_BUF(WR_TX_PLOAD,Tx_buf,8);//��NRF���ͻ�����д������
			
			GPIO_SetBits(GPIOB,NRF_CE_GPIO);	//���뷢��ģʽ
			
			my_delay(5);
			
			GPIO_ResetBits(GPIOB,NRF_CE_GPIO);	//��������ģʽ
		
			state = NRF_REG_READ_BYTE(STATUS+NRF_READ_REG);//��ȡ��־λ
			NRF_REG_WRITE_BYTE(STATUS+NRF_WRITE_REG,state);//�����־λ
		
			NRF_REG_WRITE_BYTE(FLUSH_TX,0XFF);//������ͻ�����
			if(state&TX_DS)
				break;
		}
		NRF_CE_IRQ_FLAG = 0;
		//��ȡ��Ϣ	
		NRF_REG_WRITE_BYTE(CONFIG+NRF_WRITE_REG,0x1B);//���üĴ���������ģʽ�������ж�
		GPIO_SetBits(GPIOB,NRF_CE_GPIO);
		NRF_READ_RX_DATA(Rx_buf);//�ȴ������մӻ���GPS��Ϣ
		
		//�洢GPS��Ϣ
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
	//���� x ����
	target_xy[0] = (xy[0][0]+xy[1][0]+xy[2][0])/3;
	//���� y ����
	target_xy[1] = (xy[0][1]+xy[1][1]+xy[2][1])/3;
	//���� GPS
	target_GPS[0] = target_xy[0] / pi*180;	//����
	target_GPS[1] = (2 * (atanf(expf(target_xy[1])) - pi/4)) / pi*180;//γ��
	
	//�����ϱ�γ�Ͷ�����
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
