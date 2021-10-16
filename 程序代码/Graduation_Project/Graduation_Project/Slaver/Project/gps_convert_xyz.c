#include "gps_convert_xyz.h"
#include "stdio.h"

//将GPS信息转换成 xy 坐标
unsigned char gps2xyz(int xy[2])
{
	float num[3];
	char str[4];
	
	GPS_data_process(num,str);//获取 gps 信息
	if(str[2] == 'V'|| str[3] == 'N')	//判断定位数据的有效性
		return 0;
	else
	{
		//判断南北纬，北为正，南为负
		if(str[0] == 'N'){
			
		}
		else{
			num[0] = -num[0];
		}
		//判断东西经，东为正，西为负
		if(str[1] == 'E'){
			
		}
		else{
			num[1] = -num[1];
		}
		//利用墨卡托进行坐标转换
		xy[0] = ((num[1] * pi/180)*10000000.0);	//弧度,保留七位小数
		xy[1] = (logf(tanf(pi/4 + (num[0] * pi/180)/2))*10000000.0); //保留七位小数
		return 1;
	}
}
