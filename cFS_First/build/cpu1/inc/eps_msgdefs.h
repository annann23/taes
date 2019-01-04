/*******************************************************************************
** File:
**   $Id: eps_msg.h 2017/02/02 14:55:00 wschoi  Exp  $
**
** Purpose: 
**  Define EPS app Messages and info
**
** Notes:
**
** $Log: eps_msg.h  
** Initial revision
**
*******************************************************************************/
#ifndef _eps_msgdefs_h_
#define _eps_msgdefs_h_

#include "cfe.h"
/*
** EPS command codes
*/
#define EPS_NOOP_CC                 0
#define EPS_RESET_COUNTERS_CC       1
#define EPS_SLEEP  2
#define EPS_RESUME 3
#define EPS_DEPLOY_SOLAR_PANEL_CC 		4

#define EPS_SWITCH_ALL_PDM_ON_CC 				10
#define EPS_SWITCH_ALL_PDM_OFF_CC				11
#define EPS_SWITCH_N_PDM_ON_CC					12
#define EPS_SWITCH_N_PDM_OFF_CC					13
#define EPS_SET_N_PDM_TIMER_CC					14
#define EPS_SET_WATCHDOG_PERIOD_CC				15
#define EPS_RESET_PCM_CC						16
#define EPS_SET_PDM_INITIAL_STATE				17


#define EPS_PDM_SWITCH_FLAG_1 0x02
#define EPS_PDM_SWITCH_FLAG_2 0x04
#define EPS_PDM_SWITCH_FLAG_3 0x08
#define EPS_PDM_SWITCH_FLAG_4 0x10
#define EPS_PDM_SWITCH_FLAG_5 0x20
#define EPS_PDM_SWITCH_FLAG_6 0x40
#define EPS_PDM_SWITCH_FLAG_7 0x80
#define EPS_PDM_SWITCH_FLAG_8 0x100
#define EPS_PDM_SWITCH_FLAG_9 0x200
#define EPS_PDM_SWITCH_FLAG_10 0x400


////  EPS PDM SWITCH

#define EPS_PDM_SWITCH_CAMERA		EPS_PDM_SWITCH_FLAG_1
#define EPS_PDM_SWTICH_RW			EPS_PDM_SWITCH_FLAG_2
#define EPS_PDM_SWITCH_SBAND_OFF	EPS_PDM_SWITCH_FLAG_5
#define EPS_PDM_SWITCH_STS			EPS_PDM_SWITCH_FLAG_6
#define EPS_PDM_SWTICH_PAYLOAD		EPS_PDM_SWITCH_FLAG_7
/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8          CmdHeader[CFE_SB_CMD_HDR_SIZE];

}EPS_NoArgsCmd_t;


typedef struct
{
	uint8			CmdHeader[CFE_SB_CMD_HDR_SIZE];
	uint16			channelFlag;

}EPS_PDMControlCmd_t;


typedef struct
{
	uint8			CmdHeader[CFE_SB_CMD_HDR_SIZE];
	uint16			channel;
	uint16			period;

}EPS_SetPDMTimerCmd_t;

typedef struct
{
	uint8			CmdHeader[CFE_SB_CMD_HDR_SIZE];
	uint16			flag;

}EPS_PCMResetCmd_t;

typedef struct
{
	uint8			CmdHeader[CFE_SB_CMD_HDR_SIZE];
	uint16			minute;

}EPS_SetWatchdogPeriodCmd_t;

typedef struct
{
	uint8			CmdHeader[CFE_SB_CMD_HDR_SIZE];
	uint16			channel;

}EPS_ResetPCMCmd_t;

/*************************************************************************/
/*
** Type definition 
*/


#endif /* _eps_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
