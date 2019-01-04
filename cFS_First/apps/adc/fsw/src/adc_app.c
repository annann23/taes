/************************************************************************
** File: adc_app.c 
**
/*
** $Id: $
**
** Subsystem: cFE Application Template (ADC) Application
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
#include "adc_app.h"
#include <string.h>
#include "adc_perfids.h"
#include "adc_msgids.h"
//#include "adc_functions.h"
#include "adc_events.h"
#include <stdio.h>
#include "adc_msg.h"

//#include "cndh_msgids.h"
#include "i2c_lib.h"


#include "cndh_msgids.h"
#include "eps_msgids.h"

/************************************************************************
** ADC global data
*************************************************************************/

adc_hk_tlm_t    adc_HkTelemetryPkt;
adc_logtlm_t    adc_logTlm;
adc_log      	adc_LogMsg;
adc_log2     	adc_LogMsg2;


CFE_SB_PipeId_t    adc_CommandPipe;
CFE_SB_MsgPtr_t    adcMsgPtr;
accel_t_gyro_union accel_t_gyro;

eps_log_2_adcs			*eps_LogMsg;


static CFE_EVS_BinFilter_t  ADC_EventFilters[] =
       {  /* Event ID    mask */
          {ADC_STARTUP_INF_EID,       0x0000},
          {ADC_COMMAND_ERR_EID,       0x0000},
          {ADC_COMMANDNOP_INF_EID,    0x0000},
          {ADC_COMMANDRST_INF_EID,    0x0000},
       };

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* ADC application entry point and main process loop                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void ADC_AppMain( void )
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(ADC_APPMAIN_PERF_ID);

    ADC_AppInit();

    /*
    ** adc Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        CFE_ES_PerfLogExit(ADC_APPMAIN_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&adcMsgPtr, adc_CommandPipe, 500);

        CFE_ES_PerfLogEntry(ADC_APPMAIN_PERF_ID);

        if (status == CFE_SUCCESS)
        {
			ADC_ProcessCommandPacket();
        }

    }

    CFE_ES_ExitApp(RunStatus);

} /* End of template_AppMain() */

void ADC_AppInit(void)
{
	CFE_ES_RegisterApp();

	CFE_EVS_Register(ADC_EventFilters,
		sizeof(ADC_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
		CFE_EVS_BINARY_FILTER);

	/*
	** Create the Software Bus command pipe and subscribe to housekeeping
	**  messages
	*/

	CFE_SB_CreatePipe(&adc_CommandPipe, ADC_PIPE_DEPTH, "ADC_CMD_PIPE");

	CFE_SB_Subscribe(ADC_CMD_MID, adc_CommandPipe);
	CFE_SB_Subscribe(ADC_SEND_HK_MID, adc_CommandPipe);
	CFE_SB_Subscribe(ADC_WAKE_UP_MID, adc_CommandPipe);


	ADC_ResetCounters();

	CFE_SB_InitMsg(&adc_HkTelemetryPkt,
		ADC_HK_TLM_MID,
		ADC_APP_HK_TLM_LNGTH, TRUE);

	CFE_SB_InitMsg(&adc_LogMsg,
        			ADC_LOGMSG_MID,
					ADC_APP_LOG_LNGTH, TRUE);

	CFE_SB_InitMsg (&adc_LogMsg, CNDH_CMD_MID, sizeof(adc_log), TRUE);

	adc_HkTelemetryPkt.adc_command_count=0;

	  CFE_EVS_SendEvent (ADC_STARTUP_INF_EID, CFE_EVS_INFORMATION,
		               "template ADC Initialized. Version %d.%d.%d.%d");

} /* End of template_AppInit() */

void ADC_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(adcMsgPtr);
	adc_HkTelemetryPkt.adc_command_count++;

    switch (MsgId)
    {
        case ADC_CMD_MID:
        	ADC_ProcessGroundCommand();
            break;
        case ADC_SEND_HK_MID:
            ADC_ReportHousekeeping();
            ADC_SendData();
            break;
        case ADC_WAKE_UP_MID:
			ADC_ProcessScheduleCommand();
			break;
        default:
            adc_HkTelemetryPkt.adc_command_error_count++;
            CFE_EVS_SendEvent(ADC_COMMAND_ERR_EID,CFE_EVS_ERROR,
			"ADC: invalid command packet,MID = 0x%x", MsgId);
            break;
    }

    return;

} /* End template_ProcessCommandPacket */

