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

typedef struct
{
	uint8   CmdCHeader[CFE_SB_CMD_HDR_SIZE];
	uint16 	AX100_Checksum;

} coms_log;
#define COMS_LOG_LNGTH   sizeof ( coms_log )

#endif /* _coms_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
