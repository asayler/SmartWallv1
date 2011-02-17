/* Andy Sayler
 * SmartWall Project
 * User Monitor Interface
 * usermon.h
 * Created 2/1/11
 *
 * Change Log:
 * 02/01/11 - Created
 * 02/05/11 - Added multi-include protection #ifnedef
 * ---
 */


#ifndef USERMON_H
#define USERMON_H

/* Outside Includes */
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "../com/SmartWall.h"
#include "../master/swMaster.h"

/* Defines */
/* This is the path of the SW Device Status file. This file as the following
 * text format (all values are in hex unless otherwise noted):
 * <swAddress> <ipAddress> <swType> <swChan> <uniqID> */
#define USERMON_DEVICE_FILE_BASE MASTER_DEVICE_FILE_BASE
#define USERMON_DEVICE_FILE_EXTENSION MASTER_DEVICE_FILE_EXTENSION
#define USERMON_MAXDEVICES MASTER_MAXDEVICES

/* Function to return an array of structs describing active SmartWall devices */
extern int getDevices(struct SWDeviceEntry* devices, int maxDevices);

/* Function to print SmartWall active device list to stream in decimal*/
extern int printDevices(FILE* outstream);

/* Function to print SmartWall active device list to stream in hex*/
extern int printDevicesHex(FILE* outstream);

/* Function to print current SmartWall network info */
extern int printNetInfo(FILE* outstream);

/* Function to print current master info */
extern int printMasterInfo(FILE* outstream);

#endif
