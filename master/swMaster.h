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
#include <stdlib.h>
#include <arpa/inet.h>

/* SW Includes */
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
    devType_t devTypes;  /* SW Device Types Mask */
    numChan_t numChan;   /* Number of Channels on Device */
    swVersion_t version; /* SW Protocol Version */
    devUID_t uid;        /* SW Unique Device ID */
    int lineNum;         /* Line Number in Device File or -1 if not in file */
};
extern int compSWDeviceEntry(const struct SWDeviceEntry* d1,
                             const struct SWDeviceEntry* d2);

/* Public Functions */

/* Function to populate filename with valid SW Device File filename */
/* Return Value: Length of filename on success, -1 on failure */
extern int buildDevFileName(char* filename, const int maxLength);

/* Function to return a handle to the SW Device File and                 *
 * protect the file while open                                           */
/* Return Value: SW device File handel on success, NULL on failure       */
extern FILE* openDevFile(char* filename);

/* Function to close a handel to the SW Device File and release protections */
/* Return Value: 0 on success, -1 or EOF on failure */
extern int closeDevFile(FILE* devFile);

/* Function to read the next device form the SW Device File devFile */
/* Return Value: number of fields read on success, -1 on failure */
extern int readDevice(struct SWDeviceEntry* device, FILE* devFile,
                      int lineNum);

/* Function to write the next device to the SW Device File devFile */
/* Return Value: number of fields written on success, -1 on failure */
extern int writeDevice(const struct SWDeviceEntry* device, FILE* devFile);

/* Function to update any device in devices. Devices  with lineNum = -1
   are removed from the SW Device File devFile */
/* Return Value: number of remaining devices on success, -1 on failure */
extern FILE* updateDevices(struct SWDeviceEntry* devices,
                           int* numDevices, 
                           FILE* devFile);

/* Function to populate an array of structs describing active SW devices */
/* Return Value: Number of valid devices copied into devices on success, *
 *               -1 on failure                                           */
extern int getDevices(struct SWDeviceEntry* devices, const int maxDevices,
                      FILE* devFile);

/* Function to sort an array of structs describing active SW devices     */
/* Return Value: 0 on success, -1 on failure                             */
extern int sortDevices(struct SWDeviceEntry* devices, const int numDevices);

/* Function to find and return a pointer to the Device Entry with SW
   address swAddress from a sorted array of devices                      */
/* Return Value: pointer to SWDeviceEntry in devices, NULL on failure    */
extern struct SWDeviceEntry* findDevice(const swAddress_t swAddress,
                                        const struct SWDeviceEntry* devices,
                                        const int numDevices);

#endif