void ADC_ProcessGroundCommand(void)
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(adcMsgPtr);



    switch (CommandCode)
    {
        case ADC_APP_NOOP_CC:
            adc_HkTelemetryPkt.adc_command_count++;
            CFE_EVS_SendEvent(ADC_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"adc: NOOP command");
            break;

        case ADC_APP_RESET_COUNTERS_CC:
        	ADC_ResetCounters();
            break;

		case ADC_AD_COMMAND:
			adc_logTlm.AD_Status = 1;
			OS_printf("ADCS, AD process success\n");
			break;

		case ADC_DETUMBLING_START_COMMAND:
			adc_detumb = TRUE;
			OS_printf("ADCS, Detumbling MTQ control start.\n");
			break;

		case ADC_DETUMBLING_END_COMMAND:
			adc_detumb = FALSE;
			OS_printf("ADCS, Detumbling MTQ control end.\n");
			break;

        case ADCS_GET_EPS_CC:
        	eps_LogMsg = (eps_log_2_adcs *)adcMsgPtr;
			OS_printf("ADCS, EPS data get\n");
			break;

        /* default case already found during FC vs length test */
        default:
            break;
    }
    return;

} /* End of template_ProcessGroundCommand() */

void ADC_ProcessScheduleCommand(void)
{
	CFE_SB_InitMsg (&adc_LogMsg, CNDH_CMD_MID, sizeof(adc_log), TRUE);



//	adc_LogMsg.MMT = 12;
//	adc_logTlm.MMT = 12;

	uint8 ADC_cmdBuf[4];
	ADC_cmdBuf[0] = 0;
	I2CLib_WriteBytes(1, 0x68, 0x6B, 1, ADC_cmdBuf);

	ADC_cmdBuf[0] = 0x00;
	I2CLib_WriteBytes(1, 0x68, 0x1B, 1, ADC_cmdBuf);

	ADC_cmdBuf[0] = 0x00;
	I2CLib_WriteBytes(1, 0x68, 0x1C, 1, ADC_cmdBuf);

	ADC_cmdBuf[0] = 0x06;
	I2CLib_WriteBytes(1, 0x68, 0x1A, 1, ADC_cmdBuf);

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

	adc_LogMsg.Accel[0] = accel_t_gyro.value.x_accel/(16384.0f);
	adc_LogMsg.Accel[1] = accel_t_gyro.value.y_accel/(16384.0f);
	adc_LogMsg.Accel[2] = accel_t_gyro.value.z_accel/(16384.0f);

	adc_LogMsg.AngVel[0] = accel_t_gyro.value.x_gyro/(131.0f);
	adc_LogMsg.AngVel[1] = accel_t_gyro.value.y_gyro/(131.0f);
	adc_LogMsg.AngVel[2] = accel_t_gyro.value.z_gyro/(131.0f);

	/*OS_printf("ADCS, x-axis accel %d (g)\n", adc_LogMsg.Accel[0]);
	OS_printf("ADCS, y-axis accel %d (g)\n", adc_LogMsg.Accel[1]);
	OS_printf("ADCS, z-axis accel %d (g)\n", adc_LogMsg.Accel[2]);*/


	//OS_printf("ADCS, temperature %d \n", adc_LogMsg.Temp);

/*	OS_printf("ADCS, x-axis angular velocity %d (deg/s)\n", adc_LogMsg.AngVel[0]);
	OS_printf("ADCS, y-axis angular velocity %d (deg/s)\n", adc_LogMsg.AngVel[1]);
	OS_printf("ADCS, z-axis angular velocity %d (deg/s)\n", adc_LogMsg.AngVel[2]);*/

	adc_LogMsg.IMU_Checksum = 1;
	adc_LogMsg.MMT_Checksum = 1;


	//Add code here later
	adc_LogMsg.FSS_Checksum = 1;
	adc_LogMsg.CSS_Checksum = 1;

	// need to change after SB solved
	/*if (adc_HkTelemetryPkt.adc_command_count > 1)
	{
		OS_printf("ADCS, FSS/CSS checksum done\n");
	}*/


	CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) adc_LogMsg.TmdHeader, 4);
	CFE_SB_SendMsg((CFE_SB_Msg_t *) &adc_LogMsg);

}


void ADC_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &adc_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &adc_HkTelemetryPkt);
    return;

} /* End of ADC_ReportHousekeeping() */
void ADC_ResetCounters(void)
{
    /* Status of commands processed by the template App */
    adc_HkTelemetryPkt.adc_command_count       = 0;
    adc_HkTelemetryPkt.adc_command_error_count = 0;

    CFE_EVS_SendEvent(ADC_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"ADCS: RESET command");
    return;

} /* End of ADC_ResetCounters() */

void ADC_SendData(void)
{
	 CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &adc_LogMsg,CNDH_GET_ADC_CC);
	 CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &adc_LogMsg);
	 CFE_SB_SendMsg((CFE_SB_Msg_t *) &adc_LogMsg);

	 return;
}

boolean ADC_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
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

        CFE_EVS_SendEvent(ADC_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        adc_HkTelemetryPkt.adc_command_error_count++;
    }

    return(result);

} /* End of ADC_VerifyCmdLength() */



void ADC_Detumbling(void)
{

	while(1)
	{
		if (adc_detumb)
		{
			//add detumbling mode
		}
		else
			break;
	}

}


















/************************/
/*  End of File Comment */
/************************/
