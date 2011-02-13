/* Andy Sayler
 * SmartWall Project
 * SmartWall Master Device Interface
 * swMaster.h
 * Created 2/5/11
 *
 * Change Log:
 * 02/05/11 - Created
 * 02/13/11 - Added SWDeviceEntry Struct
 * ---
 */


#ifndef SWMASTER_H
#define SWMASTER_H

/* Outside Includes */
#include "../com/SmartWall.h"

/* Defines */
#define MASTER_DEVICE_FILE "../devices.sws"
#define MASTER_MAXDEVICES 999

/* Structs */
/* SmartWall Device Entry Struct */
struct SWDeviceEntry {
    swAddress_t swAddr;  /* SW Address */
    in_addr_t ipAddr;    /* IP Address - Host Byte Order */
    devType_t devType;   /* SW Device Type */
    numChan_t numChan;   /* Number of Channels on Device */
    swVersion_t version; /* SW Protocol Version */
    devUID_t uid;        /* SW Unique Device ID */
};

#endif
