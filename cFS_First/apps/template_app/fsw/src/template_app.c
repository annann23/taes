/*******************************************************************************
** File: app_Template.c
**
** Purpose:
**   This file contains the source code for the App template.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "template_app.h"              
#include "template_app_perfids.h"
#include "template_app_msgids.h"
#include "template_app_msg.h"
#include "template_app_events.h"
#include <stdio.h>

/*
** global data
*/

template_hk_tlm_t    	template_HkTelemetryPkt;
template_log     		template_LogMsg;

CFE_SB_PipeId_t    template_CommandPipe;
CFE_SB_MsgPtr_t    templateMsgPtr;

static CFE_EVS_BinFilter_t  template_EventFilters[] =
       {  /* Event ID    mask */
          {TEMPLATE_STARTUP_INF_EID,       0x0000},
          {TEMPLATE_COMMAND_ERR_EID,       0x0000},
          {TEMPLATE_COMMANDNOP_INF_EID,    0x0000},
          {TEMPLATE_COMMANDRST_INF_EID,    0x0000},
       };

/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *	*/
/* template_AppMain() -- Application entry point and main process loop          */
/*                            													*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * *	*/
void TEMPLATE_AppMain(void)
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(APP_TEMP_PERF_ID);

    TEMPLATE_AppInit();

    /*
    ** template Run loop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        CFE_ES_PerfLogExit(APP_TEMP_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&templateMsgPtr, template_CommandPipe, 500);
        
        CFE_ES_PerfLogEntry(APP_TEMP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
			TEMPLATE_ProcessCommandPacket();
        }

    }

    CFE_ES_ExitApp(RunStatus);

} /* End of template_AppMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 	*/
/*                                                                            	*/
/*template_AppInit() --  initialization                                         */
/*                            													*/
/*                                             									*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *	*/
void TEMPLATE_AppInit(void)
{
	/*
	** Register the app with Executive services
	*/
	CFE_ES_RegisterApp();

	/*
	** Register the events
	*/
	CFE_EVS_Register(template_EventFilters,
		sizeof(template_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
		CFE_EVS_BINARY_FILTER);

	/*
	** Create the Software Bus command pipe and subscribe
	*/

	CFE_SB_CreatePipe(&template_CommandPipe, TEMPLATE_PIPE_DEPTH, "TEMPLATE_CMD_PIPE");

	CFE_SB_Subscribe(TEMPLATE_CMD_MID_APP, template_CommandPipe);
	CFE_SB_Subscribe(TEMPLATE_SEND_HK_MID_APP, template_CommandPipe);
	CFE_SB_Subscribe(TEMPLATE_WAKE_UP_MID_APP, template_CommandPipe);
	/* user code start */

	/* user code end   */

	TEMPLATE_ResetCounters();

	/*
	 * Initialize for send table data to SB
	 */
	CFE_SB_InitMsg(&template_HkTelemetryPkt,
					TEMPLATE_HK_TLM_MID_APP,
					TEMPLATE_APP_HK_TLM_LNGTH, TRUE);

	CFE_SB_InitMsg(&template_LogMsg,
        			TEMPLATE_LOGMSG_MID_APP,
					TEMPLATE_APP_LOGMSG_LNGTH, TRUE);

    CFE_EVS_SendEvent (TEMPLATE_STARTUP_INF_EID, CFE_EVS_INFORMATION,//Initializing
               "template App Initialized. Version %d.%d.%d.%d");
				
} /* End of template_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  template_ProcessCommandPacket                                        */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the template    */
/*     command pipe.                                                          */
/*                                        */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void TEMPLATE_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(templateMsgPtr); //

    switch (MsgId)  //
    {
        case TEMPLATE_CMD_MID_APP: // Command from ground system
        	TEMPLATE_ProcessGroundCommand();
            break;

        case TEMPLATE_SEND_HK_MID_APP:// HK data send command
            TEMPLATE_ReportHousekeeping();
            break;

        case TEMPLATE_WAKE_UP_MID_APP: // Command from SCH
        	TEMPLATE_ProcessScheduleCommand();
        	break;

        default:
            template_HkTelemetryPkt.template_command_error_count++;
            CFE_EVS_SendEvent(TEMPLATE_COMMAND_ERR_EID,CFE_EVS_ERROR,
			"template: invalid command packet,MID = 0x%x", MsgId);
            break;
    }

    return;

} /* End template_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *	*/
/*                                                                            	*/
/* template_ProcessGroundCommand() -- template ground commands                  */
/*                                                    							*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *	*/

