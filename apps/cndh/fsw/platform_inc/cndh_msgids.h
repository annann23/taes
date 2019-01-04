/************************************************************************
** File: cndh_msgids.h
**   
** Author:
**
** Purpose: 
**  CNDH Application Message IDs header file
**
** Notes:
**
** $Log: cndh_msgids.h  $
**
**
*************************************************************************/
#ifndef _cndh_msgids_h_
#define _cndh_msgids_h_


/*
** App command codes
*/
#define CNDH_NOOP_CC                 0
#define CNDH_RESET_COUNTERS_CC       1
#define CNDH_COMMUNICATION_CC        2
#define CNDH_GET_EPS_CC              3
#define CNDH_GET_ADCS_CC             4
#define CNDH_GET_COMS_CC             5
#define CNDH_GET_EPS_ANTENNA_CC      6

/**************************
** CNDH Command Message IDs
**************************/

#define CNDH_CMD_MID                     0x1900
#define CNDH_SEND_HK_MID                 0x1901
#define CNDH_WAKE_UP_MID				 0x1902
#define CNDH_LOGMSG_MID					 0x1903

/***************************
** CNDH Telemetry Message IDs
****************************/

#define CNDH_HK_TLM_MID                  0x0900

typedef struct
{
	uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
	uint16	Antenna_number;
} cndh_log_eps_t;
#define CNDH_LOG_EPS_LENGTH   sizeof ( cndh_log_eps_t )



#endif /* _cndh_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
