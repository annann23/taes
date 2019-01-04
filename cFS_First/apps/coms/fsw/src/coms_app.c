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

/* Includes */

// local application code include
#include "coms_app.h"

// global application code include
#include "cndh_msgids.h"
#include "eps_msgids.h"

// library and other code include
#include <stdio.h>


/* COMS global data */

// local application data
coms_hk_tlm_t    		coms_hk_tlm;
coms_log_t     	 		coms_log;

// command & send data list
coms_log_cndh_t	 		coms_log_cndh;

// receive log data list
eps_log_coms_t		 	*eps_log_coms;
ground_cmd_coms_t		*ground_cmd_coms;

// application pointers
CFE_SB_PipeId_t    coms_CommandPipe;
CFE_SB_MsgPtr_t    comsMsgPtr;

// event filter (on-going)
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

	CFE_SB_InitMsg(&coms_hk_tlm,
					COMS_HK_TLM_MID,
					COMS_APP_HK_TLM_LENGTH, TRUE);

	CFE_SB_InitMsg(&coms_log,
        			COMS_LOGMSG_MID,
					COMS_LOG_LENGTH, TRUE);


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
        	coms_hk_tlm.coms_command_error_count++;
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
    switch (CommandCode)
    {
        case COMS_APP_NOOP_CC:
        	coms_hk_tlm.coms_command_count++;
            CFE_EVS_SendEvent(COMS_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"coms: NOOP command");
            break;

        case COMS_APP_RESET_COUNTERS_CC:
        	COMS_ResetCounters();
            break;

		case COMS_GROUND_COMMAND:
			coms_hk_tlm.coms_command_count++;
			CFE_SB_InitMsg (&coms_log_cndh, CNDH_CMD_MID, COMS_LOG_CNDH_LENGTH, TRUE);
			ground_cmd_coms = (ground_cmd_coms_t *)comsMsgPtr;
			coms_log_cndh.time = ground_cmd_coms->time;
			coms_log_cndh.flag = ground_cmd_coms->flag;
			OS_printf("L6:COMS, Ground command get\n");
			CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &coms_log_cndh,CNDH_COMMUNICATION_CC);
			CFE_SB_SendMsg((CFE_SB_Msg_t *) &coms_log_cndh);
			break;

		case COMS_TRANSMIT_BEACON:
			coms_hk_tlm.coms_command_count++;
			if (coms_log.AX100_Status == 1)
				OS_printf("L3:COMS, Transmit beacon. Done.\n");
			break;

		case COMS_GET_EPS_CC:
			coms_hk_tlm.coms_command_count++;
			eps_log_coms = (eps_log_coms_t *)comsMsgPtr;
			if (eps_log_coms->AX100_Status == 1)
			{
				coms_log.AX100_Status = 1;
			}
			OS_printf("L5:COMS, EPS data get\n");
			break;

		case COMS_SEND_HK:
			CFE_SB_SendMsg((CFE_SB_Msg_t *) &coms_hk_tlm);
			break;

        /* default case already found during FC vs length test */
        default:
            break;
    }
    return;

} /* End of template_ProcessGroundCommand() */

void COMS_ProcessScheduleCommand(void)
{
	CFE_SB_InitMsg (&coms_log_cndh, CNDH_CMD_MID, COMS_LOG_CNDH_LENGTH, TRUE);

	if (coms_log.AX100_Status == 1)
	{
		coms_log_cndh.AX100_Checksum = 1;
		OS_printf("L3:COMS, AX100 operation normal\n");
	}

	COMS_SendData();

}

void COMS_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &coms_hk_tlm);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &coms_hk_tlm);
    return;

} /* End of template_ReportHousekeeping() */
void COMS_ResetCounters(void)
{
    /* Status of commands processed by the template App */
	coms_hk_tlm.coms_command_count       = 0;
	coms_hk_tlm.coms_command_error_count = 0;

    CFE_EVS_SendEvent(COMS_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"template: RESET command");
    return;

} /* End of template_ResetCounters() */

void COMS_SendData(void)
{
	 CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &coms_log_cndh,CNDH_GET_COMS_CC);
	 CFE_SB_SendMsg((CFE_SB_Msg_t *) &coms_log_cndh);

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
        coms_hk_tlm.coms_command_error_count++;
    }

    return(result);

} /* End of template_VerifyCmdLength() */





















/************************/
/*  End of File Comment */
/************************/
