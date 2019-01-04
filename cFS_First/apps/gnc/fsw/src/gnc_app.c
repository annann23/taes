/************************************************************************
** File: gnc_app.c 
**
/*
** $Id: $
**
** Subsystem: cFE Application Template (GNC) Application
**
** Author: 
**
** Notes:
**
** $Log: $
**

/************************************************************************
** Includes
*************************************************************************/
#include "gnc_app.h"
#include <string.h>
#include "gnc_perfids.h"
#include "gnc_msgids.h"
#include "gnc_functions.h"
#include "gnc_events.h"
#include <stdio.h>
#include "gnc_msg.h"

/************************************************************************
** GNC global data
*************************************************************************/

gnc_hk_tlm_t    gnc_HkTelemetryPkt;
gnc_log     	gnc_LogMsg;

CFE_SB_PipeId_t    gnc_CommandPipe;
CFE_SB_MsgPtr_t    gncMsgPtr;

static CFE_EVS_BinFilter_t  GNC_EventFilters[] =
       {  /* Event ID    mask */
          {GNC_STARTUP_INF_EID,       0x0000},
          {GNC_COMMAND_ERR_EID,       0x0000},
          {GNC_COMMANDNOP_INF_EID,    0x0000},
          {GNC_COMMANDRST_INF_EID,    0x0000},
       };

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* GNC application entry point and main process loop                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void GNC_AppMain( void )
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(GNC_APPMAIN_PERF_ID);

    GNC_AppInit();

    /*
    ** gnc Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        CFE_ES_PerfLogExit(GNC_APPMAIN_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&gncMsgPtr, gnc_CommandPipe, 500);

        CFE_ES_PerfLogEntry(GNC_APPMAIN_PERF_ID);

        if (status == CFE_SUCCESS)
        {
			GNC_ProcessCommandPacket();
        }

    }

    CFE_ES_ExitApp(RunStatus);

} /* End of template_AppMain() */
void GNC_AppInit(void)
{
	CFE_ES_RegisterApp();

	CFE_EVS_Register(GNC_EventFilters,
		sizeof(GNC_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
		CFE_EVS_BINARY_FILTER);

	/*
	** Create the Software Bus command pipe and subscribe to housekeeping
	**  messages
	*/

	CFE_SB_CreatePipe(&gnc_CommandPipe, GNC_PIPE_DEPTH, "GNC_CMD_PIPE");

	CFE_SB_Subscribe(GNC_CMD_MID, gnc_CommandPipe);
	CFE_SB_Subscribe(GNC_SEND_HK_MID, gnc_CommandPipe);
	CFE_SB_Subscribe(GNC_WAKE_UP_MID, gnc_CommandPipe);

	GNC_ResetCounters();

	CFE_SB_InitMsg(&gnc_HkTelemetryPkt,
		GNC_HK_TLM_MID,
		GNC_APP_HK_TLM_LNGTH, TRUE);

	CFE_SB_InitMsg(&gnc_LogMsg,
        			GNC_LOGMSG_MID,
					GNC_APP_LOG_LNGTH, TRUE);

} /* End of template_AppInit() */

void GNC_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(gncMsgPtr);

    switch (MsgId)
    {
        case GNC_CMD_MID:
        	GNC_ProcessGroundCommand();
            break;
        case GNC_SEND_HK_MID:
            GNC_ReportHousekeeping();
            break;
        case GNC_WAKE_UP_MID:
        	GNC_ProcessScheduleCommand();
			break;
        default:
            gnc_HkTelemetryPkt.gnc_command_error_count++;
            CFE_EVS_SendEvent(GNC_COMMAND_ERR_EID,CFE_EVS_ERROR,
			"GNC: invalid command packet,MID = 0x%x", MsgId);
            break;
    }

    return;

} /* End template_ProcessCommandPacket */


void TEMPLATE_ProcessGroundCommand(void)
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(gncMsgPtr);

    switch (CommandCode)
    {
        case GNC_APP_NOOP_CC:
            gnc_HkTelemetryPkt.gnc_command_count++;
            CFE_EVS_SendEvent(GNC_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"gnc: NOOP command");
            break;

        case GNC_APP_RESET_COUNTERS_CC:
        	GNC_ResetCounters();
            break;

		case PROCESS_CC:
			RoutineProcessingCmd(gncMsgPtr);
			break;

        /* default case already found during FC vs length test */
        default:
            break;
    }
    return;

} /* End of template_ProcessGroundCommand() */

void GNC_ProcessScheduleCommand(void)
{

}


void GNC_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &gnc_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &gnc_HkTelemetryPkt);
    return;

} /* End of template_ReportHousekeeping() */
void GNC_ResetCounters(void)
{
    /* Status of commands processed by the template App */
    gnc_HkTelemetryPkt.gnc_command_count       = 0;
    gnc_HkTelemetryPkt.gnc_command_error_count = 0;

    CFE_EVS_SendEvent(GNC_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"template: RESET command");
    return;

} /* End of template_ResetCounters() */
boolean GNC_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
{
    boolean result = TRUE;

    uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID   = CFE_SB_GetMsgId(msg);
        uint16         CommandCode = CFE_SB_GetCmdCode(msg);

        CFE_EVS_SendEvent(GNC_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        gnc_HkTelemetryPkt.gnc_command_error_count++;
    }

    return(result);

} /* End of template_VerifyCmdLength() */

/************************/
/*  End of File Comment */
/************************/
