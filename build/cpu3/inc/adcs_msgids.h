/************************************************************************
** File: adcs_msgids.h
**   
** Aythor:
**
** Purpose: 
**  ADCS Application Message IDs header file
**
** Notes:
**
** $Log: adcs_msgids.h  $
**
**
*************************************************************************/
#ifndef _adcs_msgids_h_
#define _adcs_msgids_h_

/**************************
** ADCS Command Message IDs
***************************/

#define ADCS_CMD_MID                     0x1910 /**< \brief HK Ground Commands Message ID */
#define ADCS_SEND_HK_MID                 0x1911 /**< \brief HK Send Housekeeping Data Cmd Message ID */
#define ADCS_WAKE_UP_MID	                0x1912
#define ADCS_LOGMSG_MID                  0x1913


/***************************
** ADCS Telemetry Message IDs
****************************/

#define ADCS_HK_TLM_MID                  0x0910 /**< \brief HK Housekeeping Telemetry Message ID */

/*
** App command codes
*/
#define ADCS_APP_NOOP_CC                 0
#define ADCS_APP_RESET_COUNTERS_CC       1
#define ADCS_AD_COMMAND                  2
#define ADCS_DETUMB_START_COMMAND		3
#define ADCS_DETUMB_END_COMMAND			4
#define ADCS_GET_EPS_CC					5


typedef struct
{
	uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE];
	uint16   IMU;
	uint16	 IMU_Checksum;
	uint16   MMT;
	uint16   MMT_Checksum;
	uint16	 FSS_Checksum;
	uint16	 CSS_Checksum;
	uint16	 AngularVel;
	int16    Accel[2];
	int16    Temp;
	int16    AngVel[2];
	uint8	 MTQ_Control_status;
	boolean  adcs_detumb;
} adcs_log_cndh_t;
#define ADCS_LOG_CNDH_LENGTH   sizeof ( adcs_log_cndh_t )

#endif /* _adcs_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
