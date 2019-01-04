/*
 * CNDH.c
 *
 *  Created on: 2018. 9. 18.
 *      Author: Shinhye
 */

#include "cndh_functions.h"


void CNDH_BaseSensor_Test(void)
{
	//기초 센서 테스트시 실패 했을 때 어떻게 재시도 할 것인지, 어느정도 할 것인지는 아직 확정 되지 않았다. 일단은 10번 재시도라고 설정
	//OBC와 EPS의 온도, 전압, 전류를 체크 하고 배터리 충전 상태를 체크한다.
	int Status=0;
	for (int i=0;i<10;i++)
	{
		Status=CNDH_OBC_Test('Temp');
		if (Status=!0)
			LOG(2, "%s %d\n", "OBC temp test done. Count", i);
			break;
	}
	int Status=0;
	for (int i=0;i<10;i++)
	{
		Status=CNDH_OBC_Test('Voltage');
		if (Status=!0)
			LOG(2, "%s %d\n", "OBC voltage test done. Count", i);
			break;
	}
	int Status=0;
	for (int i=0;i<10;i++)
	{
		Status=CNDH_OBC_Test('Current');
		if (Status=!0)
			LOG(2, "%s %d\n", "OBC current test done. Count", i);
			break;
	}

	int Status=0;
	for (int i=0;i<10;i++)
	{
		Status=EPS_Test('Temp');
		if (Status=!0)
			LOG(2, "%s %d\n", "EPS temp test done. Count", i);
			break;
	}

	int Status=0;
	for (int i=0;i<10;i++)
	{
		Status=EPS_Test('Voltage');
		if (Status=!0)
			LOG(2, "%s %d\n", "EPS voltage test done. Count", i);
			break;
	}

	int Status=0;
	for (int i=0;i<10;i++)
	{
		Status=EPS_Test('Current');
		if (Status=!0)
			LOG(2, "%s %d\n", "EPS current test done. Count", i);
			break;
	}

	int Status=0;
	for (int i=0;i<10;i++)
	{
		Status=EPS_Test('Bat status');
		if (Status=!0)
			LOG(2, "%s %d\n", "EPS BAT charge status. Status", i);
			break;
	}
	LOG(3, "%s\n", "CNDH base sensor test end");
	}

void CNDH_Signal_Test(char subsystem, char sensor)
{
	switch (subsystem)
	{
	case ADCS:
		ADCS_Sensor_Signal_Output(sensor);
		CNDH_Checksum(sensor);
		LOG(2, "%s %s %d \n", sensor, "signal test done");
	case COMS:
		COMS_Sensor_Signal_Output(sensor);
		CNDH_Checksum(sensor);
		LOG(2, "%s %s %d \n", sensor, "signal test done");
	}

}

void CNDH_Checksum(char sensor)
{
	int Status;
	Status=CFE_SB_RcvMsg(sensor);
	//checksum 수행
	LOG(5, "%s %s %d \n", sensor, "checksum result", Status);
}

void CNDH_HK_Monitoring(void)
{
	//HK 어플레케이션을 활용.
	//HK 데이터가 정상적으로 저장되었는지 점검한다.
	uint16_t cur_fss;
	uint16_t vol_fss;

	uint16_t temp_css1;
	uint16_t temp_css2;
	uint16_t temp_css3;
	uint16_t temp_css4;
	uint16_t temp_css5;

	uint16_t cur_oem;
	uint16_t vol_oem;
	uint16_t temp_oem;

	uint16_t cir_ax;
	uint16_t vol_ax;
	uint16_t temp_Ax;

	uint16_t cur_cam;
	uint16_t vol_cam;
	uint16_t temp_cam;

	uint16_t cur_obc;
	uint16_t vol_obc;
	uint16_t temp_obc;

	uint16_t cur_sa1;
	uint16_t cur_sa2;
	uint16_t cur_sa3;
	uint16_t cur_sa4;
	uint16_t cur_sa5;
	uint16_t vol_sa1;
	uint16_t vol_sa2;
	uint16_t vol_sa3;
	uint16_t vol_sa4;
	uint16_t vol_sa5;
	uint16_t temp_sa1;
	uint16_t temp_sa2;
	uint16_t temp_sa3;
	uint16_t temp_sa4;
	uint16_t temp_sa5;

	uint16_t vol_bat;
	uint16_t cur_bat;
	uint16_t temp_eps;

	double od_rx;
	double od_ry;
	double od_rz;
	double od_vx;
	double od_vy;
	double od_vz;
	double od_time;
	double ad_wx;
	double ad_wy;
	double ad_wz;
	double ad_q1;
	double ad_q2;
	double ad_q3;

}

void CNDH_UHF_Deployment(int UHF_number)
{
	int Status=0;
	int i=0;
	for (int i==0;i<10;i++)
	{
	Status=COMS_UHF_Deployment(UHF_number); //전개 완료하면 status 출력
	if (Status=0)
			break;
	UHF_lib('Wait_60s'); //전개 실패 시 1분 간격 반복
	}
}


void CNDH_Beacon_Tx(void)
{
	COMS_FSK_Modulation();
	COMS_Beacon_Tx();
}


void CNDH_BAT_Check(void)
{
	int SoC;
	SoC=EPS_Test('Bat status');
	if (SoC>80){
		EPS_BAT_Charge();
	}
}


/*카메라 촬영하는 함수. 촬영대상(지구, 태양등)을 지정 할 수 있고, 대상과 카메라의 각도차이와 촬영횟수 또한 정할 수 있다.*/
void CNDH_CAM_Shooting(char object, double condition, int picture_number)
{
	int MTQ_address;
	int CAM_address;

	EPS_Power(CAM_address, 1);
	PAY_CAM_Signal_Output();
	CNDH_Checksum('CAM');

	EPS_Power(MTQ_address, 1);

	double angle_error=ADCS_AngleError(object);

	while(angle_error>condition){
		ADCS_MTQ_MagneticField(5,5);
		angle_error=ADCS_AngleError(object);
	}

	for(int i=0;i<picture_number;i++){
		PAY_CAM_Shooting();
		PAY_CAM_Compress();
	}
	EPS_Power(CAM_address, 0); //NanoCam off
	EPS_Power(MTQ_address, 0);

}

void CNDH_GPS_TLE_Test(double *GPS_sol){
	//GPS 솔루션과 TLE기반 전파결과 온보드 비교 후 Table에 저장
	double *result;
	CFE_TBL_Load(result);
}

void CNDH_GPS_Update(void){
	//TIME Application 과 연동하여 시간 업데이트 및 궤도 전파
	double *GPS_sol[3]={0,0,0};
	*GPS_sol=GNC_GPS_Solution();
	TIME_Wakeup(GPS_sol);
	GNC_GPS_OD(GPS_sol);
}

void CNDH_ISL(char STATE){
	//STATE에 따라서 ISL 시행 및 중단
}


