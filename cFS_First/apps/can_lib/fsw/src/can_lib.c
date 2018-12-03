/*************************************************************************
** File:
**   $Id: can_lib.c $
**
** Purpose:
**    CFS library for can communication
**
*************************************************************************/


/*************************************************************************
** Includes
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "osapi.h"
#include "common_types.h"

#include "can_lib.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Library Initialization Routine                                  */
/* cFE requires that a library have an initialization routine      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int CAN_LibInit(void)
{
    int32 res = OS_SUCCESS;
    uint8 i = 0;

    for(i = 0; i < 2; i++)
    {
        if (CAN_mutex_id[i] == 0xffffffff)
        {
            res = OS_MutSemCreate(&CAN_mutex_id[i], CAN_MUTEX_NAME[i], 0);
        }

        if (res != OS_SUCCESS)
        {
            OS_printf("failed to initialize CAN lib.\n");
            return OS_ERROR;
        }
    }

    OS_printf ("CAN Lib Initialized\n");

    return CFE_SUCCESS;

}/* End CAN_LibInit */

int CANLib_Close(void)
{
    int32 res = OS_SUCCESS;
    uint8 i = 0;

    for(i = 0; i < 2; i++)
    {
        if (CAN_mutex_id[i] == 0)
        {
            res = OS_BinSemDelete(CAN_mutex_id[i]);
        }

        if (res != OS_SUCCESS)
        {
            OS_printf("failed to close CAN lib.\n");
            return OS_ERROR;
        }
    }

    OS_printf ("CAN Lib Closed\n");

    return CFE_SUCCESS;

}/* End CAN_Lib_Close */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CAN Lib function                                                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int CANLib_Send(uint8 channel, int CAN_ID, int CAN_dlc, void* Data)
{

    if ( (channel != 0 ) && (channel != 1) )
       return -1;

    OS_MutSemTake(CAN_mutex_id[channel]);

    int ret;
    int s, nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    
    memset(&frame, 0, sizeof(struct can_frame));
    
    system("sudo ip link set can0 type can bitrate 100000");
    system("sudo ifconfig can0 up");

    //1.Create socket
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) 
    {
        perror("socket PF_CAN failed");
        return 1;
    }
    
    //2.Specify can0 device
    strcpy(ifr.ifr_name, "can0");
    ret = ioctl(s, SIOCGIFINDEX, &ifr);
    if (ret < 0) 
    {
        perror("ioctl failed");
        return 1;
    }
    
    //3.Bind the socket to can0
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
    {
        perror("bind failed");
        return 1;
    }

    //4.Disable filtering rules, do not receive packets, only send
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);


    //5.Set send data
    frame.can_id = CAN_ID;
    frame.can_dlc = CAN_dlc;
    frame.data[0] = Data;


    //6.Send message
    nbytes = write(s, &frame, sizeof(frame)); 
    printf("send_data = %d \r\n", frame.data[0]);
    if(nbytes != sizeof(frame)) 
    {
        printf("Send Error frame!\r\n");
        system("sudo ifconfig can0 down");
    }
    
    //7.Close the socket and can0
    close(s);
    system("sudo ifconfig can0 down");

    OS_MutSemGive(CAN_mutex_id[channel]);

    return 1;

}

int CANLib_Receive(uint8 channel, int CAN_ID, void* Data)
{


    if ( (channel != 0 ) && (channel != 1) )
      return -1;

    OS_MutSemTake(CAN_mutex_id[channel]);

    int ret;
    int s, nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    
    memset(&frame, 0, sizeof(struct can_frame));
    
    system("sudo ip link set can0 type can bitrate 100000");
    system("sudo ifconfig can0 up");
    
    //1.Create socket
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) 
    {
        perror("socket PF_CAN failed");
        return 1;
    }
    
    //2.Specify can0 device
    strcpy(ifr.ifr_name, "can0");
    ret = ioctl(s, SIOCGIFINDEX, &ifr);
    if (ret < 0) 
    {
        perror("ioctl failed");
        return 1;
    }

    //3.Bind the socket to can0
    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) 
    {
        perror("bind failed");
        return 1;
    }

    //4.Define receive rules
    struct can_filter rfilter[1];
    rfilter[0].can_id = CAN_ID;
    rfilter[0].can_mask = CAN_SFF_MASK;
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));


    //5.Receive data and exit
    int t = 0;
    while(t < 10000) 
    {
        nbytes = read(s, &frame, sizeof(frame));
        if(nbytes > 0) 
        {
            printf("can_id = 0x%X\r\ncan_dlc = %d \r\nreceive_data = %d \r\n", frame.can_id, frame.can_dlc, frame.data[0]);         
	    Data = frame.data[0];
            
	    break;
        }
	t = t + 1;
    }
    
    //6.Close the socket and can0
    close(s);
    system("sudo ifconfig can0 down");
    
    OS_MutSemGive(CAN_mutex_id[channel]);
    return 1;

} /* End CAN_Functions */

/************************/
/*  End of File Comment */
/************************/
