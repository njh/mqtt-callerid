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

#ifndef MQTT_CALLERID_H_
#define MQTT_CALLERID_H_

/* Constants */
#define DEFAULT_MQTT_TOPIC      "callerid"
#define DEFAULT_MQTT_CLIENT_ID  "mqtt-callerid"
#define DEFAULT_MQTT_HOST       "localhost"
#define DEFAULT_MQTT_PORT       (1883)
#define DEFAULT_MQTT_QOS        (0)
#define DEFAULT_MQTT_RETAIN     (1)
#define DEFAULT_MQTT_KEEPALIVE  (10)

#ifndef TRUE
#define TRUE   (1)
#endif

#ifndef FALSE
#define FALSE  (0)
#endif

#endif
