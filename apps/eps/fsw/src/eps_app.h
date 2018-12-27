/*
** $Id: $
**
** Purpose:  cFE Application "template" (EPS) header file
**
** Author: Shinhye Moon  
**
** Notes:
**
** $Log: $
**
*/


#ifndef _eps_app_h_
#define _eps_app_h_


/************************************************************************
** Includes
*************************************************************************/

#include "cfe.h"
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "eps_perfids.h"
#include "eps_msgids.h"
#include "eps_events.h"
#include "eps_msg.h"
//#include "eps_functions.h"


/*************************************************************************
** Macro definitions
**************************************************************************/
#define EPS_PIPE_DEPTH                     32


/************************************************************************
** Type Definitions
*************************************************************************/



/*************************************************************************
** Exported data
**************************************************************************/


/************************************************************************
** Exported Functions
*************************************************************************/
void EPS_AppMain(void);
void EPS_AppInit(void);
void EPS_ProcessCommandPacket(void);
void EPS_ProcessGroundCommand(void);
void EPS_ProcessScheduleCommand(void);
void EPS_ReportHousekeeping(void);
void EPS_SendData(void);
void EPS_ResetCounters(void);
boolean EPS_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);


#endif /* _eps_app_ */

/************************/
/*  End of File Comment */
/************************/
