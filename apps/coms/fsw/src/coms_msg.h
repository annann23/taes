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

} coms_cmd_cndh_t;
#define COMS_CMD_CNDH_LENGTH   sizeof ( coms_cmd_cndh_t )

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
#define COMS_APP_HK_TLM_LENGTH   sizeof ( coms_hk_tlm_t )

typedef struct
{
	uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE ];
	char     Pathname[OS_MAX_PATH_LEN];
	uint8    AX100_Checksum;
	uint8	 AX100_Status;
} OS_PACK coms_log_t;
#define COMS_LOG_LENGTH   sizeof ( coms_log_t )

typedef struct
{
	uint8   CmdCHeader[CFE_SB_CMD_HDR_SIZE];
	uint32	time;
	uint16 	flag;
} ground_cmd_coms_t;
#define GROUND_CMD_COMS_LENGTH   sizeof ( ground_cmd_coms_t )


#endif /* coms_msg_app_h_ */

/************************/
/*  End of File Comment */
/************************/
