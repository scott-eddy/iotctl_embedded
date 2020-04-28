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

#include "mqtt_publisher.h"

#include <assert.h>
#include <logging/log.h>
#include <net/mqtt.h>
#include <net/net_ip.h>
#include <net/socket.h>

LOG_MODULE_REGISTER(mqtt_publisher, LOG_LEVEL_DBG);

/// The MQTT broker network details
static struct sockaddr_in g_broker_net_details = {};

/// The MQTT client context for this publisher
static struct mqtt_client g_publisher_context;

// TODO hook into kconfig
static u8_t rx_buffer[128];
static u8_t tx_buffer[128];

void InitBrokerIpv4Details(const char* broker_ip, int broker_port) {
  assert(broker_ip != NULL);

  g_broker_net_details.sin_family = AF_INET;
  g_broker_net_details.sin_port = htons(broker_port);
  inet_pton(AF_INET, broker_ip, &(g_broker_net_details.sin_addr));
}

void MqttImplInit(const char* client_id, const char* username, const char* password) {
  mqtt_client_init(&g_publisher_context);

  /* MQTT client configuration */
  g_publisher_context.broker = &g_broker_net_details;
  g_publisher_context.client_id.utf8 = (u8_t *)client_id;
  g_publisher_context.client_id.size = strlen(client_id);

  if(password) {
    // TODO support passwords
    LOG_ERR("Don't support passwords, everything is probably broken");
  } else {
    g_publisher_context.password = NULL;
  }

  if(username) {
    // TODO support usernames
    LOG_ERR("Don't support usernames, everything is probably broken");
  } else {
    g_publisher_context.user_name = NULL;
  }

  g_publisher_context.protocol_version = MQTT_VERSION_3_1_1;

  /* MQTT buffers configuration */
  g_publisher_context.rx_buf = rx_buffer;
  g_publisher_context.rx_buf_size = sizeof(rx_buffer);
  g_publisher_context.tx_buf = tx_buffer;
  g_publisher_context.tx_buf_size = sizeof(tx_buffer);

  g_publisher_context.transport.type = MQTT_TRANSPORT_NON_SECURE;

  // TODO: this
  g_publisher_context.evt_cb = mqtt_evt_handler;
}
