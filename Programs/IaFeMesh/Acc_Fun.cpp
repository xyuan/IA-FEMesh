#include "Acc_Fun.h"
Acc_Fun::Acc_Fun()
{
}

Acc_Fun::~Acc_Fun()
{
}

void Acc_Fun:: Calc_Line_Angle(float pt1[3], float pt2[3], float& theta)
{
	float pi = 3.1416;
	double tol = 1e-3;
	float slope;
	if(fabs(pt2[1] -pt1[1])>=tol) 
	{slope = (pt2[2] - pt1[2])/(pt2[1] - pt1[1]);}
	else slope = 1e6;
	float temp = fabs(pt2[1] - pt1[1]);	
	float line_length = sqrt(pow(pt2[1] - pt1[1],2.0)+
		pow(pt2[2] - pt1[2],2.0));
	if(pt2[1] < pt1[1] && pt2[2] >=pt1[2]) theta = pi - acos(temp/line_length);
	if(pt2[1] < pt1[1] && pt2[2]  < pt1[2]) theta = pi + acos(temp/line_length);
	if(pt2[1] >= pt1[1] && pt2[2] < pt1[2]) theta = 2.0*pi - acos(temp/line_length);
	if(pt2[1] >= pt1[1] && pt2[2] >=pt1[2]) theta = acos(temp/line_length);
	theta = theta*180.0/pi;		
}

void Acc_Fun:: Intersect_Circle_Line(float cir_cen[3], float pt1[3], float pt2[3], float int_poi[3], float& rad)
{
	float C, A, B;
	A = pow(pt2[1]-pt1[1],2.0) + pow(pt2[2]-pt1[2],2.0);
	B = (pt2[1]-pt1[1])*(pt1[1]-cir_cen[1]) + 
		(pt2[2]-pt1[2])*(pt1[2]-cir_cen[2]);
	C = cir_cen[1]*cir_cen[1] + cir_cen[2]*cir_cen[2] +
		pt1[1]*pt1[1] + pt1[2]*pt1[2] - 
		2.0*(cir_cen[1]*pt1[1] + cir_cen[2]*pt1[2]) - rad*rad;
	float Soln[3];
	Soln[1] = (-B + sqrt(B*B - A*C))/A;
	int_poi[1] = pt1[1] + Soln[1]*(pt2[1] - pt1[1]);
	int_poi[2] = pt1[2] + Soln[1]*(pt2[2] - pt1[2]);
}

void Acc_Fun::Calc_Normal_Line(float pt1[3], float pt2[3], float normal[3])
{
	normal[0] = 0;
	float pi = 3.1416;
	double tol = 1e-3;
	float slope;
	if(abs(pt2[1] -pt1[1])>=tol) 
	{slope = (pt2[2] - pt1[2])/(pt2[1] - pt1[1]);}
	else slope = 1e6;
	float temp = fabs(pt2[1] - pt1[1]);	
	float line_length = sqrt(pow(pt2[1] - pt1[1],2.0)+
		pow(pt2[2] - pt1[2],2.0));
	float theta;
	if(pt2[1] < pt1[1] && pt2[2] >=pt1[2]) theta = pi - acos(temp/line_length);
	if(pt2[1] < pt1[1] && pt2[2]  < pt1[2]) theta = pi + acos(temp/line_length);
	if(pt2[1] >= pt1[1] && pt2[2] < pt1[2]) theta = -acos(temp/line_length);
	if(pt2[1] >= pt1[1] && pt2[2] >=pt1[2]) theta = acos(temp/line_length);
	
	normal[1] = cos(theta+pi/2.0);
	normal[2] = sin(theta+pi/2.0);
			
}

