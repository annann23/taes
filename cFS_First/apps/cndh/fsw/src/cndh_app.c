/************************************************************************
** File:
**   $Id: hk_app.c 1.3 2016/10/28 10:51:14EDT mdeschu Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
/*
** $Id: $
**
** Subsystem: cFE Application Template (CNDH) Application
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
#include "cndh_app.h"

//#include "cndh_functions.h"
//#include "cndh_modes.h"
#include <stdio.h>
#include "gpio_lib.h"

#include "eps_msgids.h"
#include "adc_msgids.h"
#include "coms_msgids.h"
//#include "gnc_msgids.h"


/************************************************************************
** CNDH global data
*************************************************************************/
cndh_hk_tlm_t    cndh_HkTelemetryPkt;
cndh_log     	 cndh_LogMsg;
cndh_locallog	 cndh_LocalLog;

cndh_epscmd_t 	  cndh_epsCmd;
cndh_comscmd_t 	  cndh_comsCmd;
cndh_adccmd_t 	  cndh_adcCmd;
cndh_gnccmd_t 	  cndh_gncCmd;

cndh_flag_t		  mode_flag;

eps_log_2_cndh			*eps_LogMsg;
adc_log			*adc_LogMsg;
coms_log  		*coms_LogMsg;

CFE_SB_PipeId_t    cndh_CommandPipe;
CFE_SB_MsgPtr_t    cndhMsgPtr;

static CFE_EVS_BinFilter_t  CNDH_EventFilters[] =
       {  /* Event ID    mask */
          {CNDH_STARTUP_INF_EID,       0x0000},
          {CNDH_COMMAND_ERR_EID,       0x0000},
          {CNDH_COMMANDNOP_INF_EID,    0x0000},
          {CNDH_COMMANDRST_INF_EID,    0x0000},
       };

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CNDH application entry point and main process loop                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CNDH_AppMain( void )
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(CNDH_APPMAIN_PERF_ID);

    CNDH_AppInit();

    /*
    ** cndh Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        CFE_ES_PerfLogExit(CNDH_APPMAIN_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&cndhMsgPtr, cndh_CommandPipe, 500);

        CFE_ES_PerfLogEntry(CNDH_APPMAIN_PERF_ID);

        if (status == CFE_SUCCESS)
        {
			CNDH_ProcessCommandPacket();
        }

    }

    CFE_ES_ExitApp(RunStatus);

} /* End of template_AppMain() */
void CNDH_AppInit(void)
{
	CFE_ES_RegisterApp();

	CFE_EVS_Register(CNDH_EventFilters,
		sizeof(CNDH_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
		CFE_EVS_BINARY_FILTER);

	/*
	** Create the Software Bus command pipe and subscribe to housekeeping
	**  messages
	*/

	CFE_SB_CreatePipe(&cndh_CommandPipe, CNDH_PIPE_DEPTH, "CNDH_CMD_PIPE");

	CFE_SB_Subscribe(CNDH_CMD_MID, cndh_CommandPipe);
	CFE_SB_Subscribe(CNDH_SEND_HK_MID, cndh_CommandPipe);
	CFE_SB_Subscribe(CNDH_WAKE_UP_MID, cndh_CommandPipe);

	CNDH_ResetCounters();

	CFE_SB_InitMsg(&cndh_HkTelemetryPkt,
		CNDH_HK_TLM_MID,
		CNDH_APP_HK_TLM_LNGTH, TRUE);

	CFE_SB_InitMsg(&cndh_LogMsg,
        			CNDH_LOGMSG_MID,
					CNDH_LOG_LNGTH, TRUE);

	CFE_SB_InitMsg(&cndh_epsCmd,
					EPS_CMD_MID,
					CNDH_EPS_CMD_LNGTH, TRUE);

	CFE_SB_InitMsg(&cndh_comsCmd,
					COMS_CMD_MID,
					CNDH_COMS_CMD_LNGTH, TRUE);

	CFE_SB_InitMsg(&cndh_adcCmd,
					ADC_CMD_MID,
					CNDH_ADC_CMD_LNGTH, TRUE);

/*	CFE_SB_InitMsg(&cndh_gncCmd,
					GNC_CMD_MID,
					CNDH_GNC_CMD_LNGTH, TRUE);*/

    CFE_EVS_SendEvent (CNDH_STARTUP_INF_EID, CFE_EVS_INFORMATION,
               "template CNDH Initialized.");

    mode_flag.flag = 0;
    cndh_LogMsg.MTQ_Control_status = 0;
    cndh_LogMsg.Antenna_status = 0;

} /* End of template_AppInit() */


void CNDH_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(cndhMsgPtr);

    switch (MsgId)
    {
        case CNDH_CMD_MID:
        	CNDH_ProcessGroundCommand();
            break;

        case CNDH_SEND_HK_MID:
            CNDH_ReportHousekeeping();
            break;

        case CNDH_WAKE_UP_MID:
			CNDH_ProcessScheduleCommand();
			break;

        default:
            cndh_HkTelemetryPkt.cndh_command_error_count++;
            CFE_EVS_SendEvent(CNDH_COMMAND_ERR_EID,CFE_EVS_ERROR,
			"CNDH: invalid command packet,MID = 0x%x", MsgId);
            break;
    }

    return;

} /* End template_ProcessCommandPacket */

