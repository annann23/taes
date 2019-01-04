/*
** $Id: $
**
** Purpose:  cFE Application "template" (CNDH) functions header file
**
** Author: Shinhye Moon  
**
** Notes:
**
** $Log: $
**
*/


#ifndef _cndh_functions_h_
#define _cndh_functions_h_


#include "cfe.h"
#include "cndh_msgids.h"
#include "cndh_perfids.h"

void CNDH_BaseSensor_Test(void);
void CNDH_Signal_Test(char subsystem, char sensor);
void CNDH_Checksum(char sensor);
void CNDH_HK_Monitoring(void);
void CNDH_UHF_Deployment(int UHF_number);
void CNDH_Beacon_Tx(void);
void CNDH_BAT_Check(void);
void CNDH_CAM_Shooting(char object, double condition, int picture_number);
void CNDH_GPS_TLE_Test(double *GPS_sol);
void CNDH_GPS_Update(void);
void CNDH_ISL(char STATE);


#endif /* _cndh_app_ */

/************************/
/*  End of File Comment */
/************************/
