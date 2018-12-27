/************************************************************************
** File: adcs_app.c
**
/*
** $Id: $
**
** Subsystem: cFE Application Template (ADCS) Application
**
** Author: 
**
** Notes:
**
** $Log: $
**


/* Includes */

// local application code include
#include "adcs_app.h"

#include "cndh_msgids.h"
#include "eps_msgids.h"

// library and other code include
#include <string.h>
#include <stdio.h>
#include "i2c_lib.h"


/* CNDH global data */

// local application data
adcs_hk_tlm_t    	adcs_hk_tlm;
adcs_logtlm_t    	adcs_logTlm;
accel_t_gyro_union 	accel_t_gyro;

adcs_log2     		adcs_LogMsg2;

// command list
adcs_log_cndh_t      		adcs_log_cndh;

// receive log data list

// application pointers
CFE_SB_PipeId_t    	adcs_CommandPipe;
CFE_SB_MsgPtr_t    	adcsMsgPtr;

// event filter (on-going)
static CFE_EVS_BinFilter_t  ADCS_EventFilters[] =
       {  /* Event ID    mask */
          {ADCS_STARTUP_INF_EID,       0x0000},
          {ADCS_COMMAND_ERR_EID,       0x0000},
          {ADCS_COMMANDNOP_INF_EID,    0x0000},
          {ADCS_COMMANDRST_INF_EID,    0x0000},
       };

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* ADCS application entry point and main process loop                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void ADCS_AppMain( void )
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(ADCS_APPMAIN_PERF_ID);

    ADCS_AppInit();

    /*
    ** adcs Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        CFE_ES_PerfLogExit(ADCS_APPMAIN_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&adcsMsgPtr, adcs_CommandPipe, 500);

        CFE_ES_PerfLogEntry(ADCS_APPMAIN_PERF_ID);

        if (status == CFE_SUCCESS)
        {
			ADCS_ProcessCommandPacket();
        }

    }

    CFE_ES_ExitApp(RunStatus);

} /* End of template_AppMain() */

void ADCS_AppInit(void)
{
	CFE_ES_RegisterApp();

	CFE_EVS_Register(ADCS_EventFilters,
		sizeof(ADCS_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
		CFE_EVS_BINARY_FILTER);

	/*
	** Create the Software Bus command pipe and subscribe to housekeeping
	**  messages
	*/

	CFE_SB_CreatePipe(&adcs_CommandPipe, ADCS_PIPE_DEPTH, "ADCS_CMD_PIPE");

	CFE_SB_Subscribe(ADCS_CMD_MID, adcs_CommandPipe);
	CFE_SB_Subscribe(ADCS_SEND_HK_MID, adcs_CommandPipe);
	CFE_SB_Subscribe(ADCS_WAKE_UP_MID, adcs_CommandPipe);


	ADCS_ResetCounters();

	CFE_SB_InitMsg(&adcs_hk_tlm,
		ADCS_HK_TLM_MID,
		ADCS_APP_HK_TLM_LENGTH, TRUE);

	CFE_SB_InitMsg(&adcs_logTlm,
        			ADCS_LOGMSG_MID,
					ADCS_APP_LOG_LENGTH, TRUE);

	adcs_hk_tlm.adcs_command_count=0;

	  CFE_EVS_SendEvent (ADCS_STARTUP_INF_EID, CFE_EVS_INFORMATION,
		               "template ADCS Initialized. Version %d.%d.%d.%d");

} /* End of template_AppInit() */

void ADCS_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(adcsMsgPtr);

    switch (MsgId)
    {
        case ADCS_CMD_MID:
        	ADCS_ProcessGroundCommand();
            break;
        case ADCS_SEND_HK_MID:
            ADCS_ReportHousekeeping();
            ADCS_SendData();
            break;
        case ADCS_WAKE_UP_MID:
			ADCS_ProcessScheduleCommand();
			break;
        default:
        	adcs_hk_tlm.adcs_command_error_count++;
            CFE_EVS_SendEvent(ADCS_COMMAND_ERR_EID,CFE_EVS_ERROR,
			"ADCS: invalid command packet,MID = 0x%x", MsgId);
            break;
    }

    return;

} /* End template_ProcessCommandPacket */