void CNDH_ProcessGroundCommand(void)
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(cndhMsgPtr);

    switch (CommandCode)
    {
        case CNDH_NOOP_CC:
            cndh_HkTelemetryPkt.cndh_command_count++;
            CFE_EVS_SendEvent(CNDH_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"CNDH, NOOP command\n");
            break;

        case CNDH_RESET_COUNTERS_CC:
        	CNDH_ResetCounters();

            break;

		case CNDH_COMMUNICATION_CC:
			break;

		case CNDH_GET_EPS_CC:
			cndh_HkTelemetryPkt.cndh_command_count++;
			eps_LogMsg = (eps_log_2_cndh *)cndhMsgPtr;
			CNDH_Save_data(1);
			OS_printf("CNDH, EPS data get\n");
			break;

		case CNDH_GET_ADC_CC:
			cndh_HkTelemetryPkt.cndh_command_count++;
		   adc_LogMsg = (adc_log *)cndhMsgPtr;
		   CNDH_Save_data(2);
			OS_printf("CNDH, ADCS data get\n");
			break;

		case CNDH_GET_COMS_CC:
			cndh_HkTelemetryPkt.cndh_command_count++;
			coms_LogMsg = (coms_log *)cndhMsgPtr;
			CNDH_Save_data(3);
			OS_printf("CNDH, COMS data get\n");
			break;
			/*
			 * here need to add details
			 */

        /* default case already found during FC vs length test */
        default:
            break;
    }
    return;

} /* End of template_ProcessGroundCommand() */


void CNDH_Save_data(int number)
{
	   switch (number)
	    {

	   case 1:
		   cndh_LocalLog.Soc = eps_LogMsg->Soc;
		   cndh_LocalLog.temp = eps_LogMsg->temp;
		   cndh_LocalLog.AX100_Status = eps_LogMsg->AX100_Status;
		break;

	   case 2:
		   cndh_LocalLog.IMU_Checksum = adc_LogMsg->IMU_Checksum;
		   cndh_LocalLog.MMT_Checksum = adc_LogMsg->MMT_Checksum;
		   cndh_LocalLog.FSS_Checksum = adc_LogMsg->FSS_Checksum;
			cndh_LocalLog.CSS_Checksum = adc_LogMsg->CSS_Checksum;
			cndh_LocalLog.AngVel[0] = adc_LogMsg->AngVel[0];
			cndh_LocalLog.AngVel[1] = adc_LogMsg->AngVel[1];
			cndh_LocalLog.AngVel[2] = adc_LogMsg->AngVel[2];
		break;

	   case 3:
		   cndh_LocalLog.AX100_Checksum = coms_LogMsg->AX100_Checksum;
		break;
	    }
}

void CNDH_ProcessScheduleCommand(void)
{
	switch (mode_flag.flag)
	{
		case 0:
			if (cndh_HkTelemetryPkt.cndh_command_count > 0)
				mode_flag.flag = 1;
			break;
		case 1: //separation mode
			Separation_Mode_1U();
			break;

		case 2: // detumbling mode
			Stablization_Mode_1U();
			break;

		case 3: // normal(EO) mode
			//normalEO_Mode_1U();
			break;
		case 6:

			break;
	}

	/*
	 * mode operation consider flag
	 */
}

