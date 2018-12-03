/*
** $Id: $
**
** Purpose:  cFE Application "template" (ADC) header file
**
** Author: Shinhye Moon  
**
** Notes:
**
** $Log: $
**
*/


#ifndef _adc_app_h_
#define _adc_app_h_


/************************************************************************
** Includes
*************************************************************************/

#include "cfe.h"
#include "adc_msgids.h"
#include "adc_perfids.h"
//#include "adc_functions.h"
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>


/*************************************************************************
** Macro definitions
**************************************************************************/
#define ADC_PIPE_DEPTH                     32


/************************************************************************
** Type Definitions
*************************************************************************/



/*************************************************************************
** Exported data
**************************************************************************/
boolean adc_detumb;

/************************************************************************
** Exported Functions
*************************************************************************/
void ADC_AppMain(void);
void ADC_AppInit(void);
void ADC_ProcessCommandPacket(void);
void ADC_ProcessGroundCommand(void);
void ADC_ProcessScheduleCommand(void);
void ADC_ReportHousekeeping(void);
void ADC_ResetCounters(void);
void ADC_SendData(void);
void ADC_Detumbling(void);
boolean ADC_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);


#endif /* _adc_app_ */

/************************/
/*  End of File Comment */
/************************/