void ADCS_ProcessGroundCommand(void)
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(adcsMsgPtr);



    switch (CommandCode)
    {
        case ADCS_APP_NOOP_CC:
        	adcs_hk_tlm.adcs_command_count++;
            CFE_EVS_SendEvent(ADCS_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"adcs: NOOP command");
            break;

        case ADCS_APP_RESET_COUNTERS_CC:
        	ADCS_ResetCounters();
            break;

		case ADCS_AD_COMMAND:
			adcs_logTlm.AD_Status = 1;
			OS_printf("L2:ADCS, AD process success\n");
			break;

		case ADCS_DETUMB_START_COMMAND:
			adcs_detumb = TRUE;
			OS_printf("L3:ADCS, Detumbling MTQ control start.\n");
			break;

		case ADCS_DETUMB_END_COMMAND:
			adcs_detumb = FALSE;
			OS_printf("L3:ADCS, Detumbling MTQ control end.\n");
			break;

        /* default case already found during FC vs length test */
        default:
            break;
    }
    return;

} /* End of template_ProcessGroundCommand() */

void ADCS_ProcessScheduleCommand(void)
{
	CFE_SB_InitMsg (&adcs_log_cndh, CNDH_CMD_MID, ADCS_LOG_CNDH_LENGTH, TRUE);

	uint8 ADCS_cmdBuf[4];
	ADCS_cmdBuf[0] = 0;
	I2CLib_WriteBytes(1, 0x68, 0x6B, 1, ADCS_cmdBuf);

	ADCS_cmdBuf[0] = 0x00;
	I2CLib_WriteBytes(1, 0x68, 0x1B, 1, ADCS_cmdBuf);

	ADCS_cmdBuf[0] = 0x00;
	I2CLib_WriteBytes(1, 0x68, 0x1C, 1, ADCS_cmdBuf);

	ADCS_cmdBuf[0] = 0x06;
	I2CLib_WriteBytes(1, 0x68, 0x1A, 1, ADCS_cmdBuf);

	I2CLib_ReadOnly(1, 0x68, 0x3B, sizeof(accel_t_gyro), (uint8 *) &accel_t_gyro, 40);

	uint8 swap;
	#define SWAP(x,y) swap = x; x = y; y = swap
	SWAP (accel_t_gyro.reg.x_accel_h, accel_t_gyro.reg.x_accel_l);
	SWAP (accel_t_gyro.reg.y_accel_h, accel_t_gyro.reg.y_accel_l);
	SWAP (accel_t_gyro.reg.z_accel_h, accel_t_gyro.reg.z_accel_l);
	SWAP (accel_t_gyro.reg.t_h, accel_t_gyro.reg.t_l);
	SWAP (accel_t_gyro.reg.x_gyro_h, accel_t_gyro.reg.x_gyro_l);
	SWAP (accel_t_gyro.reg.y_gyro_h, accel_t_gyro.reg.y_gyro_l);
	SWAP (accel_t_gyro.reg.z_gyro_h, accel_t_gyro.reg.z_gyro_l);

	adcs_log_cndh.Accel[0] = accel_t_gyro.value.x_accel/(16384.0f);
	adcs_log_cndh.Accel[1] = accel_t_gyro.value.y_accel/(16384.0f);
	adcs_log_cndh.Accel[2] = accel_t_gyro.value.z_accel/(16384.0f);

	adcs_log_cndh.AngVel[0] = accel_t_gyro.value.x_gyro/(131.0f);
	adcs_log_cndh.AngVel[1] = accel_t_gyro.value.y_gyro/(131.0f);
	adcs_log_cndh.AngVel[2] = accel_t_gyro.value.z_gyro/(131.0f);
	OS_printf("L6:ADCS, IMU and MTQ sensor data get.\n");

	adcs_log_cndh.IMU_Checksum = 1;
	adcs_log_cndh.MMT_Checksum = 1;

	adcs_log_cndh.FSS_Checksum = 1;
	adcs_log_cndh.CSS_Checksum = 1;

	CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &adcs_log_cndh, CNDH_GET_ADCS_CC);
	CFE_SB_SendMsg((CFE_SB_Msg_t *) &adcs_log_cndh);

}


void ADCS_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &adcs_hk_tlm);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &adcs_hk_tlm);
    return;

} /* End of ADCS_ReportHousekeeping() */
void ADCS_ResetCounters(void)
{
    /* Status of commands processed by the template App */
	adcs_hk_tlm.adcs_command_count       = 0;
	adcs_hk_tlm.adcs_command_error_count = 0;

    CFE_EVS_SendEvent(ADCS_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"ADCS: RESET command");
    return;

} /* End of ADCS_ResetCounters() */

boolean ADCS_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
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

        CFE_EVS_SendEvent(ADCS_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        adcs_hk_tlm.adcs_command_error_count++;
    }

    return(result);

} /* End of adcs_VerifyCmdLength() */

















/************************/
/*  End of File Comment */
/************************/
