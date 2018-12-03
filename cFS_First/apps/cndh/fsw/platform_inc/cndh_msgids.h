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
#define CNDH_GET_ADC_CC              4
#define CNDH_GET_COMS_CC             5

/**************************
** CNDH Command Message IDs
***************************/

#define CNDH_CMD_MID                     0x1900 /**< \brief HK Ground Commands Message ID */
#define CNDH_SEND_HK_MID                 0x1901 /**< \brief HK Send Housekeeping Data Cmd Message ID */
#define CNDH_WAKE_UP_MID				 0x1902
#define CNDH_LOGMSG_MID					 0x1903


// send command to other subsystems
//#define EPS_CMD_MID				 		 0x1930
//#define ADC_CMD_MID				 		 0x1910


/***************************
** CNDH Telemetry Message IDs
****************************/

#define CNDH_HK_TLM_MID                  0x0900 /**< \brief HK Housekeeping Telemetry Message ID */


#endif /* _cndh_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
