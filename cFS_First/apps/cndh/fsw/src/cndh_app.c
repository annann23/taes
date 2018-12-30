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

/* Includes */

// local application code include
#include "cndh_app.h"

// global application code include
#include "eps_msgids.h"
#include "coms_msgids.h"
#include "adcs_msgids.h"
//#include "gnc_msgids.h"

// library and other code include
#include <stdio.h>
#include "gpio_lib.h"

/* CNDH global data */

// local application data
cndh_flag_t		 	cndh_flag;
cndh_hk_tlm_t    	cndh_hk_tlm;
cndh_log_t     	 	cndh_log;
cndh_locallog_t	 	cndh_locallog;

// command list
cndh_cmd_eps_t		cndh_cmd_eps;
cndh_cmd_coms_t		cndh_cmd_coms;
cndh_cmd_adc_t		cndh_cmd_adc;
cndh_cmd_gnc_t		cndh_cmd_gnc;

// receive log data list
eps_log_cndh_t		*eps_log_cndh;
adcs_log_cndh_t		*adcs_log_cndh;
coms_log_cndh_t		*coms_log_cndh;

// application pointers
CFE_SB_PipeId_t    	cndh_CommandPipe;
CFE_SB_MsgPtr_t    	cndhMsgPtr;

// event filter (on-going)
static CFE_EVS_BinFilter_t  CNDH_EventFilters[] =
       {  /* Event ID    mask */
          {CNDH_STARTUP_INF_EID,       0x0000},
          {CNDH_COMMAND_ERR_EID,       0x0000},
          {CNDH_COMMANDNOP_INF_EID,    0x0000},
          {CNDH_COMMANDRST_INF_EID,    0x0000},
       };

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CNDH application entry point and main process loop              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CNDH_AppMain( void )
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(CNDH_APPMAIN_PERF_ID);

    CNDH_AppInit();

    /* cndh Run loop */
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

}