int Acc_Fun::Calc_Line_Line_Intersect(float pt1_Bot[3], float pt2_Bot[3],
							  float pt1_Top[3], float pt2_Top[3], float pt_int[3])
{
	float slope_bot,slope_top;
	double tol = 1e-3;
	pt_int[0] = 0;
	bool intersection = false;
	if(fabs(pt2_Bot[1] - pt1_Bot[1]) >= tol)
	{
		slope_bot = (pt2_Bot[2] - pt1_Bot[2])/(pt2_Bot[1] - (pt1_Bot[1]));			
	}
	else slope_bot = 1e6;
	
	if(fabs(pt2_Top[1] - pt1_Top[1]) >= tol)
	{
		slope_top = (pt2_Top[2] - pt1_Top[2])/(pt2_Top[1] - (pt1_Top[1]));			
	}
	else slope_top = 1e6;
	
	if(slope_bot ==1e6 && slope_top != 1e6)
	{
		pt_int[1] = pt1_Bot[1];
		pt_int[2] = slope_top*pt_int[1] + pt1_Top[2] + pt1_Top[1]*slope_top;
	}
	else
	{
		if(slope_top ==1e6 && slope_bot != 1e6)
		{
			pt_int[1] = pt1_Top[1];
			pt_int[2] = slope_bot*pt_int[1] + pt1_Bot[2] + pt1_Bot[1]*slope_bot;			
		}
		else
		{
			if(slope_top ==1e6 && slope_bot == 1e6)
			{
				pt_int[1] = pt1_Bot[1];
				pt_int[2] = (pt1_Top[2] + pt2_Top[2])/2.0;
			}
			else
			{
				pt_int[1] = ((pt1_Top[2]-pt1_Top[1]*slope_top)-
					(pt1_Bot[2]-pt1_Bot[1]*slope_bot))/(slope_bot-slope_top);
				pt_int[2] = slope_bot*pt_int[1] + pt1_Bot[2]-pt1_Bot[1]*slope_bot;
			}
		}
	}

	if((pt2_Top[1] - pt1_Top[1]) >= 0 && (pt2_Top[2] -pt1_Top[2])>= 0) 
	{
		if((pt_int[1] - pt1_Top[1] >= 0) && (pt_int[1] -pt2_Top[1] <=0)
			&&(pt_int[2] - pt1_Top[2] >= 0) && (pt_int[2] - pt2_Top[2] <=0))
			intersection =true;
	}

	if(pt2_Top[1] < pt1_Top[1] && pt2_Top[2]  < pt1_Top[2])
	{
		if((pt_int[1] - pt1_Top[1] <= 0) && (pt_int[1] >=pt2_Top[1])
			&&(pt_int[2] <= pt1_Top[2]) && (pt_int[2] >= pt2_Top[2]))
			intersection =true;
	}
	
	if(pt2_Top[1] >= pt1_Top[1] && pt2_Top[2] < pt1_Top[2])
	{
		if((pt_int[1] >= pt1_Top[1]) && (pt_int[1] <=pt2_Top[1])
			&&(pt_int[2] <= pt1_Top[2]) && (pt_int[2] >= pt2_Top[2]))
			intersection =true;
	}
	if(pt2_Top[1] < pt1_Top[1] && pt2_Top[2] >=pt1_Top[2])
	{
		if((pt_int[1] <= pt1_Top[1]) && (pt_int[1] >=pt2_Top[1])
			&&(pt_int[2] >= pt1_Top[2]) && (pt_int[2] <= pt2_Top[2]))
			intersection =true;
	}
	float dist1 = sqrt(pow(pt_int[1]-pt1_Top[1],2.0) + 
		pow(pt_int[2]-pt1_Top[2],2.0));
	float dist2 = sqrt(pow(pt_int[1]-pt2_Top[1],2.0) + 
		pow(pt_int[2]-pt2_Top[2],2.0));
	if(dist1 <=0.2) intersection = true;
	if(dist2<=0.2) intersection = true;
	if(slope_top != slope_bot)
	{if(intersection) return 1;
	else return 0;}
	else return 0;
	
}

void Acc_Fun:: Calc_Line_Dir(float pt1[3], float pt2[3], float dir[3])
{
	float length = sqrt(pow(pt1[0]-pt2[0],2.0) + 
		pow(pt1[1]-pt2[1],2.0) + pow(pt1[2]-pt2[2],2.0));
	dir[0] = (pt2[0] - pt1[0])/length;
	dir[1] = (pt2[1] - pt1[1])/length;
	dir[2] = (pt2[2] - pt1[2])/length;
}

void Acc_Fun::Calc_Distance(float pt1[3],float pt2[3],float& length)
{
		length = sqrt(pow(pt1[0]-pt2[0],2.0) + 
				pow(pt1[1]-pt2[1],2.0) + pow(pt1[2]-pt2[2],2.0));

}

void Acc_Fun::Rotation_X(float vector[3], float deg, float ans[3])
{
	deg = deg*3.1416/180.0;
	ans[0] = vector[0];
	ans[1] = cos(deg)*vector[1] - sin(deg)*(vector[2]);
	ans[2] = sin(deg)*vector[1] + cos(deg)*(vector[2]);
}

void Acc_Fun::Rotation_Y(float vector[3], float deg, float ans[3])
{
	deg = deg*3.1416/180.0;
	ans[1] = vector[1];
	ans[0] = cos(deg)*vector[0] - sin(deg)*(vector[2]);
	ans[2] = sin(deg)*vector[0] + cos(deg)*(vector[2]);
}

void Acc_Fun::Rotation_Z(float vector[3], float deg, float ans[3])
{
	deg = deg*3.1416/180.0;
	ans[2] = vector[2];
	ans[0] = cos(deg)*vector[0] - sin(deg)*(vector[1]);
	ans[1] = sin(deg)*vector[0] + cos(deg)*(vector[1]);
}

