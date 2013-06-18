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

#include "callerid.h"


#define assert_int_equals(actual,expect) \
        if (expect != actual) { fprintf(stderr, "%s:%d: expected %d but got %d.\n", __FILE__, __LINE__, expect, actual); return 1; }

#define assert_chr_equals(actual,expect) \
        if (expect != actual) { fprintf(stderr, "%s:%d: expected '%c' but got '%c'.\n", __FILE__, __LINE__, expect, actual); return 1; }

#define assert_str_equals(actual,expect) \
        if (strcmp(expect,actual)) { fprintf(stderr, "%s:%d: expected '%s' but got '%s'.\n", __FILE__, __LINE__, expect, actual); return 1; }


callerid_t* process_file(const char* filename)
{
    uint8_t buf[CID_MAX_LENGTH];
    callerid_t *cid = NULL;
    FILE* file = NULL;
    size_t bytes;

    file = fopen(filename, "rb");

    // FIXME: check for error
    bytes = fread(buf, 1, CID_MAX_LENGTH, file);

    cid = malloc(sizeof(callerid_t));
    bzero(cid, sizeof(callerid_t));

    cid_parse_buffer(cid, buf, bytes);

    fclose(file);

    return cid;
}

int test_bttext1()
{
    callerid_t *cid = process_file("tests/bttext1.bin");

    assert_int_equals(cid->call_type, CID_CALL_TYPE_VOICE);
    assert_int_equals(cid->month, 6);
    assert_int_equals(cid->day, 12);
    assert_int_equals(cid->hour, 23);
    assert_int_equals(cid->min, 12);
    assert_str_equals(cid->caller, "08456021111");
    assert_str_equals(cid->called, "");
    assert_chr_equals(cid->reason_dn, 0);
    assert_str_equals(cid->name, "");
    assert_chr_equals(cid->reason_name, 0);
    free(cid);

    // Success
    return 0;
}

int test_international1()
{
    callerid_t *cid = process_file("tests/international1.bin");

    assert_int_equals(cid->call_type, CID_CALL_TYPE_VOICE);
    assert_int_equals(cid->month, 6);
    assert_int_equals(cid->day, 12);
    assert_int_equals(cid->hour, 22);
    assert_int_equals(cid->min, 41);
    assert_str_equals(cid->caller, "");
    assert_str_equals(cid->called, "");
    assert_chr_equals(cid->reason_dn, CID_REASON_UNAVAILABLE);
    assert_str_equals(cid->name, "INTERNATIONAL");
    assert_chr_equals(cid->reason_name, 0);

    free(cid);

    // Success
    return 0;
}

int test_linetest1()
{
    callerid_t *cid = process_file("tests/linetest1.bin");

    assert_int_equals(cid->call_type, CID_CALL_TYPE_VOICE);
    assert_int_equals(cid->month, 6);
    assert_int_equals(cid->day, 12);
    assert_int_equals(cid->hour, 22);
    assert_int_equals(cid->min, 59);
    assert_str_equals(cid->caller, "08003289393");
    assert_str_equals(cid->called, "");
    assert_chr_equals(cid->reason_dn, 0);
    assert_str_equals(cid->name, "");
    assert_chr_equals(cid->reason_name, 0);

    free(cid);

    // Success
    return 0;
}

int test_tan9table1()
{
    // This test case comes from Table 1 in Exar TAN-009:
    // "Designing Caller Identification Delivery using XR-2211 for B.T."
    callerid_t *cid = process_file("tests/tan9table1.bin");

    assert_int_equals(cid->call_type, CID_CALL_TYPE_UNKNOWN);
    assert_int_equals(cid->month, 4);
    assert_int_equals(cid->day, 28);
    assert_int_equals(cid->hour, 13);
    assert_int_equals(cid->min, 20);
    assert_str_equals(cid->caller, "4084346400");
    assert_str_equals(cid->called, "");
    assert_chr_equals(cid->reason_dn, 0);
    assert_str_equals(cid->name, "");
    assert_chr_equals(cid->reason_name, 'p');   // This is wrong

    free(cid);

    // Success
    return 0;
}

int test_unavailable1()
{
    callerid_t *cid = process_file("tests/unavailable1.bin");

    assert_int_equals(cid->call_type, CID_CALL_TYPE_VOICE);
    assert_int_equals(cid->month, 6);
    assert_int_equals(cid->day, 12);
    assert_int_equals(cid->hour, 22);
    assert_int_equals(cid->min, 46);
    assert_str_equals(cid->caller, "");
    assert_str_equals(cid->called, "");
    assert_chr_equals(cid->reason_dn, CID_REASON_UNAVAILABLE);
    assert_str_equals(cid->name, "UNAVAILABLE");
    assert_chr_equals(cid->reason_name, 0);

    free(cid);

    // Success
    return 0;
}

int test_unavailable2()
{
    callerid_t *cid = process_file("tests/unavailable2.bin");

    assert_int_equals(cid->call_type, CID_CALL_TYPE_VOICE);
    assert_int_equals(cid->month, 6);
    assert_int_equals(cid->day, 12);
    assert_int_equals(cid->hour, 22);
    assert_int_equals(cid->min, 54);
    assert_str_equals(cid->caller, "");
    assert_str_equals(cid->called, "");
    assert_chr_equals(cid->reason_dn, CID_REASON_UNAVAILABLE);
    assert_str_equals(cid->name, "UNAVAILABLE");
    assert_chr_equals(cid->reason_name, 0);

    free(cid);

    // Success
    return 0;
}

int test_withheld1()
{
    callerid_t *cid = process_file("tests/withheld1.bin");

    assert_int_equals(cid->call_type, CID_CALL_TYPE_VOICE);
    assert_int_equals(cid->month, 6);
    assert_int_equals(cid->day, 12);
    assert_int_equals(cid->hour, 22);
    assert_int_equals(cid->min, 15);
    assert_str_equals(cid->caller, "");
    assert_str_equals(cid->called, "");
    assert_chr_equals(cid->reason_dn, CID_REASON_WITHHELD);
    assert_str_equals(cid->name, "WITHHELD");
    assert_chr_equals(cid->reason_name, 0);

    free(cid);

    // Success
    return 0;
}


int main(int argc, char *argv[])
{
    int failed = 0;

    if (test_bttext1()) failed++;
    if (test_international1()) failed++;
    if (test_linetest1()) failed++;
    if (test_tan9table1()) failed++;
    if (test_unavailable1()) failed++;
    if (test_unavailable2()) failed++;
    if (test_withheld1()) failed++;

    if (failed) {
        fprintf(stderr, "Failed %d tests.\n", failed);
    } else {
        printf("All Tests Passed.\n");
    }

    // Return the number of failed tests
    return failed;
}
