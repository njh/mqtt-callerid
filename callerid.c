/*
    Library for talking to BT Caller ID device over serial port
    Copyright (C) 2013 Nicholas J Humfrey <njh@aelius.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdarg.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>

#include "callerid.h"


/* From BT Suppliers' Information Note 227

   Transmission rate: 1200 baud ± 1%
   Data format:       Serial binary asynchronous (1 start bit first, 
                      then 8 data bits with least significant bit first,
                      followed by 1 stop bit minimum, up to 10 stop bits maximum).
                      Start bit=0, Stop bit=1.
*/
int cid_open_serial(const char* dev)
{
    struct termios options;
    int fd;

    // Open the serial port Read Only
    fd = open(dev, O_RDONLY | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        perror("Unable to open serial port");
        return fd;
    }

    // Set port to blocking mode
    fcntl(fd, F_SETFL, 0);

    // Read in current configuration options
    // FIXME: check for error
    tcgetattr(fd, &options);
    
    // Set baud rate to 1200
    // FIXME: check for error
    cfsetispeed(&options, B1200);
    cfsetospeed(&options, B1200);

    // No parity (8N1):
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // Save configuration options
    // FIXME: check for error
    tcsetattr(fd, TCSANOW, &options);
    
    return fd;
}

