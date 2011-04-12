/* Andy Sayler
 * SmartWall Project
 * SmartWall Structs and Defines
 * SmartWallSockets.h
 * Created 3/11/11
 *
 * Change Log:
 * 03/11/11 - Created
  * ---
 */

#ifndef SMARTWALLSOCKETS_H
#define SMARTWALLSOCKETS_H

/* Outside Includes */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

/* Local Includes */
#include "SmartWall.h"

struct SWDeviceInfo {
    struct SmartWallDev devInfo; /* SW Device Info */
    struct sockaddr_in devIP;
};

#endif
