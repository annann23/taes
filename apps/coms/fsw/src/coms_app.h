/*
** $Id: $
**
** Purpose:  cFE Application "template" (COMS) header file
**
** Author: Shinhye Moon  
**
** Notes:
**
** $Log: $
**
*/


#ifndef _coms_app_h_
#define _coms_app_h_


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

#include "coms_perfids.h"
#include "coms_msgids.h"
#include "coms_msg.h"
#include "coms_events.h"

/*************************************************************************
** Macro definitions
**************************************************************************/
#define COMS_PIPE_DEPTH                     32


/************************************************************************
** Type Definitions
*************************************************************************/



/*************************************************************************
** Exported data
**************************************************************************/


/************************************************************************
** Exported Functions
*************************************************************************/
void COMS_AppMain(void);
void COMS_AppInit(void);
void COMS_ProcessCommandPacket(void);
void COMS_ProcessGroundCommand(void);
void COMS_ProcessScheduleCommand(void);
void COMS_ReportHousekeeping(void);
void COMS_ResetCounters(void);
void COMS_SendData(void);
boolean COMS_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);


#endif /* _coms_app_ */

/************************/
/*  End of File Comment */
/************************/
