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
#include <stdio.h>
#include <arpa/inet.h>

#include "../com/swDevice.h"

/* Defines */
#define MASTER_DEVICE_FILE_BASE "../devices"
#define MASTER_DEVICE_FILE_EXTENSION DEVICE_STATUS_EXTENSION
#define MASTER_MAXDEVICES 999

/* Structs */
/* SmartWall Device Entry Struct */
struct SWDeviceEntry {
    swAddress_t swAddr;  /* SW Address */
    in_addr_t ipAddr;    /* IP Address - Host Byte Order */
    devType_t devType;   /* SW Device Type Mask */
    numChan_t numChan;   /* Number of Channels on Device */
    swVersion_t version; /* SW Protocol Version */
    devUID_t uid;        /* SW Unique Device ID */
};

/* Public Functions */
extern int buildDevFileName(char* filename);
extern int readDevice(struct SWDeviceEntry* device, FILE* devFile);
extern int writeDevice(const struct SWDeviceEntry* device, FILE* devFile);

#endif