void Acc_Fun:: Calc_Line_Angle_ZX(float pt1[3], float pt2[3], float& theta)
{
	float pi = 3.1416;
	double tol = 1e-3;
	float slope;
	if(fabs(pt2[2] -pt1[2])>=tol) 
	{slope = (pt2[0] - pt1[0])/(pt2[2] - pt1[2]);}
	else slope = 1e6;
	float temp = fabs(pt2[0] - pt1[0]);	
	float line_length = sqrt(pow(pt2[0] - pt1[0],2.0)+
		pow(pt2[2] - pt1[2],2.0));
	if(pt2[2] < pt1[2] && pt2[0] >=pt1[0]) theta = pi - acos(temp/line_length);
	if(pt2[2] < pt1[2] && pt2[0]  < pt1[0]) theta = pi + acos(temp/line_length);
	if(pt2[2] >= pt1[2] && pt2[0] < pt1[0]) theta = 2.0*pi - acos(temp/line_length);
	if(pt2[2] >= pt1[2] && pt2[0] >=pt1[0]) theta = acos(temp/line_length);
	theta = theta*180.0/pi;		
}

void Acc_Fun:: Rotate_Bounding_Box(float XYZ[6],int axis,float degree,float XYZr[6])
{
	float pt1[3],pt2[3],pt3[3],pt4[3],pt5[3],pt6[3],pt7[3],pt8[3];
	float pt1r[3],pt2r[3],pt3r[3],pt4r[3],pt5r[3],pt6r[3],pt7r[3],pt8r[3];
	pt1[0] = XYZ[0];	pt1[1] = XYZ[2];	pt1[2] = XYZ[4];
	pt2[0] = XYZ[1];	pt2[1] = XYZ[2];	pt2[2] = XYZ[4];
	pt3[0] = XYZ[1];	pt3[1] = XYZ[3];	pt3[2] = XYZ[4];
	pt4[0] = XYZ[0];	pt4[1] = XYZ[3];	pt4[2] = XYZ[4];
	pt5[0] = XYZ[0];	pt5[1] = XYZ[2];	pt5[2] = XYZ[5];
	pt6[0] = XYZ[1];	pt6[1] = XYZ[2];	pt6[2] = XYZ[5];
	pt7[0] = XYZ[1];	pt7[1] = XYZ[3];	pt7[2] = XYZ[5];
	pt8[0] = XYZ[0];	pt8[1] = XYZ[3];	pt8[2] = XYZ[5];

	if(axis == 1)
	{
		Rotation_X(pt1,degree,pt1r);
		Rotation_X(pt2,degree,pt2r);
		Rotation_X(pt3,degree,pt3r);
		Rotation_X(pt4,degree,pt4r);
		Rotation_X(pt5,degree,pt5r);
		Rotation_X(pt6,degree,pt6r);
		Rotation_X(pt7,degree,pt7r);
		Rotation_X(pt8,degree,pt8r);
	}

	if(axis == 2)
	{
		Rotation_Y(pt1,degree,pt1r);
		Rotation_Y(pt2,degree,pt2r);
		Rotation_Y(pt3,degree,pt3r);
		Rotation_Y(pt4,degree,pt4r);
		Rotation_Y(pt5,degree,pt5r);
		Rotation_Y(pt6,degree,pt6r);
		Rotation_Y(pt7,degree,pt7r);
		Rotation_Y(pt8,degree,pt8r);
	}

	if(axis == 3)
	{
		Rotation_Z(pt1,degree,pt1r);
		Rotation_Z(pt2,degree,pt2r);
		Rotation_Z(pt3,degree,pt3r);
		Rotation_Z(pt4,degree,pt4r);
		Rotation_Z(pt5,degree,pt5r);
		Rotation_Z(pt6,degree,pt6r);
		Rotation_Z(pt7,degree,pt7r);
		Rotation_Z(pt8,degree,pt8r);
	}

	for(int i=1; i <=3; i++)
	{
		float temp[8];
		temp[0] = pt1r[i-1];temp[1] = pt2r[i-1];temp[2] = pt3r[i-1];temp[3] = pt4r[i-1];
		temp[4] = pt5r[i-1];temp[5] = pt6r[i-1];temp[6] = pt7r[i-1];temp[7] = pt8r[i-1];
		XYZr[2*i-2] = temp[0]; XYZr[2*i-1] = temp[0];
		for(int j=1; j<8; j++)
		{
			if((temp[j]) < XYZr[2*i-2]) XYZr[2*i-2] = temp[j];
			if((temp[j]) > XYZr[2*i-1]) XYZr[2*i-1] = temp[j];
		}	
	}
}