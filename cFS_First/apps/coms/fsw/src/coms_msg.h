/*******************************************************************************
** File:
**  coms_msg.h
**
** Purpose: 
**  Define  COMS Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef coms_msg_app_h_
#define coms_msg_app_h_

/*
** App command codes
*/
#define COMS_APP_NOOP_CC                 0
#define COMS_APP_RESET_COUNTERS_CC       1
#define COMS_GROUND_COMMAND              2
#define COMS_TRANSMIT_BEACON             3
#define COMS_GET_EPS_CC					 4


/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} COMS_NoArgsCmd_t;


typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} coms_cndhcmd_t;

#define COMS_CNDH_CMD_LNGTH   sizeof ( coms_cndhcmd_t )

/*************************************************************************/
/*
** Type definition ( housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              coms_command_error_count;
    uint8              coms_command_count;
    uint8              spare[2];

}   OS_PACK coms_hk_tlm_t  ;


#define COMS_APP_HK_TLM_LNGTH   sizeof ( coms_hk_tlm_t )

typedef struct
{
	uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE ];
	char     Pathname[OS_MAX_PATH_LEN];
	uint8    AX100_Checksum;
} OS_PACK coms_logtlm_t;

#define COMS_APP_LOG_LNGTH   sizeof ( coms_log )


typedef struct
{
	uint8   CmdCHeader[CFE_SB_CMD_HDR_SIZE];
	uint16 	AX100_Status;

} coms_log2;
#define COMS_LOG2_LNGTH   sizeof ( coms_log2 )



#endif /* coms_msg_app_h_ */

/************************/
/*  End of File Comment */
/************************/