void CNDH_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &cndh_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &cndh_HkTelemetryPkt);
    return;

} /* End of template_ReportHousekeeping() */
void CNDH_ResetCounters(void)
{
    /* Status of commands processed by the template App */
    cndh_HkTelemetryPkt.cndh_command_count       = 0;
    cndh_HkTelemetryPkt.cndh_command_error_count = 0;

    CFE_EVS_SendEvent(CNDH_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"CNDH, RESET command");
    return;

} /* End of template_ResetCounters() */

boolean CNDH_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
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

        CFE_EVS_SendEvent(CNDH_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        cndh_HkTelemetryPkt.cndh_command_error_count++;
    }

    return(result);

} /* End of template_VerifyCmdLength() */

void Separation_Mode_1U(void)
{
	cndh_LogMsg.mode_status = 0;
	OS_printf("Early orbit phase: Separation mode 1U entry\n");
	// default initialization

	if (cndh_LocalLog.Soc != 0)
	{
		OS_printf("CNDH, Current Soc is %d\n", cndh_LocalLog.Soc);
		if (cndh_LocalLog.temp != 0)
		{
			OS_printf("CNDH, Current bat temp is %d\n", cndh_LocalLog.temp);
			cndh_LogMsg.mode_status++;
		}
	}

	if (cndh_LocalLog.IMU_Checksum == 1)
	{
		if (cndh_LocalLog.MMT_Checksum ==1)
		{
			OS_printf("CNDH, IMU/MMT checksum done\n");
			cndh_LogMsg.mode_status++;
		}
	}

	if (cndh_LocalLog.FSS_Checksum == 1)
	{
		if (cndh_LocalLog.CSS_Checksum == 1)
		{
			OS_printf("CNDH, FSS/CSS checksum done\n");
			cndh_LogMsg.mode_status++;
		}
	}
	if (cndh_LocalLog.AX100_Status == 0)
	{
		CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &cndh_epsCmd, EPS_AX100_ON); //send command 3 to eps for AX100U on
		CFE_SB_SendMsg((CFE_SB_Msg_t *) &cndh_epsCmd);
	}

	if (cndh_LocalLog.AX100_Checksum == 1) //AX100U check sum result
	{
		OS_printf("CNDH, AX100 checksum done\n");
		cndh_LogMsg.mode_status++;
	}

	if (cndh_LogMsg.Antenna_status == 0)
	{
		//UHF antenna 1, LED1 12
		//UHF antenna 2, LED1 16
		//UHF antenna 3, LED1 20
		//UHF antenna 4, LED1 21

		GPIO_LibInit();

		GPIOLib_Export(12);
		GPIOLib_Direction(12,1);
		GPIOLib_Write(12,1);

		if (GPIOLib_Read(12) == 1)
			{
				OS_printf("UHF antenna 1 deploy done\n");
				cndh_LogMsg.mode_status++;
				//sleep(1);
				GPIOLib_Write(12,0);
			}

		GPIOLib_Export(16);
		GPIOLib_Direction(16,1);
		GPIOLib_Write(16,1);

		if (GPIOLib_Read(16) == 1)
			{
				OS_printf("UHF antenna 2 deploy done\n");
				cndh_LogMsg.mode_status++;
				//sleep(1);
				GPIOLib_Write(16,0);
			}

		GPIOLib_Export(20);
		GPIOLib_Direction(20,1);
		GPIOLib_Write(20,1);

		if (GPIOLib_Read(20) == 1)
			{
				OS_printf("UHF antenna 3 deploy done\n");
				cndh_LogMsg.mode_status++;
				//sleep(1);
				GPIOLib_Write(20,0);
			}

		GPIOLib_Export(21);
		GPIOLib_Direction(21,1);
		GPIOLib_Write(21,1);

		if (GPIOLib_Read(21) == 1)
			{
				OS_printf("UHF antenna 4 deploy done\n");
				cndh_LogMsg.mode_status++;
				//sleep(1);
				GPIOLib_Write(21,0);
			}
		GPIOLib_Unexport(12);
		GPIOLib_Unexport(16);
		GPIOLib_Unexport(20);
		GPIOLib_Unexport(21);
		GPIOLib_Close();
		cndh_LogMsg.Antenna_status = 1;
	}


	if (cndh_LogMsg.Antenna_status == 1)
	{
		cndh_LogMsg.mode_status++;
		cndh_LogMsg.mode_status++;
		cndh_LogMsg.mode_status++;
		cndh_LogMsg.mode_status++;
	}

	if (cndh_LocalLog.AX100_Checksum == 1) //AX100U check sum result
	{
		CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &cndh_comsCmd, 3); //send command 1 to coms for beacon
		CFE_SB_SendMsg((CFE_SB_Msg_t *) &cndh_comsCmd);
	}

	CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &cndh_adcCmd, 2); //send command 1 to adc for AD
	CFE_SB_SendMsg((CFE_SB_Msg_t *) &cndh_adcCmd);

	if (cndh_LogMsg.mode_status == 8)
	{
		mode_flag.flag = 2;
	}

}

