/*
** $Id: $
**
** Purpose:  cFE Application "template" (ADCS) header file
**
** Author: Shinhye Moon  
**
** Notes:
**
** $Log: $
**
*/


#ifndef _adcs_app_h_
#define _adcs_app_h_


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "adcs_perfids.h"
#include "adcs_msgids.h"
#include "adcs_events.h"
#include "adcs_msg.h"

/*************************************************************************
** Macro definitions
**************************************************************************/
#define ADCS_PIPE_DEPTH                     32


/************************************************************************
** Type Definitions
*************************************************************************/


/*************************************************************************
** Exported data
**************************************************************************/
boolean adcs_detumb;

/************************************************************************
** Exported Functions
*************************************************************************/
void ADCS_AppMain(void);
void ADCS_AppInit(void);
void ADCS_ProcessCommandPacket(void);
void ADCS_ProcessGroundCommand(void);
void ADCS_ProcessScheduleCommand(void);
void ADCS_ReportHousekeeping(void);
void ADCS_ResetCounters(void);
//void ADCS_SendData(void);
//void ADCS_Detumbling(void);
boolean ADCS_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);


#endif /* _adcs_app_ */

/************************/
/*  End of File Comment */
/************************/
