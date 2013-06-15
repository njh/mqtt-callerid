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

#ifndef CALLERID_H_
#define CALLERID_H_

#include <stdint.h>


#ifndef TRUE
#define TRUE   (1)
#endif

#ifndef FALSE
#define FALSE  (0)
#endif

// Channel Seizure may be up to 315 bits (262ms): 40 bytes.
// The message length may be up to 64 bytes for CLIP
// Message type, length, an checksum: 3 bytes
#define CID_MAX_LENGTH   (128)


typedef struct {
  // Call Type
  int call_type;

  // Call date and time
  int month;
  int day;
  int hour;
  int min;

  char caller[19];  // The maximum length of number sent is 18 characters.
  char called[19];  // The maximum length of number sent is 18 characters.

  char reason_dn;

  char name[21];    // The Name/Text shall consist of between 1 and 20 BT IA5 characters.

} callerid_t;


int cid_open_serial(const char* dev);
int cid_parse_buffer(callerid_t* cid, const uint8_t* str, size_t len);


// From BT Suppliers' Information Note 227, Annex A

enum {
  CID_PARAM_DATE_TIME = 0x01,   // 00000001: Time & date
  CID_PARAM_CALLER = 0x02,      // 00000010: Calling line directory number (DN)
  CID_PARAM_CALLED = 0x03,      // 00000011: Called directory number
  CID_PARAM_REASON_DN = 0x04,   // 00000100: Reason for absence of DN
  CID_PARAM_NAME = 0x07,        // 00000111: Caller name/Text
  CID_PARAM_REASON_NAME = 0x08, // 00001000: Reason for absence of name
  CID_PARAM_CALL_TYPE = 0x11,   // 00010001: Call Type
  CID_PARAM_NMS_STATUS = 0x13,  // 00010011: Network Message System Status
};

enum {
  CID_REASON_WITHHELD = 'P',
  CID_REASON_UNAVAILABLE = 'O',
};

enum {
  CID_CALL_TYPE_VOICE = 0x01,
  CID_CALL_TYPE_RINGBACK = 0x02,
  CID_CALL_TYPE_MESSAGE_WAITING = 0x81,
};

enum {
  CID_STATE_UNKNOWN,
  CID_STATE_SEIZURE,
};


#endif