void CNDH_AppInit(void)
{
	CFE_ES_RegisterApp();

	CFE_EVS_Register(CNDH_EventFilters,
		sizeof(CNDH_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
		CFE_EVS_BINARY_FILTER);

	/* Create the Software Bus command pipe and subscribe to housekeeping messages */

	CFE_SB_CreatePipe(&cndh_CommandPipe, CNDH_PIPE_DEPTH, "CNDH_CMD_PIPE");

	CFE_SB_Subscribe(CNDH_CMD_MID, cndh_CommandPipe);
	CFE_SB_Subscribe(CNDH_SEND_HK_MID, cndh_CommandPipe);
	CFE_SB_Subscribe(CNDH_WAKE_UP_MID, cndh_CommandPipe);

	CNDH_ResetCounters();

	CFE_SB_InitMsg(&cndh_hk_tlm,
		CNDH_HK_TLM_MID,
		CNDH_APP_HK_TLM_LENGTH, TRUE);

	CFE_SB_InitMsg(&cndh_cmd_eps,
					EPS_CMD_MID,
					CNDH_CMD_EPS_LENGTH, TRUE);

	CFE_SB_InitMsg(&cndh_cmd_coms,
					COMS_CMD_MID,
					CNDH_CMD_COMS_LENGTH, TRUE);

	CFE_SB_InitMsg(&cndh_cmd_adc,
					ADCS_CMD_MID,
					CNDH_CMD_ADC_LENGTH, TRUE);

/*	CFE_SB_InitMsg(&cndh_cmd_gnc,
					GNC_CMD_MID,
					CNDH_CMD_GNC_LENGTH, TRUE);*/

    CFE_EVS_SendEvent (CNDH_STARTUP_INF_EID, CFE_EVS_INFORMATION,
               "template CNDH Initialized.");

    cndh_flag.flag = 0;
    cndh_log.MTQ_Control_status = 0;
    cndh_log.Antenna_status = 0;

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
        	cndh_hk_tlm.cndh_command_error_count++;
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
        	cndh_hk_tlm.cndh_command_count++;
            CFE_EVS_SendEvent(CNDH_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"CNDH, NOOP command\n");
            break;

        case CNDH_RESET_COUNTERS_CC:
        	CNDH_ResetCounters();
            break;

		case CNDH_COMMUNICATION_CC:
			cndh_hk_tlm.cndh_command_count++;
			OS_printf("L1:CNDH, Communication mode entry\n");
			coms_log_cndh = (coms_log_cndh_t *)cndhMsgPtr;
			CNDH_Save_data(4);
			CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &cndh_cmd_coms, COMS_SEND_HK);
			CFE_SB_SendMsg((CFE_SB_Msg_t *) &cndh_cmd_coms);
			break;

		case CNDH_GET_EPS_CC:
			cndh_hk_tlm.cndh_command_count++;
			eps_log_cndh = (eps_log_cndh_t *)cndhMsgPtr;
			CNDH_Save_data(1);
			OS_printf("L5:CNDH, EPS data get\n");
			break;

		case CNDH_GET_ADCS_CC:
			cndh_hk_tlm.cndh_command_count++;
			adcs_log_cndh = (adcs_log_cndh_t *)cndhMsgPtr;
			CNDH_Save_data(2);
			OS_printf("L5:CNDH, ADCS data get\n");
			break;

		case CNDH_GET_COMS_CC:
			cndh_hk_tlm.cndh_command_count++;
			coms_log_cndh = (coms_log_cndh_t *)cndhMsgPtr;
			CNDH_Save_data(3);
			OS_printf("L5:CNDH, COMS data get\n");
			break;

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
		   cndh_locallog.Soc = eps_log_cndh->Soc;
		   cndh_locallog.temp = eps_log_cndh->temp;
		   cndh_locallog.AX100_Status = eps_log_cndh->AX100_Status;
		   break;

	   case 2:
		   cndh_locallog.IMU_Checksum = adcs_log_cndh->IMU_Checksum;
		   cndh_locallog.MMT_Checksum = adcs_log_cndh->MMT_Checksum;
		   cndh_locallog.FSS_Checksum = adcs_log_cndh->FSS_Checksum;
		   cndh_locallog.CSS_Checksum = adcs_log_cndh->CSS_Checksum;
		   cndh_locallog.AngVel[0] = adcs_log_cndh->AngVel[0];
		   cndh_locallog.AngVel[1] = adcs_log_cndh->AngVel[1];
		   cndh_locallog.AngVel[2] = adcs_log_cndh->AngVel[2];
		   break;

	   case 3:
		   cndh_locallog.AX100_Checksum = coms_log_cndh->AX100_Checksum;
		   break;

	   case 4:
		   cndh_locallog.Ground_time = coms_log_cndh->time;
		   cndh_flag.flag = coms_log_cndh->flag;
		   break;
	   }
}

void CNDH_ProcessScheduleCommand(void)
{
	switch (cndh_flag.flag)
	{
		case 0:
			if (cndh_hk_tlm.cndh_command_count > 0)
				cndh_flag.flag = 1;
			break;

		case 1: //separation mode
			Separation_Mode_1U();
			break;

		case 2: // detumbling mode
			Stablization_Mode_1U();
			break;

		case 3: // normal(EO) mode
			normalEO_Mode_1U();
			break;

		case 4: //commissioning 1 (GPS)
			OS_printf("L1:Early orbit phase: Commissioning 1 (GPS) mode 1U entry\n");
			OS_printf("CNDH, Commissioning 1 (GPS) done\n");
			cndh_flag.flag = 3;
			break;
	}
}

void CNDH_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &cndh_hk_tlm);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &cndh_hk_tlm);
    return;

}

void CNDH_ResetCounters(void)
{
    /* Status of commands processed by the template App */
	cndh_hk_tlm.cndh_command_count       = 0;
	cndh_hk_tlm.cndh_command_error_count = 0;

    CFE_EVS_SendEvent(CNDH_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"CNDH, RESET command");
    return;

}

boolean CNDH_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
{
    boolean result = TRUE;

    uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

    /* Verify the command packet length. */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID   = CFE_SB_GetMsgId(msg);
        uint16         CommandCode = CFE_SB_GetCmdCode(msg);

        CFE_EVS_SendEvent(CNDH_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        cndh_hk_tlm.cndh_command_error_count++;
    }
    return(result);

}

