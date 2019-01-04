/*
** $Id: $
**
** Purpose:  cFE Application "template" (GNC) header file
**
** Author: Shinhye Moon  
**
** Notes:
**
** $Log: $
**
*/


#ifndef _gnc_app_h_
#define _gnc_app_h_


/************************************************************************
** Includes
*************************************************************************/

#include "cfe.h"
#include "gnc_msgids.h"
#include "gnc_perfids.h"
#include "gnc_functions.h"
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
#define GNC_PIPE_DEPTH                     32


/************************************************************************
** Type Definitions
*************************************************************************/



/*************************************************************************
** Exported data
**************************************************************************/


/************************************************************************
** Exported Functions
*************************************************************************/
void GNC_AppMain(void);
void GNC_AppInit(void);
void GNC_ProcessCommandPacket(void);
void GNC_ProcessGroundCommand(void);
void GNC_ProcessScheduleCommand(void);
void GNC_ReportHousekeeping(void);
void GNC_ResetCounters(void);
boolean GNC_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);


#endif /* _gnc_app_ */

/************************/
/*  End of File Comment */
/************************/
