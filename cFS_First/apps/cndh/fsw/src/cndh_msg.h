/*******************************************************************************
** File:
**  cndh_msg_app.h
**
** Purpose: 
**  Define  App  Messages and info
**
** Notes: Share with other modes(EPS,ADCS,COMS) for convenient
**			includes command & structure
**
*******************************************************************************/
#ifndef cndh_msg_h_
#define cndh_msg_h_

/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} CNDH_NoArgsCmd_t;


// command structure
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} cndh_cmd_eps_t;
#define CNDH_CMD_EPS_LENGTH   sizeof ( cndh_cmd_eps_t )

typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} cndh_cmd_coms_t;
#define CNDH_CMD_COMS_LENGTH   sizeof ( cndh_cmd_coms_t )

typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} cndh_cmd_adc_t;
#define CNDH_CMD_ADC_LENGTH   sizeof ( cndh_cmd_adc_t )

typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} cndh_cmd_gnc_t;
#define CNDH_CMD_GNC_LENGTH   sizeof ( cndh_cmd_gnc_t )


//
typedef struct
{
	uint8			tlmHeader[CFE_SB_TLM_HDR_SIZE];
	uint16			flag;

} cndh_flag_t;


// CNDH App housekeeping
typedef struct
{
    uint8              tlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              cndh_command_error_count;
    uint8              cndh_command_count;
    uint8              spare[2];

} cndh_hk_tlm_t ;
#define CNDH_APP_HK_TLM_LENGTH   sizeof ( cndh_hk_tlm_t )

typedef struct
{
	uint8    tlmHeader[CFE_SB_TLM_HDR_SIZE];
	char     Pathname[OS_MAX_PATH_LEN];
	char     LogMsg;
	uint8    CNDH_Time;
	uint8	 MTQ_Control_status;
	uint8	 Antenna_status;
	uint16	 mode_status;

}  cndh_log_t;

typedef struct
{
	uint16		Soc;
	uint16		temp;
	uint16		AX100_Status;
	uint16		IMU_Checksum;
	uint16		MMT_Checksum;
	uint16		FSS_Checksum;
	uint16		CSS_Checksum;
	int16    	AngVel[2];

	uint16		AX100_Checksum;
	uint32		Ground_time;
}   cndh_locallog_t  ;

#endif /* cndh_msg_app_h_ */

/************************/
/*  End of File Comment */
/************************/
