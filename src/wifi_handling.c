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
#include <net/net_mgmt.h>
#include <net/wifi_mgmt.h>
#include "wifi_handling.h"

LOG_MODULE_REGISTER(wifi_handling, LOG_LEVEL_DBG);

/// A semaphore used to wait a thread until an IPv4 address is found
K_SEM_DEFINE(sem_ipv4_address_found, 0, 1);

/// A semaphore used to wait a thread until wifi has connected
K_SEM_DEFINE(sem_wifi_connected, 0, 1);

#define WIFI_HANDLING_EVENTS (NET_EVENT_WIFI_SCAN_RESULT |		\
				NET_EVENT_WIFI_SCAN_DONE |		    \
				NET_EVENT_WIFI_CONNECT_RESULT |		\
				NET_EVENT_WIFI_DISCONNECT_RESULT)

#define IPV4_HANDLING_EVENTS (NET_EVENT_IPV4_ADDR_ADD | NET_EVENT_IPV4_ADDR_DEL)

/**
 * The wifi interface used for all wifi related comms
 */
static struct net_if *g_wifi_iface = NULL;

/**
 * @brief Callback handler for when an IPv4 address has been assigned to a network interface
 */
static void HandleIpv4AddrAdd(struct net_if *iface) {
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

/**
 * @brief Callback handler for when an interface goes up
 */
static void HandleIfaceUp() {
  LOG_INF("Iface up");
}

/**
 * @brief Callback handler for when an interface goes down
 */
static void HandleIfaceDown() {
  LOG_INF("Iface down");
}

static void HandleWifiScanResult(struct net_mgmt_event_callback *cb)
{
  const struct wifi_scan_result *entry =
      (const struct wifi_scan_result *)cb->info;

  LOG_DBG("Scan Result: %s", entry->ssid);
  LOG_DBG("SSID: %s", entry->ssid);
  LOG_DBG("RSSI: %d", entry->rssi);
  LOG_DBG("Channel: %d", entry->channel);
  LOG_DBG("Security: %s", (entry->security == WIFI_SECURITY_TYPE_PSK ?
                           "WPA/WPA2" : "Open"));
}

static void HandleWifiScanDone(struct net_mgmt_event_callback *cb)
{
  const struct wifi_status *status =
      (const struct wifi_status *)cb->info;

  if (status->status) {
    LOG_WRN("Scan request failed (%d)\n", status->status);
  } else {
    LOG_DBG("Scan request done (%d)\n", status->status);
  }
}

static void HandleWifiConnectResult(struct net_mgmt_event_callback *cb)
{
  const struct wifi_status *status =
      (const struct wifi_status *) cb->info;

  if (status->status) {
    LOG_WRN("Connection request failed (%d)\n", status->status);
  } else {
    LOG_WRN("Connected \n", status->status);
  }

  k_sem_give(&sem_wifi_connected);
}

static void HandleWifiDisconnectResult(struct net_mgmt_event_callback *cb)
{
  const struct wifi_status *status =
      (const struct wifi_status *) cb->info;

  LOG_DBG("Disconnection request %s (%d)\n",
          status->status ? "failed" : "done",
          status->status);
}

/**
 * @brief Callback handler for wifi events
 */
static struct net_mgmt_event_callback wifi_event_handler;
static void WifiEventCallback(struct net_mgmt_event_callback *cb,
                              u32_t mgmt_event,
                              struct net_if *iface) {

  switch (mgmt_event) {
    case NET_EVENT_WIFI_SCAN_RESULT:
      HandleWifiScanResult(cb);
      break;
    case NET_EVENT_WIFI_SCAN_DONE:
      HandleWifiScanDone(cb);
      break;
    case NET_EVENT_WIFI_CONNECT_RESULT:
      HandleWifiConnectResult(cb);
      break;
    case NET_EVENT_WIFI_DISCONNECT_RESULT:
      HandleWifiDisconnectResult(cb);
      break;
    default:
      break;
  }
}

static struct net_mgmt_event_callback ipv4_event_handler;
static void Ipv4EventCallback(struct net_mgmt_event_callback *cb,
                              u32_t mgmt_event,
                              struct net_if *iface) {

  switch (mgmt_event) {
    case NET_EVENT_IPV4_ADDR_ADD:
      HandleIpv4AddrAdd(iface);
      break;
    case NET_EVENT_IPV4_ADDR_DEL:
      LOG_INF("IPV4 ADDR DEL");
      break;
    default:
      break;

  }

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
  net_mgmt_init_event_callback(&wifi_event_handler, WifiEventCallback,
                               WIFI_HANDLING_EVENTS);
  net_mgmt_add_event_callback(&wifi_event_handler);
  net_mgmt_init_event_callback(&ipv4_event_handler, Ipv4EventCallback,
                               IPV4_HANDLING_EVENTS);
  net_mgmt_add_event_callback(&ipv4_event_handler);
}

void ScanNetworks() {
  if(!g_wifi_iface) {
    return;
  }

  if (net_mgmt(NET_REQUEST_WIFI_SCAN, g_wifi_iface, NULL, 0)) {
    LOG_ERR("Scan request failed");
    return;
  } else {
    LOG_INF("Scan requested");
  }

}


int WifiConnect(struct wifi_connect_req_params* wifi_params, int timeout_ms) {
  if(wifi_params == NULL) {
    LOG_DBG("Invlaid wifi params");
    return -EBADF;
  }

  if(g_wifi_iface == NULL) {
    LOG_ERR("Failed to find wifi iface prior to call");
    return -ENXIO;
  }

  int submission_ret = net_mgmt(NET_REQUEST_WIFI_CONNECT, g_wifi_iface,
               wifi_params, sizeof(struct wifi_connect_req_params));
  if(submission_ret) {
    LOG_DBG("net_mgmt failed for ssid: %s", wifi_params->ssid);
    return submission_ret;
  }

  k_timeout_t wait_period;
  if(timeout_ms == -1) {
    wait_period = K_FOREVER;
  } else {
    wait_period = K_MSEC(timeout_ms);
  }

  int ret = k_sem_take(&sem_wifi_connected, wait_period);
  return ret;
}
