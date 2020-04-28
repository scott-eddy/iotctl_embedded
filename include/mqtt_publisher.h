/**
 * COPYRIGHTS AND PERMISSIONS:
 * Copyright 2020 Eddy Scott All rights reserved.

 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef IOTCLT_EMBEDDED_INCLUDE_MQTT_PUBLISHER_H_
#define IOTCLT_EMBEDDED_INCLUDE_MQTT_PUBLISHER_H_

/**
 * Initializes the MQTT broker network details.  This will be the network
 * information used by the publisher in order to connect to the MQTT network
 * @param broker_ip: the IPv4 address of the MQTT broker
 * @param broker_port: the port of the MQTT broker
 */
void InitBrokerIpv4Details(const char* broker_ip, int broker_port);

/**
 * Initializes the MQTT client impementation details
 * @details things like mutex initialization, version specification, etc
 * @param client_id: the MQTT client id.  See MQTT docs
 * @param username: the MQTT username.  NULL indicates no user name
 * @param password: the MQTT password.  NULL indicates no password
 */
void MqttImplInit(const char* client_id, const char* username, const char* password);

#endif //IOTCLT_EMBEDDED_INCLUDE_MQTT_PUBLISHER_H_
