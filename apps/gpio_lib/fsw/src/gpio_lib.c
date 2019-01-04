/*******************************************************************************
** File:
**   $Id: gpio_lib.c $
**
** Purpose:
**    CFS library for GPIO communication
**
 +-----+-----+---------+------+---+---Pi 3+--+---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 |     |     |    3.3v |      |   |  1 || 2  |   |      | 5v      |     |     |
 |   2 |   8 |   SDA.1 | ALT0 | 1 |  3 || 4  |   |      | 5v      |     |     |
 |   3 |   9 |   SCL.1 | ALT0 | 1 |  5 || 6  |   |      | 0v      |     |     |
 |   4 |   7 | GPIO. 7 |   IN | 1 |  7 || 8  | 0 | IN   | TxD     | 15  | 14  |
 |     |     |      0v |      |   |  9 || 10 | 1 | IN   | RxD     | 16  | 15  |
 |  17 |   0 | GPIO. 0 |   IN | 0 | 11 || 12 | 0 | IN   | GPIO. 1 | 1   | 18  |
 |  27 |   2 | GPIO. 2 |   IN | 0 | 13 || 14 |   |      | 0v      |     |     |
 |  22 |   3 | GPIO. 3 |   IN | 0 | 15 || 16 | 0 | IN   | GPIO. 4 | 4   | 23  |
 |     |     |    3.3v |      |   | 17 || 18 | 0 | IN   | GPIO. 5 | 5   | 24  |
 |  10 |  12 |    MOSI | ALT0 | 0 | 19 || 20 |   |      | 0v      |     |     |
 |   9 |  13 |    MISO | ALT0 | 0 | 21 || 22 | 0 | IN   | GPIO. 6 | 6   | 25  |
 |  11 |  14 |    SCLK | ALT0 | 0 | 23 || 24 | 1 | OUT  | CE0     | 10  | 8   |
 |     |     |      0v |      |   | 25 || 26 | 1 | OUT  | CE1     | 11  | 7   |
 |   0 |  30 |   SDA.0 |   IN | 1 | 27 || 28 | 1 | IN   | SCL.0   | 31  | 1   |
 |   5 |  21 | GPIO.21 |   IN | 1 | 29 || 30 |   |      | 0v      |     |     |
 |   6 |  22 | GPIO.22 |   IN | 1 | 31 || 32 | 0 | IN   | GPIO.26 | 26  | 12  |
 |  13 |  23 | GPIO.23 |   IN | 0 | 33 || 34 |   |      | 0v      |     |     |
 |  19 |  24 | GPIO.24 |   IN | 0 | 35 || 36 | 0 | IN   | GPIO.27 | 27  | 16  |
 |  26 |  25 | GPIO.25 |   IN | 0 | 37 || 38 | 0 | IN   | GPIO.28 | 28  | 20  |
 |     |     |      0v |      |   | 39 || 40 | 0 | IN   | GPIO.29 | 29  | 21  |
 +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---+---Pi 3+--+---+------+---------+-----+-----+
*******************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "osapi.h"
#include "common_types.h"

#include "gpio_lib.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Library Initialization Routine                                  */
/* cFE requires that a library have an initialization routine      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 GPIO_LibInit(void)
{
    int32 res = OS_SUCCESS;

    if (GPIO_mutex_id == 0xffffffff)
    {
        res = OS_MutSemCreate(&GPIO_mutex_id, GPIO_MUTEX_NAME, 0);
    }

    if (res != OS_SUCCESS)
    {
        OS_printf("failed to initialize GPIO lib.\n");
        return OS_ERROR;
    }

    OS_printf ("GPIO Lib Initialized\n");
    return CFE_SUCCESS;

}/* End GPIO_LibInit */

int32 GPIOLib_Close(void)
{
    int32 res = OS_SUCCESS;

    if (GPIO_mutex_id == 0)
    {
        res = OS_BinSemDelete(GPIO_mutex_id);
    }

    if (res != OS_SUCCESS)
    {
        OS_printf("failed to close GPIO lib.\n");
        return OS_ERROR;
    }

    OS_printf ("GPIO Lib Closed\n");

    return CFE_SUCCESS;

}/* End GPIO_Lib_Close */


int16 GPIOLib_Export(uint16 pin)
{
    char buffer[GPIO_BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    OS_MutSemTake(GPIO_mutex_id);

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd == -1) {
        OS_printf("Failed to open export for writing!\n");
        OS_MutSemGive(GPIO_mutex_id);
        return(-1);
    }

    bytes_written = snprintf(buffer, GPIO_BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);

    OS_MutSemGive(GPIO_mutex_id);

    return(0);
}

int16 GPIOLib_Unexport(uint16 pin)
{
    char buffer[GPIO_BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    OS_MutSemTake(GPIO_mutex_id);

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1) {
        OS_printf("Failed to open unexport for writing!\n");
        OS_MutSemGive(GPIO_mutex_id);
        return(-1);
    }

    bytes_written = snprintf(buffer, GPIO_BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);

    OS_MutSemGive(GPIO_mutex_id);

    return(0);
}

int16 GPIOLib_Direction(uint16 pin, uint16 dir)
{
    static const char s_directions_str[]  = "in\0out";

    char path[GPIO_DIRECTION_MAX];
    int fd;

    OS_MutSemTake(GPIO_mutex_id);

    snprintf(path, GPIO_DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(path, O_WRONLY);
    if (fd == -1) {
        OS_printf("Failed to open gpio direction for writing!\n");
        OS_MutSemGive(GPIO_mutex_id);
        return(-1);
    }

    if (write(fd, &s_directions_str[GPIO_IN == dir ? 0 : 3], GPIO_IN == dir ? 2 : 3) == -1) {
        OS_printf("Failed to set direction!\n");
        OS_MutSemGive(GPIO_mutex_id);
        return(-1);
    }

    close(fd);

    OS_MutSemGive(GPIO_mutex_id);

    return(0);
}

int16 GPIOLib_Read(uint16 pin)
{
    char path[GPIO_VALUE_MAX];
    char value_str[3];
    int fd;

    OS_MutSemTake(GPIO_mutex_id);

    snprintf(path, GPIO_VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_RDONLY);
    if (fd == -1) {
        OS_printf("Failed to open gpio value for reading!\n");
        OS_MutSemGive(GPIO_mutex_id);
        return(-1);
    }

    if (read(fd, value_str, 3) == -1) {
        OS_printf("Failed to read value!\n");
        OS_MutSemGive(GPIO_mutex_id);
        return(-1);
    }

    close(fd);

    OS_MutSemGive(GPIO_mutex_id);

    return(atoi(value_str));
}

int16 GPIOLib_Write(uint16 pin, uint16 value)
{
    static const char s_values_str[] = "01";

    char path[GPIO_VALUE_MAX];
    int fd;

    OS_MutSemTake(GPIO_mutex_id);

    snprintf(path, GPIO_VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_WRONLY);
    if (fd == -1) {
        OS_printf("Failed to open gpio value for writing!\n");
        OS_MutSemGive(GPIO_mutex_id);
        return(-1);
    }

    if (write(fd, &s_values_str[GPIO_LOW == value ? 0 : 1], 1) != 1) {
        OS_printf("Failed to write value!\n");
        OS_MutSemGive(GPIO_mutex_id);
        return(-1);
    }

    close(fd);

    OS_MutSemGive(GPIO_mutex_id);

    return(0);
}
