/*************************************************************************
** File:
**   $Id: i2c_lib.c $
**
** Purpose: 
**    CFS library for i2c communication 
**
*************************************************************************/


/*************************************************************************
** Includes
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <errno.h>

#include <linux/i2c-dev.h>
#include <linux/i2c.h>



#include "osapi.h"
#include "common_types.h"

#include "i2c_lib.h"

/*************************************************************************
** Macro Definitions
*************************************************************************/


const char* I2C_DEVICE[2] = {"/dev/i2c-0", "/dev/i2c-1"};
const char* I2C_MUTEX_NAME[2] = {"I2C_Mutex0", "I2C_Mutex1"};




/*************************************************************************
** global variables 
*************************************************************************/
uint32 I2C_mutex_id[2] = {0xffffffff, 0xffffffff};





/*************************************************************************
** Private Function Prototypes
*************************************************************************/


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Library Initialization Routine                                  */
/* cFE requires that a library have an initialization routine      */ 
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 I2C_LibInit(void)
{
	int32 res = OS_SUCCESS;
	uint8 i = 0;	
	

	for(i =0; i < 2; i++)
	{
 		if (I2C_mutex_id[i] == 0xffffffff)
		{
			res = OS_MutSemCreate(&I2C_mutex_id[i], I2C_MUTEX_NAME[i], 0);
		}

	    	if (res != OS_SUCCESS)
		{
			OS_printf("failed to initialize I2C lib.\n");
			return -1;
		}
	}


    OS_printf ("I2C Lib Initialized\n");
    return CFE_SUCCESS;
 
}/* End I2C_LibInit */