void TEMPLATE_ProcessGroundCommand(void)
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(templateMsgPtr);

    switch (CommandCode)
    {
        case TEMPLATE_APP_NOOP_CC:
            template_HkTelemetryPkt.template_command_count++;
            CFE_EVS_SendEvent(TEMPLATE_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"template: NOOP command");
            break;

        case TEMPLATE_APP_RESET_COUNTERS_CC:
        	TEMPLATE_ResetCounters();
            break;

		case TEMPLATE_APP_TEST_CC:
			/* example: Simple Led on/off operation using GPIO Pin, and read pin status */
			printf("RasberryPi - LED Blink|n");
			// GPIO pin number setting
			#define LED1 20
			#define LED2 21
			// initial setting
			GPIO_LibInit();
			GPIO_Lib_Export(LED1);
			GPIO_Lib_Export(LED2);

			GPIO_Lib_Direction(LED1,1);
			GPIO_Lib_Direction(LED2,1);

			// LED on/off operation
			for (int i=0;i<10;i++)
			{
				GPIO_Lib_Write(LED1,0);
				GPIO_Lib_Write(LED2,1);
				sleep(1);
				// send data using HK
				template_HkTelemetryPkt.led1_state+=GPIO_Lib_Read(LED1); // Read GPIO Pin data
				template_HkTelemetryPkt.led2_state+=GPIO_Lib_Read(LED2);

				GPIO_Lib_Write(LED1,1);
				GPIO_Lib_Write(LED2,0);
				sleep(1);

				template_HkTelemetryPkt.led1_state+=GPIO_Lib_Read(LED1);
				template_HkTelemetryPkt.led2_state+=GPIO_Lib_Read(LED2);
			}
			// Close GPIO_Lib
			GPIO_Lib_Unexport(LED1);
			GPIO_Lib_Unexport(LED2);
			GPIO_Lib_Close();

			/* user code start */

			/* user code end   */
			break;

        default:
            break;
    }
    return;

} /* End of template_ProcessGroundCommand() */

void TEMPLATE_ProcessScheduleCommand(void)
{
	/* example code: get time data and send to DS for periodic data save */
	OS_printf("CURRENT TIME IS : %d\n", CFE_TIME_GetTime().Seconds); // display message on Terminal
	if (template_LogMsg.LogMsg==NULL)
		template_LogMsg.LogMsg = "CURRENT TIME IS : ";  // set log data
	template_LogMsg.template_Time =  CFE_TIME_GetTime().Seconds; // save time data at 'template_LogMsg' table 'template_Time' slot
	CFE_SB_SendMsg((CFE_SB_Msg_t *) &template_LogMsg);  //Send data

	/* user code start */

	/* user code end   */

} /* End of template_ProcessScheduleCommand()  */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*  Name:  template_ReportHousekeeping                                        */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/*                               											  */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */

void TEMPLATE_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &template_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &template_HkTelemetryPkt);
    return;

} /* End of template_ReportHousekeeping() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  template_ResetCounters                                             */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                											  */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void TEMPLATE_ResetCounters(void)
{
    /* Status of commands processed by the template App */
    template_HkTelemetryPkt.template_command_count       = 0;
    template_HkTelemetryPkt.template_command_error_count = 0;

    CFE_EVS_SendEvent(TEMPLATE_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"template: RESET command");
    return;

} /* End of template_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* template_VerifyCmdLength() -- Verify command packet length                 */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
boolean TEMPLATE_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
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

        CFE_EVS_SendEvent(TEMPLATE_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        template_HkTelemetryPkt.template_command_error_count++;
    }

    return(result);

} /* End of template_VerifyCmdLength() */
