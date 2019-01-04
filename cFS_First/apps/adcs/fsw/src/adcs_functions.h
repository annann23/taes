/*
** $Id: $
**
** Purpose:  cFE Application "template" (ADC) functions header file
**
** Author: Shinhye Moon  
**
** Notes:
**
** $Log: $
**
*/


#ifndef _adc_functions_h_
#define _adc_functions_h_


#include "../../../adcs/fsw/mission_inc/adcs_perfids.h"
#include "../../../adcs/fsw/platform_inc/adcs_msgids.h"
#include "cfe.h"

void ADCS_Sensor_Signal_Output(char Sensor);
void ADCS_Initial_AD(void);
double ADCS_AngularVelocity(void);
double ADCS_AngleError(char object);
void ADCS_AD(void);





#endif /* _adc_functions_ */

/************************/
/*  End of File Comment */
/************************/
