/*******************************************************************************
** File: sample_app.h
**
** Purpose:
**   This file is main hdr file for the SAMPLE application.
**
**
*******************************************************************************/

#ifndef _sample_app_h_
#define _sample_app_h_

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

#define SAMPLE_PIPE_DEPTH                     32

/************************************************************************
** Type Definitions
*************************************************************************/


//Table for led results
typedef struct
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              sample_command_error_count;
    uint8              sample_command_count;
    int16	       sample_led1_state;
    int16              sample_led2_state;
    uint8              spare[2];
}LED_MyTable_t;



/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (SAMPLE_AppMain), these
**       functions are not called from any other source module.
*/
void SAMPLE_AppMain(void);
void SAMPLE_AppInit(void);
void SAMPLE_ProcessCommandPacket(void);
void SAMPLE_ProcessGroundCommand(void);
void SAMPLE_ReportHousekeeping(void);
void SAMPLE_ResetCounters(void);
int32 LED_MyTableValidationFunc(void *TblPtr);

boolean SAMPLE_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _sample_app_h_ */
