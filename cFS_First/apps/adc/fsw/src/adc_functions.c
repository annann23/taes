/*
 * ADCS.c
 *
 *  Created on: 2018. 9. 18.
 *      Author: Shinhye
 */


#include "adc_functions.h"


void ADCS_Sensor_Signal_Output(char Sensor)
{
	switch (Sensor)
	{
	case IMU:
		int Status;
		Status=IMU_lib('IMU_TEST');
		CFE_TBL_Load(Status);  // Table에 측정 데이터 저장 또는 로그에서 활용?
		LOG(4, "%s\n", "ADCS IMU test signal done");
	case MMT:
		int Status;
		Status=MMT_lib('MMT_TEST');
		CFE_TBL_Load(Status);
		LOG(4, "%s\n", "ADCS MMT test signal done");
	case FSS:
		int Status;
		Status=FSS_lib('FSS_TEST');
		CFE_TBL_Load(Status);
		LOG(4, "%s\n", "ADCS FSS test signal done");
	case CSS:
		int Status;
		Status=FSS_lib('CSS_TEST');
		CFE_TBL_Load(Status);
		LOG(4, "%s\n", "ADCS CSS test signal done");
	}
}

void ADCS_Initial_AD(void){
	int x, y;
	x=FSS_lib('FSS_AD');
	y=CSS_lib('CSS_TEST');
	AD_Function(x,y);
}

double ADCS_AngularVelocity(void){
	double omega;
	omega=IMU_lib('IMU_AngularVelocity');
	return omega;
}

double ADCS_AngleError(char object){
	double angle_error;
	//ADCS 태양지향 or 지구지향 각도 에러 측정하여 return
	return angle_error;
}

void ADCS_AD(void){
	//궤도 결정 알고리즘
}


