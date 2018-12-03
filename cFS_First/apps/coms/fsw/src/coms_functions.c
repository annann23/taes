/*
 * COMS.c
 *
 *  Created on: 2018. 9. 21.
 *      Author: Shinhye
 */

#include "coms_functions.h"

void COMS_Sensor_Signal_Output(char sensor)
{
	int Status;
	Status=AX100U_lib('AX100U_TEST');
	CFE_TBL_Load(Status);
}

int COMS_UHF_Deployment(int UHF_number)
 {
	 int Status;
	 Status=Deploy_lib('UHF_Deployment',UHF_number); //10초간 신호인가
	 return Status;
 }

void COMS_Beacon_Tx(void){
	 //비콘 신호 전송을 한다.
 }

void COMS_FSK_Modulation(double MHz=123, double lower_dB=123, double upper_dB=123)
 {
	 //비콘 Tx시 기본 디폴트 값은 비콘설정이다. 일단은 123이라고 대략적음
	 COMS_lib('FSK', MHz, lower_dB, upper_dB);
 }

int COMS_Uplink(void){
	 COMS_lib('AX100','Uplink');
	 int State=COMS_lib('Checksum') //통신 무결성 확인 알고리즘
	 return State;
 }

int COMS_Downlink(void){
	 COMS_lib('AX100','Downlink');
	 int State=COMS_lib('Checksum') //통신 무결성 확인 알고리즘
	 return State;
 }





