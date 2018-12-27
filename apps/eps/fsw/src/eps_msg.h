/*******************************************************************************
** File:
**  eps_msg.h
**
** Purpose: 
**  Define  EPS Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef eps_msg_h_
#define eps_msg_h_



/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} EPS_NoArgsCmd_t;
/*************************************************************************/
/*
** Type definition ( housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              eps_command_error_count;
    uint8              eps_command_count;
    uint8              spare[2];

}   OS_PACK eps_hk_tlm_t  ;
#define EPS_HK_TLM_LENGTH   sizeof ( eps_hk_tlm_t )

typedef struct
{
	uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE];
	char     Pathname[OS_MAX_PATH_LEN];
	uint16	 Soc;
	uint16	 temp;
	uint16	 FSS_Status;
	uint16	 CSS_Status;
	uint16	 AX100_Status;
	uint16	 MTQ_Status;
	uint16	 buf1;
	uint16	 buf2;
} eps_log_t;


#endif /* eps_msg_app_h_ */

/************************/
/*  End of File Comment */
/************************/
