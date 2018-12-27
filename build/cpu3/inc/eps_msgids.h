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


/*
** App command codes
*/
#define EPS_NOOP_CC                 0
#define EPS_RESET_COUNTERS_CC       1
#define EPS_FSS_CSS_ON              2
#define EPS_AX100_ON              	3
#define EPS_MTQ_ON              	4
#define EPS_MTQ_OFF              	5

// global data structure
typedef struct
{
	uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
	uint16 	Soc;
	uint16 	temp;
	uint16	AX100_Status;
} eps_log_cndh_t;
#define EPS_LOG_CNDH_LENGTH   sizeof ( eps_log_cndh_t )

typedef struct
{
	uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
	uint16	AX100_Status;
} eps_log_coms_t;
#define EPS_LOG_COMS_LENGTH   sizeof ( eps_log_coms_t  )

#endif /* _eps_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
