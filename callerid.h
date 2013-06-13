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


#ifndef TRUE
#define TRUE   (1)
#endif

#ifndef FALSE
#define FALSE  (0)
#endif


typedef struct {
  // Call Type
  int call_type;
  
  // Call date and time
  int hour;
  int min;
  int day;
  int month; 

  char calling[16];
  char called[16];
  
} callerid_t;


int cid_open_serial(const char* dev);



// From BT Suppliers' Information Note 242, Appendix 1.1

enum {
  CID_PARAM_CALL_TYPE = 0x11,   // 00010001: Call Type
  CID_PARAM_DATE_TIME = 0x01,   // 00000001: Time & date
  CID_PARAM_CALLER = 0x02,      // 00000010: Calling line directory number (DN)
  CID_PARAM_CALLED = 0x03,      // 00000011: Called directory number
  CID_PARAM_REASON_DN = 0x04,   // 00000100: Reason for absence of DN
  CID_PARAM_NAME = 0x07,        // 00000111: Caller name/Text
  CID_PARAM_REASON_NAME = 0x08, // 00001000: Reason for absence of name
  CID_PARAM_NMS_STATUS = 0x13,  // 00010011: Network Message System Status
};

#endif
