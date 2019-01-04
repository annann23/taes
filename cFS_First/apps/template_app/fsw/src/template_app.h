/*******************************************************************************
** File: template_app.h
**
** Purpose:
**   This file is main header file for the application.
**
** Note:
*******************************************************************************/

#ifndef _template_app_h_
#define _template_app_h_

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>

/***********************************************************************/

#define TEMPLATE_PIPE_DEPTH                     32


/************************************************************************
** Type Definitions
*************************************************************************/


/****************************************************************************/
/*
** Local function prototypes.
**
** Note: App main
**
*/

void TEMPLATE_AppMain(void);
void TEMPLATE_AppInit(void);
void TEMPLATE_ProcessCommandPacket(void);
void TEMPLATE_ProcessGroundCommand(void);
void TEMPLATE_ProcessSchdulePacket(void);
void TEMPLATE_ReportHousekeeping(void);
void TEMPLATE_ResetCounters(void);
boolean TEMPLATE_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _template_app_h_ */
