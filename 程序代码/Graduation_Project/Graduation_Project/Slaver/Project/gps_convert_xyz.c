#include "gps_convert_xyz.h"
#include "stdio.h"

//��GPS��Ϣת���� xy ����
unsigned char gps2xyz(int xy[2])
{
	float num[3];
	char str[4];
	
	GPS_data_process(num,str);//��ȡ gps ��Ϣ
	if(str[2] == 'V'|| str[3] == 'N')	//�ж϶�λ���ݵ���Ч��
		return 0;
	else
	{
		//�ж��ϱ�γ����Ϊ������Ϊ��
		if(str[0] == 'N'){
			
		}
		else{
			num[0] = -num[0];
		}
		//�ж϶���������Ϊ������Ϊ��
		if(str[1] == 'E'){
			
		}
		else{
			num[1] = -num[1];
		}
		//����ī���н�������ת��
		xy[0] = ((num[1] * pi/180)*10000000.0);	//����,������λС��
		xy[1] = (logf(tanf(pi/4 + (num[0] * pi/180)/2))*10000000.0); //������λС��
		return 1;
	}
}
