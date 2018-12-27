/*
** $Id: $
**
** Purpose:  cFE Application "template" (COMS) functions header file
**
** Author: Shinhye Moon  
**
** Notes:
**
** $Log: $
**
*/


#ifndef _coms_functions_h_
#define _coms_functions_h_


#include "cfe.h"
#include "coms_msgids.h"
#include "coms_perfids.h"


void COMS_Sensor_Signal_Output(char sensor);
int COMS_UHF_Deployment(int UHF_number);
void COMS_Beacon_Tx(void);
void COMS_FSK_Modulation(double MHz, double lower_dB, double upper_dB);
int COMS_Uplink(void);
int COMS_Downlink(void);




#endif /* _coms_app_ */

/************************/
/*  End of File Comment */
/************************/
