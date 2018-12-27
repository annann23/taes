/*******************************************************************************
** File:
**  gnc_msg.h
**
** Purpose: 
**  Define  GNC Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef gnc_msg_app_h_
#define gnc_msg_app_h_

/*
** App command codes
*/
#define GNC_APP_NOOP_CC                 0
#define GNC_APP_RESET_COUNTERS_CC       1
#define PROCESS_CC                         2


/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} GNC_NoArgsCmd_t;
/*************************************************************************/
/*
** Type definition ( housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              gnc_command_error_count;
    uint8              gnc_command_count;
    uint8              spare[2];

}   OS_PACK gnc_hk_tlm_t  ;


#define GNC_APP_HK_TLM_LNGTH   sizeof ( gnc_hk_tlm_t )

typedef struct
{
	uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
	char     Pathname[OS_MAX_PATH_LEN];
	char     LogMsg;
	uint8    GNC_Time;

} 	OS_PACK gnc_log;

#define GNC_APP_LOG_LNGTH   sizeof ( gnc_log )


#endif /* gnc_msg_app_h_ */

/************************/
/*  End of File Comment */
/************************/