void Stablization_Mode_1U(void)
{
	cndh_LogMsg.mode_status = 0;
	OS_printf("Early orbit phase: Stablization mode 1U entry\n");

	if (cndh_LocalLog.Soc != 0)
	{
		OS_printf("CNDH, Current Soc is %d\n", cndh_LocalLog.Soc);
		if (cndh_LocalLog.temp != 0)
		{
			OS_printf("CNDH, Current bat temp is %d\n", cndh_LocalLog.temp);
			cndh_LogMsg.mode_status++;
		}
	}

	if (cndh_LocalLog.IMU_Checksum == 1)
	{
		if (cndh_LocalLog.MMT_Checksum ==1)
		{
			OS_printf("CNDH, IMU/MMT checksum done\n");
			cndh_LogMsg.mode_status++;
		}
	}

	if (cndh_LocalLog.FSS_Checksum == 1)
	{
		if (cndh_LocalLog.CSS_Checksum == 1)
		{
			OS_printf("CNDH, FSS/CSS checksum done\n");
			cndh_LogMsg.mode_status++;
		}
	}

	if (cndh_LocalLog.AngVel[0] > 5 || cndh_LocalLog.AngVel[0] < -5)
		{
			if (cndh_LogMsg.MTQ_Control_status == 1)
				{
					OS_printf("CNDH, Detumbling On going\n");
				}
			if (cndh_LogMsg.MTQ_Control_status == 0)
				{
					OS_printf("CNDH, Detumbling Start\n");
					cndh_LogMsg.MTQ_Control_status = 1;

					CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) cndh_adcCmd.CmdHeader, 3); //MTQ control
					CFE_SB_SendMsg((CFE_SB_Msg_t *) &cndh_adcCmd);
				}
		}
	if (cndh_LocalLog.AngVel[0] < 5 && cndh_LocalLog.AngVel[0] > -5)
		{

			if (cndh_LogMsg.MTQ_Control_status == 0)
				{
					OS_printf("CNDH, Detumbling pass\n");
				}
			if (cndh_LogMsg.MTQ_Control_status == 1)
				{
					OS_printf("CNDH, Detumbling end\n");
					CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) cndh_adcCmd.CmdHeader, 4); //MTQ control
					CFE_SB_SendMsg((CFE_SB_Msg_t *) &cndh_adcCmd);
					cndh_LogMsg.MTQ_Control_status = 0;
				}

		}

	if (cndh_LocalLog.AX100_Checksum == 1) //AX100U check sum result
	{
		OS_printf("CNDH, AX100 checksum done\n");
		cndh_LogMsg.mode_status++;
	}

	if (cndh_LocalLog.AX100_Checksum == 1) //AX100U check sum result
	{
		CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &cndh_comsCmd, 3); //send command 1 to coms for beacon
		CFE_SB_SendMsg((CFE_SB_Msg_t *) &cndh_comsCmd);
	}

	if (cndh_LogMsg.mode_status == 77)
	{
		mode_flag.flag = 3;
	}
}

/************************/
/*  End of File Comment */
/************************/
