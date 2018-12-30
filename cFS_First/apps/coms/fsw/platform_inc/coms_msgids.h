/************************************************************************
** File: coms_msgids.h
**   
** Aythor:
**
** Purpose: 
**  COMS Application Message IDs header file
**
** Notes:
**
** $Log: coms_msgids.h  $
**
**
*************************************************************************/
#ifndef _coms_msgids_h_
#define _coms_msgids_h_

/**************************
** COMS Command Message IDs
***************************/

#define COMS_CMD_MID                     0x1940 /**< \brief HK Ground Commands Message ID */
#define COMS_SEND_HK_MID                 0x1941 /**< \brief HK Send Housekeeping Data Cmd Message ID */
#define COMS_WAKE_UP_MID                 0x1942
#define COMS_LOGMSG_MID                  0x1943

/***************************
** COMS Telemetry Message IDs
****************************/

#define COMS_HK_TLM_MID                  0x0940 /**< \brief HK Housekeeping Telemetry Message ID */

/*
** App command codes
*/
#define COMS_APP_NOOP_CC                 0
#define COMS_APP_RESET_COUNTERS_CC       1
#define COMS_GROUND_COMMAND              2
#define COMS_TRANSMIT_BEACON             3
#define COMS_GET_EPS_CC					 4
#define COMS_SEND_HK					 5

/* global coms data structure */

typedef struct
{
	uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
	uint16 	AX100_Checksum;
	uint32	time;
	uint16 	flag;
} coms_log_cndh_t;
#define COMS_LOG_CNDH_LENGTH   sizeof ( coms_log_cndh_t )

#endif /* _coms_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
