/*******************************************************************************
** File:
**  adcs_msg.h
**
** Purpose: 
**  Define  ADCS Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef adcs_msg_h_
#define adcs_msg_h_

/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} ADCS_NoArgsCmd_t;
/*************************************************************************/
/*
** Type definition ( housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              adcs_command_error_count;
    uint8              adcs_command_count;
    uint8              spare[2];

}   OS_PACK adcs_hk_tlm_t  ;
#define ADCS_APP_HK_TLM_LENGTH   sizeof ( adcs_hk_tlm_t )

typedef struct
{
	uint8    tlmHeader[CFE_SB_TLM_HDR_SIZE];
	char     Pathname[OS_MAX_PATH_LEN];
	int16    Accel[2];
	int16    Temp;
	int16    AngVel[2];
	uint16	 IMU_Checksum;
	uint16   MMT;
	uint16   MMT_Checksum;
	uint16   AD_Status;
} adcs_logtlm_t;
#define ADCS_APP_LOG_LENGTH   sizeof ( adcs_logtlm_t )

typedef struct
{
	uint16	FSS_Status;
	uint16	CSS_Status;
	uint16	AX100_Status;
	uint16	MTQ_Status;
} adcs_log2;

#define ADCS_APP_LOG2_LENGTH   sizeof ( adcs_log2 )


typedef union
	{
	  struct
	  {
	    uint8 x_accel_h;
	    uint8 x_accel_l;
	    uint8 y_accel_h;
	    uint8 y_accel_l;
	    uint8 z_accel_h;
	    uint8 z_accel_l;
	    uint8 t_h;
	    uint8 t_l;
	    uint8 x_gyro_h;
	    uint8 x_gyro_l;
	    uint8 y_gyro_h;
	    uint8 y_gyro_l;
	    uint8 z_gyro_h;
	    uint8 z_gyro_l;
	  } reg;

	  struct
	  {
	    int16 x_accel;
	    int16 y_accel;
	    int16 z_accel;
	    int16 temperature;
	    int16 x_gyro;
	    int16 y_gyro;
	    int16 z_gyro;
	  } value;
	} accel_t_gyro_union;



#endif /* adcs_msg_app_h_ */

/************************/
/*  End of File Comment */
/************************/
