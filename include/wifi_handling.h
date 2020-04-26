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
#ifndef IOTCLT_EMBEDDED_SRC_WIFI_HANDLING_H_
#define IOTCLT_EMBEDDED_SRC_WIFI_HANDLING_H_

/**
 * @brief finds the wifi interface to use
 * @return 0 if success, else -ENXIO
 */
int FindWifiIface();

/**
 * @brief initialize network handlers
 */
void InitNetHandlers();

/**
 * @brief scans available networks using the wifi iface
 */
void ScanNetworks();

#endif //IOTCLT_EMBEDDED_SRC_WIFI_HANDLING_H_
