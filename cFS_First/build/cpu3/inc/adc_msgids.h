/************************************************************************
** File: adc_msgids.h
**   
** Aythor:
**
** Purpose: 
**  ADC Application Message IDs header file
**
** Notes:
**
** $Log: adc_msgids.h  $
**
**
*************************************************************************/
#ifndef _adc_msgids_h_
#define _adc_msgids_h_

/**************************
** ADC Command Message IDs
***************************/

#define ADC_CMD_MID                     0x1910 /**< \brief HK Ground Commands Message ID */
#define ADC_SEND_HK_MID                 0x1911 /**< \brief HK Send Housekeeping Data Cmd Message ID */
#define ADC_WAKE_UP_MID	                0x1912
#define ADC_LOGMSG_MID                  0x1913


/***************************
** ADC Telemetry Message IDs
****************************/

#define ADC_HK_TLM_MID                  0x0910 /**< \brief HK Housekeeping Telemetry Message ID */

typedef struct
{
	uint8    TmdHeader[CFE_SB_TLM_HDR_SIZE];
	uint16   IMU;
	uint16	  IMU_Checksum;
	uint16   MMT;
	uint16   MMT_Checksum;
	uint16	  FSS_Checksum;
	uint16	  CSS_Checksum;
	uint16	  AngularVel;
	int16    Accel[2];
	int16    Temp;
	int16    AngVel[2];
	uint8	 MTQ_Control_status;
	boolean   adc_detumb;
} adc_log;

#define ADC_LOG_LNGTH   sizeof ( adc_log )

#endif /* _adc_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
