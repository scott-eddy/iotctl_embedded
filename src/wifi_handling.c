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

#include <logging/log.h>
#include <net/net_if.h>
#include "wifi_handling.h"

LOG_MODULE_REGISTER(wifi_handling, LOG_LEVEL_DBG);

/**
 * The wifi interface used for all wifi related comms
 */
static struct net_if *g_wifi_iface = NULL;

/// Callback link to the net management event subsystem
static struct net_mgmt_event_callback ipv4_addr_add_handler;
__attribute__((optimize("-O0")))
static void HandleIpv4AddrAdd(struct net_mgmt_event_callback *cb,
                              u32_t mgmt_event,
                              struct net_if *iface) {
  if(mgmt_event != NET_EVENT_IPV4_ADDR_ADD) {
    return;
  }

  for (int i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) {
    char buf[NET_IPV4_ADDR_LEN];

    struct net_if_addr *if_addr =
        &iface->config.ip.ipv4->unicast[i];

    if (if_addr->addr_type != NET_ADDR_DHCP || !if_addr->is_used) {
      continue;
    }

    LOG_INF("Assigned Address: %s",
            log_strdup(net_addr_ntop(AF_INET,
                                     &iface->config.ip.ipv4->unicast[i].address.in_addr,
                                     buf, sizeof(buf))));
    LOG_INF("Subnet: %s",
            log_strdup(net_addr_ntop(AF_INET,
                                     &iface->config.ip.ipv4->netmask,
                                     buf, sizeof(buf))));
    LOG_INF("Router: %s",
            log_strdup(net_addr_ntop(AF_INET,
                                     &iface->config.ip.ipv4->gw,
                                     buf, sizeof(buf))));
  }
}

/// Callback link to the net management event subsystem
static struct net_mgmt_event_callback iface_up_handler;
static void HandleIfaceUp(struct net_mgmt_event_callback *cb,
                              u32_t mgmt_event,
                              struct net_if *iface) {

  if(mgmt_event != NET_EVENT_IF_UP) {
    return;
  }
  LOG_INF("Iface up");
}

/// Callback link to the net management event subsystem
static struct net_mgmt_event_callback iface_down_handler;
static void HandleIfaceDown(struct net_mgmt_event_callback *cb,
                          u32_t mgmt_event,
                          struct net_if *iface) {

  if(mgmt_event != NET_EVENT_IF_DOWN) {
    return;
  }
  LOG_INF("Iface down");
}

int FindWifiIface() {
  // Assume we only have wifi
  g_wifi_iface = net_if_get_default();
  if(g_wifi_iface) {
    return 0;
  }
  return -ENXIO;
}

void InitNetHandlers() {
  net_mgmt_init_event_callback(&ipv4_addr_add_handler, HandleIpv4AddrAdd,
                               NET_EVENT_IPV4_ADDR_ADD);
  net_mgmt_add_event_callback(&ipv4_addr_add_handler);

  net_mgmt_init_event_callback(&iface_up_handler, HandleIfaceUp,
                               NET_EVENT_IF_UP);
  net_mgmt_add_event_callback(&iface_up_handler);

  net_mgmt_init_event_callback(&iface_down_handler, HandleIfaceDown,
                               NET_EVENT_IF_DOWN);
  net_mgmt_add_event_callback(&iface_down_handler);
}

void ScanNetworks() {

}
