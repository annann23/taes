/************************************************************************
** File: eps_msgids.h
**   
** Aythor:
**
** Purpose: 
**  ADC Application Message IDs header file
**
** Notes:
**
** $Log: eps_msgids.h  $
**
**
*************************************************************************/
#ifndef _eps_msgids_h_
#define _eps_msgids_h_

/*
** App command codes
*/
#define EPS_NOOP_CC                 0
#define EPS_RESET_COUNTERS_CC       1
#define EPS_FSS_CSS_ON              2
#define EPS_AX100_ON              	3
#define EPS_MTQ_ON              	4
#define EPS_MTQ_OFF              	5
/**************************
** EPS Command Message IDs
***************************/

#define EPS_CMD_MID                     0x1930 /**< \brief HK Ground Commands Message ID */
#define EPS_SEND_HK_MID                 0x1931 /**< \brief HK Send Housekeeping Data Cmd Message ID */
#define EPS_WAKE_UP_MID                 0x1932
#define EPS_LOGMSG_MID                  0x1933


/***************************
** EPS Telemetry Message IDs
****************************/

#define EPS_HK_TLM_MID                  0x0930 /**< \brief HK Housekeeping Telemetry Message ID */

typedef struct
{
	uint8   CmdCHeader[CFE_SB_CMD_HDR_SIZE];
	uint16 	Soc;
	uint16 	temp;
	uint16	AX100_Status;
} eps_log_2_cndh;

#define EPS_LOG_LNGTH   sizeof ( eps_log_2_cndh )

typedef struct
{
	uint8   CmdCHeader[CFE_SB_CMD_HDR_SIZE];
	//Add sturct later
} eps_log_2_adcs;

#define EPS_LOG2_LNGTH   sizeof ( eps_log_2_adcs )

typedef struct
{
	uint8   CmdCHeader[CFE_SB_CMD_HDR_SIZE];
	uint16	AX100_Status;
} eps_log_2_coms;

#define EPS_LOG2_LNGTH   sizeof ( eps_log_2_coms )

#endif /* _eps_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
