/*
    mqtt-callerid - BT Caller ID over serial port to MQTT bridge
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
#include <mosquitto.h>

#include "mqtt-callerid.h"



/* Global Variables */
struct mosquitto *mosq = NULL;
int keep_running = TRUE;
int mqtt_connected = FALSE;
int debug = TRUE;
int exit_code = EXIT_SUCCESS;

// Configurable parameters
const char* serial_device = "/dev/tty.usbserial-AM01Z7LE";
const char* mqtt_topic = DEFAULT_MQTT_TOPIC;
const char* mqtt_host = DEFAULT_MQTT_HOST;
const char* mqtt_client_id = DEFAULT_MQTT_CLIENT_ID;
int mqtt_port = DEFAULT_MQTT_PORT;
int mqtt_qos = DEFAULT_MQTT_QOS;
int mqtt_retain = DEFAULT_MQTT_RETAIN;
int mqtt_keepalive = DEFAULT_MQTT_KEEPALIVE;


static void termination_handler(int signum)
{
    switch(signum) {
        case SIGHUP:  printf("Received SIGHUP, exiting.\n"); break;
        case SIGTERM: printf("Received SIGTERM, exiting.\n"); break;
        case SIGINT:  printf("Received SIGINT, exiting.\n"); break;
    }

    keep_running = FALSE;
}

static void cid_connect_callback(struct mosquitto *mosq, void *obj, int rc)
{
  if(!rc){
    printf("Connected to MQTT server.\n");
    mqtt_connected = TRUE;
  } else {
    const char *str = mosquitto_connack_string(rc);
    printf("Connection Refused: %s\n", str);
    mqtt_connected = FALSE;
  }
}


static void cid_disconnect_callback(struct mosquitto *mosq, void *obj, int rc)
{
    mqtt_connected = FALSE;

    // FIXME: re-establish the connection
    // FIXME: keep count of re-connects
}

static void cid_log_callback(struct mosquitto *mosq, void *obj, int level, const char *str)
{
    // FIXME: use the log level
    printf("LOG: %s\n", str);
}

static struct mosquitto * cid_initialise_mqtt(const char* id)
{
    struct mosquitto *mosq = NULL;
    int res = 0;

    mosq = mosquitto_new(id, TRUE, NULL);
    if (!mosq) {
        printf("Failed to initialise MQTT client.\n");
        return NULL;
    }

    // FIXME: add support for username and password
    mosquitto_log_callback_set(mosq, cid_log_callback);
    mosquitto_connect_callback_set(mosq, cid_connect_callback);
    mosquitto_disconnect_callback_set(mosq, cid_disconnect_callback);

    printf("Connecting to %s:%d...\n", mqtt_host, mqtt_port);
    res = mosquitto_connect(mosq, mqtt_host, mqtt_port, mqtt_keepalive);
    if (res) {
        printf("Unable to connect (%d).\n", res);
        mosquitto_destroy(mosq);
        return NULL;
    }

    return mosq;
}


/*
static void usage()
{
    printf("%s version %s\n\n", PACKAGE_NAME, PACKAGE_VERSION);
    printf("Usage: %s [options]\n", PACKAGE_NAME);
    printf("   -h            Display this message\n");
    printf("   -D            Enable debug mode\n");
    exit(-1);
}
*/




int main(int argc, char *argv[])
{
    //int opt;
    struct termios options;
    int res;
    int fd;

    // Make stdout unbuffered for logging/debugging
    setbuf(stdout, NULL);

    // FIXME: Parse Switches
    /*
    while ((opt = getopt(argc, argv, "Dh")) != -1) {
        switch (opt) {
            case 'D':  debug = TRUE; break;
            case 'h':
            default:   usage(); break;
        }
    }
    */

    // Open the serial port
    fd = open(serial_device, O_RDONLY | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        perror("Unable to open serial port");
        exit(-1);
    }

    fcntl(fd, F_SETFL, 0);

    tcgetattr(fd, &options);
    cfsetispeed(&options, B1200);
    cfsetospeed(&options, B1200);

    // No parity (8N1):
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    tcsetattr(fd, TCSANOW, &options);

    // Initialise libmosquitto
    mosquitto_lib_init();
    if (debug) {
        int major, minor, revision;
        mosquitto_lib_version(&major, &minor, &revision);
        printf("libmosquitto version: %d.%d.%d\n", major, minor, revision);
    }

    // Create MQTT client
    mosq = cid_initialise_mqtt(mqtt_client_id);
    if (!mosq) {
        printf("Failed to initialise MQTT client\n");
        goto cleanup;
    }

    // Setup signal handlers - so we exit cleanly
    signal(SIGTERM, termination_handler);
    signal(SIGINT, termination_handler);
    signal(SIGHUP, termination_handler);


    int i = 0;
    unsigned char buf[128];
    while (keep_running) {
        size_t bytes = read(fd, buf, 1);
        if (bytes) {
            unsigned int hex = buf[0];
            printf("Read[%d]: '%c' (0x%2.2x)\n", i, buf[0], hex);
        }

        i++;
        // FIXME: convert this to a select loop?

        // Wait for network packets for a maximum of 50ms
        //res = mosquitto_loop(mosq, 50, 1);
        // FIXME: check for errors
    }

cleanup:
    printf("Cleaning up.\n");

    // Disconnect from MQTT server
    if (mosq) mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();


    // Close the serial port
    close(fd);

    // exit_code is non-zero if something went wrong
    return exit_code;
}
