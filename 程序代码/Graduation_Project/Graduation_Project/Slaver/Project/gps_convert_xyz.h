#ifndef _GPS_CONVERT_XYZ_H_
#define _GPS_CONVERT_XYZ_H_

#include "GPS.h"
#include <math.h>
#include <stdlib.h>

#define pi				 3.14159265358979323846
#define EARTH_R 	6371393	 //地球半径单位：米

unsigned char gps2xyz(int xy[2]);

#endif
