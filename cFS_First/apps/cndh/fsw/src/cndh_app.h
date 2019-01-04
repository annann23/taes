/*
** $Id: $
**
** Purpose:  cFE Application "template" (CNDH) header file
**
** Author: Shinhye Moon  
**
** Notes:
**
** $Log: $
**
*/


#ifndef _cndh_app_h_
#define _cndh_app_h_


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "cndh_msgids.h"
#include "cndh_perfids.h"
#include "cndh_msg.h"
#include "cndh_events.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>

/*************************************************************************
** Macro definitions
**************************************************************************/
#define CNDH_PIPE_DEPTH                     64


/************************************************************************
** Type Definitions
*************************************************************************/



/*************************************************************************
** Exported data
**************************************************************************/

/************************************************************************
** Exported Functions
*************************************************************************/
void CNDH_AppMain(void);
void CNDH_AppInit(void);
void CNDH_ProcessCommandPacket(void);
void CNDH_ProcessGroundCommand(void);
void CNDH_ProcessScheduleCommand(void);
void CNDH_ReportHousekeeping(void);
void CNDH_ResetCounters(void);
void CNDH_Save_data(int number);
boolean CNDH_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

void Separation_Mode_1U(void);
void Stablization_Mode_1U(void);
void normalEO_Mode_1U(void);
#endif /* _cndh_app_ */

/************************/
/*  End of File Comment */
/************************/