void I2CLib_Close(void)
{

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* I2C Lib function                                             */ 
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uint8 I2CLib_WriteBytes(uint8 channel, uint8 devAddr, uint8 cmd, uint8 length, void* data)
{
	int8 count = 0;
	uint8 buf[32];
	int32 fd;
	
	if ( (channel != 0 ) && (channel != 1) )
		return -1;

	OS_MutSemTake(I2C_mutex_id[channel]);

	fd = open(I2C_DEVICE[channel], O_RDWR);
	if(fd < 0)
	{
		OS_printf("fail to open device : %s, devAddr : 0x%x\n", strerror(errno), devAddr);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return 0;
	}
	
	if (ioctl(fd, I2C_SLAVE, devAddr) < 0)
	{
		OS_printf("fail to select device : %s\n", strerror(errno));
		close(fd);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return 0;
	}
	
	buf[0] = cmd;
	memcpy(buf+1, data, length);
	count = write(fd, buf, length+1);
	if (count < 0)
	{
		OS_printf("fail to write device : %s,  devAddr : 0x%x\n", strerror(errno), devAddr);
		close(fd);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return 0;
	}
	else if (count != length +1 )
	{
		OS_printf("short write to device, expected : %d, got : %d\n", length+1, count);
		close(fd);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return 0;
	}

	close(fd);
	OS_MutSemGive(I2C_mutex_id[channel]);

	return 1;

}

uint8 I2CLib_ReadBytes(uint8 channel, uint8 devAddr, uint8 length, void* data)
{

	int8 count = 0;

	if ( (channel != 0 ) && (channel != 1) )
		return -1;

	OS_MutSemTake(I2C_mutex_id[channel]);
	

	int32 fd = open(I2C_DEVICE[channel], O_RDWR);
	if (fd < 0)
	{
		OS_printf("fail to open device : %s, devAddr : %d\n", strerror(errno), devAddr);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return -1;
	}


	if (ioctl(fd, I2C_SLAVE, devAddr) < 0)
	{
		OS_printf("fail to select device : %s\n", strerror(errno));
		close(fd);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return -1;
	}
	
	count = read(fd, data, length);


	if(count < 0)
	{
		OS_printf("fail to read device(%d) : %s\n", count, strerror(errno));
		close(fd);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return -1;
	}
	else if (count != length)
	{
		OS_printf("short read from device, expected : %d, got : %d\n", length, count);
		close(fd);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return -1;
	}

	close(fd);
	OS_MutSemGive(I2C_mutex_id[channel]);
	return count;

}

uint8 I2CLib_WriteReadBytes(uint8 channel, uint8 devAddr, uint8 cmd, uint8 wLength, void* wData, uint8 rLength, void* rData, uint16 delay)
{
	int8 count = 0;
	uint8 buf[32];

	if ( (channel != 0 ) && (channel != 1) )
		return -1;

	OS_MutSemTake(I2C_mutex_id[channel]);
	


	int32 fd = open(I2C_DEVICE[channel], O_RDWR);
	if (fd < 0)
	{
		OS_printf("fail to open device : %s, devAddr : 0x%x\n", strerror(errno), devAddr);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return -1;
	}


	if (ioctl(fd, I2C_SLAVE, devAddr) < 0)
	{
		OS_printf("fail to select device : %s, devAddr : 0x%x\n", strerror(errno), devAddr);
		close(fd);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return -1;
	}

	buf[0] = cmd;
	memcpy(buf+1, wData, wLength);
	count = write(fd, buf, wLength+1);
	if (count < 0)
	{
		OS_printf("fail to write device : %s, devAddr : 0x%x\n", strerror(errno), devAddr);
		close(fd);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return 0;
	}
	else if (count != wLength +1 )
	{
		OS_printf("short write to device, expected : %d, got : %d\n", wLength+1, count);
		close(fd);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return 0;
	}

	OS_MutSemGive(I2C_mutex_id[channel]);

	OS_TaskDelay(delay);

	OS_MutSemTake(I2C_mutex_id[channel]);

	count = read(fd, rData, rLength);


	if(count < 0)
	{
		OS_printf("fail to read device(%d) : %s\n", count, strerror(errno));
		close(fd);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return -1;
	}
/*	else if (count != rLength)
	{
		OS_printf("short read from device, expected : %d, got : %d\n", rLength, count);
		close(fd);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return -1;
	}*/

	close(fd);
	OS_MutSemGive(I2C_mutex_id[channel]);


	return count;




}

uint8 I2CLib_ReadOnly(uint8 channel, uint8 devAddr, uint8 cmd, uint8 rLength, void* rData, uint16 delay)
{
	int8 count = 0;
	uint8 buf[32];

	if ( (channel != 0 ) && (channel != 1) )
		return -1;

	OS_MutSemTake(I2C_mutex_id[channel]);



	int32 fd = open(I2C_DEVICE[channel], O_RDWR);
	if (fd < 0)
	{
		OS_printf("fail to open device : %s, devAddr : 0x%x\n", strerror(errno), devAddr);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return -1;
	}


	if (ioctl(fd, I2C_SLAVE, devAddr) < 0)
	{
		OS_printf("fail to select device : %s, devAddr : 0x%x\n", strerror(errno), devAddr);
		close(fd);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return -1;
	}

	buf[0] = cmd;
	count = write(fd, buf, 1);
	if (count < 0)
	{
		OS_printf("fail to write device : %s, devAddr : 0x%x\n", strerror(errno), devAddr);
		close(fd);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return 0;
	}
	else if (count != 1 )
	{
		OS_printf("short write to device, expected : %d, got : %d\n", 1, count);
		close(fd);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return 0;
	}

	OS_MutSemGive(I2C_mutex_id[channel]);

	OS_TaskDelay(delay);

	OS_MutSemTake(I2C_mutex_id[channel]);

	count = read(fd, rData, rLength);


	if(count < 0)
	{
		OS_printf("fail to read device(%d) : %s\n", count, strerror(errno));
		close(fd);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return -1;
	}
/*	else if (count != rLength)
	{
		OS_printf("short read from device, expected : %d, got : %d\n", rLength, count);
		close(fd);
		OS_MutSemGive(I2C_mutex_id[channel]);
		return -1;
	}*/

	close(fd);
	OS_MutSemGive(I2C_mutex_id[channel]);


	return count;




}


/* End I2C_Functions */

/************************/
/*  End of File Comment */
/************************/
