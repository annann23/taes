/*************************************************************************
** File: i2c_lib.h
**
** Purpose: 
**   Specification for the comm library functions.
**
*************************************************************************/
#ifndef _i2c_lib_h_
#define _i2c_lib_h_

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/************************************************************************
** Type Definitions
*************************************************************************/

/*************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief I2C Lib Function 
**  
**  \par Description
**        This is a library for I2C communication
**
**  \par Assumptions, External Events, and Notes:
**        None
**       
**  \returns
**  \retstmt Returns #CFE_SUCCESS \endcode
**  \endreturns
** 
*************************************************************************/
int32 I2C_LibInit(void);
uint8 I2CLib_WriteBytes(uint8 channel, uint8 devAddr, uint8 cmd, uint8 length, void* data);
uint8 I2CLib_ReadBytes(uint8 channel, uint8 devAddr, uint8 length, void* data);
uint8 I2CLib_WriteReadBytes(uint8 channel, uint8 devAddr, uint8 cmd, uint8 wLength, void* wData, uint8 rLength, void* rData, uint16 delay);
uint8 I2CLib_ReadOnly(uint8 channel, uint8 devAddr, uint8 cmd, uint8 rLength, void* rData, uint16 delay);

void I2CLib_Close(void);

#endif /* _i2c_lib_h_ */

/************************/
/*  End of File Comment */
/************************/
