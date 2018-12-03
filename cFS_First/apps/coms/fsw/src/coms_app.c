/************************************************************************
** File: coms_app.c 
**
/*
** $Id: $
**
** Subsystem: cFE Application Template (COMS) Application
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
#include "coms_app.h"
#include "coms_perfids.h"
#include "coms_msgids.h"
//#include "coms_functions.h"
#include "coms_msg.h"
#include "coms_events.h"
#include <stdio.h>


//#include "cndh_msg.h"
#include "cndh_msgids.h"
#include "eps_msgids.h"

/************************************************************************
** COMS global data
*************************************************************************/

coms_hk_tlm_t    coms_HkTelemetryPkt;
coms_log     	 coms_LogMsg;
coms_log2     	 coms_LogMsg2;


coms_cndhcmd_t	 coms_cndhCmd;

eps_log_2_coms		 *eps_LogMsg3;


CFE_SB_PipeId_t    coms_CommandPipe;
CFE_SB_MsgPtr_t    comsMsgPtr;

static CFE_EVS_BinFilter_t  COMS_EventFilters[] =
       {  /* Event ID    mask */
          {COMS_STARTUP_INF_EID,       0x0000},
          {COMS_COMMAND_ERR_EID,       0x0000},
          {COMS_COMMANDNOP_INF_EID,    0x0000},
          {COMS_COMMANDRST_INF_EID,    0x0000},
       };

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* COMS application entry point and main process loop                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void COMS_AppMain( void )
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(COMS_APPMAIN_PERF_ID);

    COMS_AppInit();

    /*
    ** coms Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        CFE_ES_PerfLogExit(COMS_APPMAIN_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&comsMsgPtr, coms_CommandPipe, 500);

        CFE_ES_PerfLogEntry(COMS_APPMAIN_PERF_ID);

        if (status == CFE_SUCCESS)
        {
			COMS_ProcessCommandPacket();
        }

    }

    CFE_ES_ExitApp(RunStatus);

} /* End of template_AppMain() */

void COMS_AppInit(void)
{
	CFE_ES_RegisterApp();

	CFE_EVS_Register(COMS_EventFilters,
		sizeof(COMS_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
		CFE_EVS_BINARY_FILTER);

	/*
	** Create the Software Bus command pipe and subscribe to housekeeping
	**  messages
	*/

	CFE_SB_CreatePipe(&coms_CommandPipe, COMS_PIPE_DEPTH, "COMS_CMD_PIPE");

	CFE_SB_Subscribe(COMS_CMD_MID, coms_CommandPipe);
	CFE_SB_Subscribe(COMS_SEND_HK_MID, coms_CommandPipe);
	CFE_SB_Subscribe(COMS_WAKE_UP_MID, coms_CommandPipe);

	COMS_ResetCounters();

	CFE_SB_InitMsg(&coms_HkTelemetryPkt,
		COMS_HK_TLM_MID,
		COMS_APP_HK_TLM_LNGTH, TRUE);

	CFE_SB_InitMsg(&coms_LogMsg,
        			COMS_LOGMSG_MID,
					COMS_APP_LOG_LNGTH, TRUE);

	CFE_SB_InitMsg(&coms_cndhCmd,
        			CNDH_CMD_MID,
					COMS_CNDH_CMD_LNGTH, TRUE);


    CFE_EVS_SendEvent (COMS_STARTUP_INF_EID, CFE_EVS_INFORMATION,
               "template COMS Initialized. Version %d.%d.%d.%d");

} /* End of template_AppInit() */


void COMS_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(comsMsgPtr);

    switch (MsgId)
    {
        case COMS_CMD_MID:
        	COMS_ProcessGroundCommand();
            break;

        case COMS_SEND_HK_MID:
            COMS_ReportHousekeeping();
            COMS_SendData();
            break;

        case COMS_WAKE_UP_MID:
        	COMS_ProcessScheduleCommand();
			break;

        default:
            coms_HkTelemetryPkt.coms_command_error_count++;
            CFE_EVS_SendEvent(COMS_COMMAND_ERR_EID,CFE_EVS_ERROR,
			"COMS: invalid command packet,MID = 0x%x", MsgId);
            break;
    }

    return;

} /* End template_ProcessCommandPacket */



void COMS_ProcessGroundCommand(void)
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(comsMsgPtr);
    coms_HkTelemetryPkt.coms_command_count++;
    switch (CommandCode)
    {
        case COMS_APP_NOOP_CC:
            coms_HkTelemetryPkt.coms_command_count++;
            CFE_EVS_SendEvent(COMS_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"coms: NOOP command");
            break;

        case COMS_APP_RESET_COUNTERS_CC:
        	COMS_ResetCounters();
            break;

		case COMS_GROUND_COMMAND:
			break;

		case COMS_TRANSMIT_BEACON:
			if (coms_LogMsg2.AX100_Status == 1)
				OS_printf("COMS, Transmit beacon. Done.\n");
			break;

		case COMS_GET_EPS_CC:
			eps_LogMsg3 = (eps_log_2_coms *)comsMsgPtr;
			if (eps_LogMsg3->AX100_Status == 1)
			{
				coms_LogMsg2.AX100_Status = 1;
			}
			OS_printf("ADCS, EPS data get\n");
			break;

        /* default case already found during FC vs length test */
        default:
            break;
    }
    return;

} /* End of template_ProcessGroundCommand() */

void COMS_ProcessScheduleCommand(void)
{
	CFE_SB_InitMsg (&coms_LogMsg, CNDH_CMD_MID, sizeof(coms_log), TRUE);

	if (coms_LogMsg2.AX100_Status == 1)
	{
		coms_LogMsg.AX100_Checksum = 1;
		OS_printf("COMS, AX100 operation normal\n");
	}

	COMS_SendData();

}

void COMS_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &coms_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &coms_HkTelemetryPkt);
    return;

} /* End of template_ReportHousekeeping() */
void COMS_ResetCounters(void)
{
    /* Status of commands processed by the template App */
    coms_HkTelemetryPkt.coms_command_count       = 0;
    coms_HkTelemetryPkt.coms_command_error_count = 0;

    CFE_EVS_SendEvent(COMS_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"template: RESET command");
    return;

} /* End of template_ResetCounters() */

void COMS_SendData(void)
{
	 CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) coms_LogMsg.CmdCHeader,CNDH_GET_COMS_CC);
	 //CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &coms_LogMsg);
	 CFE_SB_SendMsg((CFE_SB_Msg_t *) &coms_LogMsg);

	 return;
}


boolean COMS_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
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

        CFE_EVS_SendEvent(COMS_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        coms_HkTelemetryPkt.coms_command_error_count++;
    }

    return(result);

} /* End of template_VerifyCmdLength() */





















/************************/
/*  End of File Comment */
/************************/