void Separation_Mode_1U(void)
{
	cndh_log.mode_status = 0;
	OS_printf("L1:Early orbit phase: Separation mode 1U entry\n");

	if (cndh_locallog.Soc != 0)
	{
		OS_printf("L0:CNDH, Current Soc is %d\n", cndh_locallog.Soc);
		if (cndh_locallog.temp != 0)
		{
			OS_printf("L0:CNDH, Current bat temp is %d\n", cndh_locallog.temp);
			cndh_log.mode_status++;
		}
	}

	if (cndh_locallog.IMU_Checksum == 1)
	{
		if (cndh_locallog.MMT_Checksum ==1)
		{
			OS_printf("L2:CNDH, IMU/MMT checksum done\n");
			cndh_log.mode_status++;
		}
	}

	if (cndh_locallog.FSS_Checksum == 1)
	{
		if (cndh_locallog.CSS_Checksum == 1)
		{
			OS_printf("L2:CNDH, FSS/CSS checksum done\n");
			cndh_log.mode_status++;
		}
	}
	if (cndh_locallog.AX100_Status == 0)
	{
		CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &cndh_cmd_eps, EPS_AX100_ON);
		CFE_SB_SendMsg((CFE_SB_Msg_t *) &cndh_cmd_eps);
	}

	if (cndh_locallog.AX100_Checksum == 1) //AX100U check sum result
	{
		OS_printf("L2:CNDH, AX100 checksum done\n");
		cndh_log.mode_status++;
	}

	if (cndh_log.Antenna_status == 0) // need to change
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
				OS_printf("L3:UHF antenna 1 deploy done\n");
				cndh_log.mode_status++;
				//sleep(1);
				GPIOLib_Write(12,0);
			}

		GPIOLib_Export(16);
		GPIOLib_Direction(16,1);
		GPIOLib_Write(16,1);

		if (GPIOLib_Read(16) == 1)
			{
				OS_printf("L3:UHF antenna 2 deploy done\n");
				cndh_log.mode_status++;
				//sleep(1);
				GPIOLib_Write(16,0);
			}

		GPIOLib_Export(20);
		GPIOLib_Direction(20,1);
		GPIOLib_Write(20,1);

		if (GPIOLib_Read(20) == 1)
			{
				OS_printf("L3:UHF antenna 3 deploy done\n");
				cndh_log.mode_status++;
				//sleep(1);
				GPIOLib_Write(20,0);
			}

		GPIOLib_Export(21);
		GPIOLib_Direction(21,1);
		GPIOLib_Write(21,1);

		if (GPIOLib_Read(21) == 1)
			{
				OS_printf("L3:UHF antenna 4 deploy done\n");
				cndh_log.mode_status++;
				//sleep(1);
				GPIOLib_Write(21,0);
			}
		GPIOLib_Unexport(12);
		GPIOLib_Unexport(16);
		GPIOLib_Unexport(20);
		GPIOLib_Unexport(21);
		GPIOLib_Close();
		cndh_log.Antenna_status = 1;
	}


	if (cndh_log.Antenna_status == 1)
	{
		cndh_log.mode_status++;
		cndh_log.mode_status++;
		cndh_log.mode_status++;
		cndh_log.mode_status++;
	}

	if (cndh_locallog.AX100_Checksum == 1) //AX100U check sum result
	{
		CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &cndh_cmd_coms, COMS_TRANSMIT_BEACON); //send command 1 to coms for beacon
		CFE_SB_SendMsg((CFE_SB_Msg_t *) &cndh_cmd_coms);
	}

	CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &cndh_cmd_adc, ADCS_AD_COMMAND); //send command 1 to adc for AD
	CFE_SB_SendMsg((CFE_SB_Msg_t *) &cndh_cmd_adc);

	if (cndh_log.mode_status == 8)
	{
		cndh_flag.flag = 2;
	}

}

