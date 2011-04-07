/* Andy Sayler
 * SmartWall Project
 * User Monitor Implementation
 * usermon.c
 * Created 2/5/11
 *
 * Change Log:
 * 02/05/11 - Created
 * ---
 */

#include "usermon.h"

static const int errorVal = -1;
static const int successVal = 0;

extern int printDevices(FILE* outstream,
                        struct SWDeviceEntry* devices,
                        const int numDevices){
    
    /* Function Vars */
    int charCnt = 0;
    int i = 0;
    
    /* Check Input */
    if(devices == NULL){
        fprintf(stderr, "printDevices: devices must not be NULL.\n");
        return errorVal;
    }

    /* Print Devices */
    charCnt += fprintf(outstream,
                       "   | %5s | %15s | %18s | %3s | %2s\n",
                       "SWAdr", "IP Address  ", "Types       ",
                       "#Ch", "Vr");
    charCnt += fprintf(outstream,
                       "----------------------------------------"
                       "----------------------\n");
    for(i = 0; i < numDevices; i++){
        printDevice(outstream, &devices[i]);
    }
    
    return charCnt;
}

extern int printDevice(FILE* outstream,
                       struct SWDeviceEntry* device){

    /* Local vars */
    int charCnt = 0;
    
    /* check user input */
    if(device == NULL){
        fprintf(stderr, "printDevice: device must not be NULL.\n");
        return errorVal;
    }

    /* Write Device Line Output */
    /* Print device count */
    charCnt += fprintf(outstream, "%3.3d| ", device->lineNum);
    /* Print SW Address */
    charCnt += fprintf(outstream, "%5" PRIswAddr " | ",
                       device->devInfo.swAddr);
    /* Print IPv4 Address */
    /* Convert IP Address to dotted decimal string */
    charCnt += fprintf(outstream, "%3.3" PRIipAddr ".",
                       ((device->ipAddr >> 24) & 0xff));
    charCnt += fprintf(outstream, "%3.3" PRIipAddr ".",
                       ((device->ipAddr >> 16) & 0xff));
    charCnt += fprintf(outstream, "%3.3" PRIipAddr ".",
                       ((device->ipAddr >> 8) & 0xff));
    charCnt += fprintf(outstream, "%3.3" PRIipAddr " | ",
                       ((device->ipAddr >> 0) & 0xff));
    /* Print SW Device Type */ 
    charCnt += fprintf(outstream, "0x%16.16" PRIxDevType " | ",
                       device->devInfo.devTypes);
    /* Print Number of Device Channels */
    charCnt += fprintf(outstream, "%3" PRInumChan " | ",
                       device->devInfo.numChan);
    /* Print SW Protocol version */
    charCnt += fprintf(outstream, "%2" PRIswVer "\n", device->devInfo.version);
    
    return charCnt;

}

extern int printDevicesHex(FILE* outstream,
                           struct SWDeviceEntry* devices,
                           const int numDevices){

    /* Function Vars */
    int charCnt = 0;
    int i = 0;

    /* Check Input */
    if(devices == NULL){
        fprintf(stderr, "printDevicesHex: devices must not be NULL.\n");
        return errorVal;
    }
    
    /* Print Devices */
    charCnt += fprintf(outstream,
                       "   | %6s | %10s | %18s | %4s | %4s | %18s\n",
                       "SW Adr", "IP Address", "Types       ",
                       " #Ch", "Ver", "SW Device UID  ");
    charCnt += fprintf(outstream,
                       "----------------------------------------"
                       "----------------------------------------\n");
    for(i = 0; i < numDevices; i++){
        printDeviceHex(outstream, &devices[i]);
     }

    return charCnt;
}

extern int printDeviceHex(FILE* outstream,
                          struct SWDeviceEntry* device){
    
    /* local vars */
    int charCnt = 0;
    
    /* check user input */
    if(device == NULL){
        fprintf(stderr, "printDeviceHex: device must nut be NULL.\n");
        return errorVal;
    }

    /* Write Device Line Output */
    /* Print device count */
    charCnt += fprintf(outstream, "%3.3d| ", device->lineNum);
    /* Print SW Address */
    charCnt += fprintf(outstream, "0x%4.4" PRIxSWAddr " | ",
                       device->devInfo.swAddr);
    /* Print IP Address */
    charCnt += fprintf(outstream, "0x%8.8" PRIxIPAddr " | ",
                       device->ipAddr);
    /* Print SW Device Type */
    charCnt += fprintf(outstream, "0x%16.16" PRIxDevType " | ",
                       device->devInfo.devTypes);
    /* Print Number of Device Channels */
    charCnt += fprintf(outstream, "0x%2.2" PRIxNumChan " | ",
                       device->devInfo.numChan);
    /* Print SW Protcol Version */
    charCnt += fprintf(outstream, "0x%2.2" PRIxSWVer " | ",
                       device->devInfo.version);
    /* Print SW Device Unique ID */
    charCnt += fprintf(outstream, "0x%16.16" PRIxDevUID "\n",
                       device->devInfo.uid);
    
    return charCnt;
}


extern int printDevicesRaw(FILE* outstream,
                           struct SWDeviceEntry* devices,
                           const int numDevices){
    
    /* Function Vars */
    int charCnt = 0;
    int i = 0;
    
    /* Check Input */
    if(devices == NULL){
        fprintf(stderr, "printDevices: devices must not be NULL.\n");
        return errorVal;
    }
    
    /* Print Devices */
    for(i = 0; i < numDevices; i++){
        printDeviceRaw(outstream, &devices[i]);
    }
    
    return charCnt;
}

/* Format: <Line Number> <SW Address> <IP Address> <Device Types Mask>  *
           <Number of Channels> <SW Group ID> <SW Version> <Device UID> */
extern int printDeviceRaw(FILE* outstream,
                          struct SWDeviceEntry* device){

    /* Local vars */
    int charCnt = 0;
    
    /* check user input */
    if(device == NULL){
        fprintf(stderr, "printDevice: device must not be NULL.\n");
        return errorVal;
    }

    /* Write Device Line Output */
    /* Print device count */
    charCnt += fprintf(outstream, "%3.3d ", device->lineNum);
    /* Print SW Address */
    charCnt += fprintf(outstream, "0x%4.4" PRIxSWAddr " ",
                       device->devInfo.swAddr);
    /* Print IP Address */
    charCnt += fprintf(outstream, "0x%8.8" PRIxIPAddr " ",
                       device->ipAddr);
    /* Print SW Device Type */
    charCnt += fprintf(outstream, "0x%16.16" PRIxDevType " ",
                       device->devInfo.devTypes);
    /* Print Number of Device Channels */
    charCnt += fprintf(outstream, "0x%2.2" PRIxNumChan " ",
                       device->devInfo.numChan);
    /* Print SW Group ID  */
    charCnt += fprintf(outstream, "0x%2.2" PRIxGrpID " ",
                       device->devInfo.groupID);
    /* Print SW Protcol Version */
    charCnt += fprintf(outstream, "0x%2.2" PRIxSWVer " ",
                       device->devInfo.version);
    /* Print SW Device Unique ID */
    charCnt += fprintf(outstream, "0x%16.16" PRIxDevUID "\n",
                       device->devInfo.uid);
    
    return charCnt;
}
