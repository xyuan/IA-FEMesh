//#include "stdafx.h"
#ifndef __Acc_Fun_h
#define __Acc_Fun_h
#include <math.h>
class Acc_Fun
{
public:
	Acc_Fun();
	~Acc_Fun();
	void Calc_Line_Angle(float[3],float[3],float&);
	void Calc_Line_Angle_ZX(float[3],float[3],float&);
	void Intersect_Circle_Line(float[3], float[3],float[3],float[3], float&);
	void Calc_Normal_Line(float[3], float[3],float[3]);
	int Calc_Line_Line_Intersect(float[3], float[3], float[3], float[3], float[3]);
	void Calc_Line_Dir(float[3], float[3], float[3]);
	void Calc_Distance(float[3],float[3],float&);
	void Rotation_X(float[3],float,float[3]);
	void Rotation_Y(float[3],float,float[3]);
	void Rotation_Z(float[3],float,float[3]);
	void Rotate_Bounding_Box(float[6], int ,float,float[6]);
private:
};
#endif 