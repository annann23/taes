/*******************************************************************************
** File:
**  app_template_msg.h 
**
** Purpose: 
**  Define  App  Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef template_msg_app_h_
#define template_msg_app_h_

/*
** App command codes
*/
#define TEMPLATE_APP_NOOP_CC                 0
#define TEMPLATE_APP_RESET_COUNTERS_CC       1
#define TEMPLATE_APP_TEST_CC                 2
/* user code start */

/* user code end   */

/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct  //struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];  //struct

} Template_NoArgsCmd_t;

/*************************************************************************/
/*
** Type definition (TEMPLATE App housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              template_command_error_count;
    uint8              template_command_count;

    /* example: Simple Led on/off operation */
    uint8			   led1_state;
    uint8			   led2_state;

    /* user code start */

    /* user code end   */

    uint8              spare[2];

}   OS_PACK template_hk_tlm_t  ;


#define TEMPLATE_APP_HK_TLM_LNGTH   sizeof ( template_hk_tlm_t )


typedef struct
{
	uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
	char     Pathname[OS_MAX_PATH_LEN];

	/* example: get time data and send to DS for periodic data save */
	char     LogMsg;
	uint8    template_Time;

	/* user code start */

	/* user code end   */

} template_log;

#define TEMPLATE_APP_LOGMSG_LNGTH   sizeof ( template_LogMsg )


#endif /* template_msg_app_h_ */

/************************/
/*  End of File Comment */
/************************/
