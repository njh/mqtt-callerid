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


static int parse_ascii_num(const uint8_t* str)
{
    char num[3];
    num[0] = str[0];
    num[1] = str[1];
    num[2] = '\0';
    return atoi(num);
}

int cid_parse_buffer(callerid_t *cid, const uint8_t* buf, size_t len)
{
    size_t seizure_count = 0;
    int end;
    int i=0;

    // Look for the start of the channel seizure
    while (i<len) {
        if (buf[i] == 0x55) {
            break;
        } else {
            i++;
        }
    }

    // Count the number of '01010101' bytes
    while (i<len) {
        if (buf[i] == 0x55 || buf[i] == 0xD5) {
            seizure_count++;
            i++;
        } else {
            break;
        }
    }

    // The length of Channel Seizure as seen by TE should be at least 96 bits (12 bytes)
    if (seizure_count < 12) {
        printf("Failed to find enough seizure bytes\n");
        return -1;
    }

    // Check that the next byte is the type for CLIP
    if (buf[i] != 0x80) {
        printf("Message type is not 0x80 (CLIP)\n");
        return -2;
    } else {
        i++;
    }

    // FIXME: Check that the length is valid
    end = buf[i] + i;
    if (end >= len) {
        printf("Not enough bytes in buffer\n");
        return -3;
    } else {
        i++;
    }

    while (i < end) {
        int param_type = buf[i++];
        int param_len = buf[i++];

        // FIXME: check that param_len isn't too long

        switch(param_type) {
            case CID_PARAM_DATE_TIME:
              cid->month = parse_ascii_num(&buf[i]);
              cid->day = parse_ascii_num(&buf[i+2]);
              cid->hour = parse_ascii_num(&buf[i+4]);
              cid->min = parse_ascii_num(&buf[i+6]);
            break;

            case CID_PARAM_CALLER:
              memcpy(cid->caller, &buf[i], param_len);
              cid->caller[param_len] = '\0';
            break;

            case CID_PARAM_CALLED:
              memcpy(cid->called, &buf[i], param_len);
              cid->called[param_len] = '\0';
            break;

            case CID_PARAM_REASON_DN:
              cid->reason_dn = buf[i];
            break;

            case CID_PARAM_NAME:
              memcpy(cid->name, &buf[i], param_len);
              cid->called[param_len] = '\0';
            break;

            case CID_PARAM_CALL_TYPE:
              cid->call_type = buf[i];
            break;

            default:
              fprintf(stderr, "Warning: unknown param type: 0x%2.2x\n", param_type);
            break;
        }

        i += param_len;
    }

    return 0;
}