void Stablization_Mode_1U(void)
{
	cndh_log.mode_status = 0;
	OS_printf("L1:Early orbit phase: Stablization mode 1U entry\n");

	if (cndh_locallog.Soc != 0)
	{
		OS_printf("L0:CNDH, Current Soc is %d\n", cndh_locallog.Soc);
		if (cndh_locallog.temp != 0)
		{
			OS_printf("L0:CNDH, Current bat temp is %d\n", cndh_locallog.temp);
			cndh_log.mode_status++;
		}
	}

	if (cndh_locallog.IMU_Checksum == 1)
	{
		if (cndh_locallog.MMT_Checksum ==1)
		{
			OS_printf("L2:CNDH, IMU/MMT checksum done\n");
			cndh_log.mode_status++;
		}
	}

	if (cndh_locallog.FSS_Checksum == 1)
	{
		if (cndh_locallog.CSS_Checksum == 1)
		{
			OS_printf("L2:CNDH, FSS/CSS checksum done\n");
			cndh_log.mode_status++;
		}
	}

	if (cndh_locallog.AngVel[0] > 5 || cndh_locallog.AngVel[0] < -5)
		{
			if (cndh_log.MTQ_Control_status == 1)
				{
					OS_printf("L3:CNDH, Detumbling On going\n");
				}
			if (cndh_log.MTQ_Control_status == 0)
				{
					OS_printf("L3:CNDH, Detumbling Start\n");
					cndh_log.MTQ_Control_status = 1;

					CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) cndh_cmd_adc.CmdHeader, ADCS_DETUMB_START_COMMAND); //MTQ control
					CFE_SB_SendMsg((CFE_SB_Msg_t *) &cndh_cmd_adc);
				}
		}

	if (cndh_locallog.AngVel[0] < 5 && cndh_locallog.AngVel[0] > -5)
		{

			if (cndh_log.MTQ_Control_status == 0)
				{
					OS_printf("L3:CNDH, Detumbling pass\n");
				}
			if (cndh_log.MTQ_Control_status == 1)
				{
					OS_printf("L3:CNDH, Detumbling end\n");
					CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) cndh_cmd_adc.CmdHeader, ADCS_DETUMB_END_COMMAND); //MTQ control
					CFE_SB_SendMsg((CFE_SB_Msg_t *) &cndh_cmd_adc);
					cndh_log.MTQ_Control_status = 0;
				}
		}

	if (cndh_locallog.AX100_Checksum == 1) //AX100U check sum result
	{
		OS_printf("L2:CNDH, AX100 checksum done\n");
		cndh_log.mode_status++;
	}

	if (cndh_locallog.AX100_Checksum == 1) //AX100U check sum result
	{
		CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &cndh_cmd_coms, COMS_TRANSMIT_BEACON); //send command 1 to coms for beacon
		CFE_SB_SendMsg((CFE_SB_Msg_t *) &cndh_cmd_coms);
	}

	if (cndh_log.mode_status == 4)
	{
		cndh_flag.flag = 3;
	}
}

void normalEO_Mode_1U(void)
{
	cndh_log.mode_status = 0;

	OS_printf("L1:Early orbit phase: Normal mode 1U entry\n");
	if (cndh_locallog.Soc != 0)
	{
		OS_printf("L0:CNDH, Current Soc is %d\n", cndh_locallog.Soc);
		if (cndh_locallog.temp != 0)
		{
			OS_printf("L0:CNDH, Current bat temp is %d\n", cndh_locallog.temp);
			cndh_log.mode_status++;
		}
	}

	if (cndh_locallog.IMU_Checksum == 1)
	{
		if (cndh_locallog.MMT_Checksum ==1)
		{
			OS_printf("L2:CNDH, IMU/MMT checksum done\n");
			cndh_log.mode_status++;
		}
	}

	if (cndh_locallog.FSS_Checksum == 1)
	{
		if (cndh_locallog.CSS_Checksum == 1)
		{
			OS_printf("L2:CNDH, FSS/CSS checksum done\n");
			cndh_log.mode_status++;
		}
	}

	if (cndh_locallog.AX100_Checksum == 1) //AX100U check sum result
		{
			OS_printf("L2:CNDH, AX100 checksum done\n");
			cndh_log.mode_status++;
		}

		if (cndh_locallog.AX100_Checksum == 1) //AX100U check sum result
		{
			CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &cndh_cmd_coms, COMS_TRANSMIT_BEACON); //send command 1 to coms for beacon
			CFE_SB_SendMsg((CFE_SB_Msg_t *) &cndh_cmd_coms);
		}
}

/************************/
/*  End of File Comment */
/************************/
