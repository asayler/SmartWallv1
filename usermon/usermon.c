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

extern int printDevices(FILE* outstream,
                        struct SWDeviceEntry* devices,
                        const int numDevices){
    
    /* Function Vars */
    const int errorVal = -1;
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
        printDevice(outstream, &devices[i], i);
    }
    
    return charCnt;
}

extern int printDevice(FILE* outstream,
                       struct SWDeviceEntry* device, int cnt){

    /* Local vars */
    int charCnt = 0;
    const int errorVal = -1;

    /* check user input */
    if(device == NULL){
        fprintf(stderr, "printDevice: device must not be NULL.\n");
        return errorVal;
    }

    /* Write Device Line Output */
    /* Print device count */
    charCnt += fprintf(outstream, "%3.3d| ", cnt);
    /* Print SW Address */
    charCnt += fprintf(outstream, "%5" PRIswAddr " | ", device->swAddr);
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
                       device->devTypes);
    /* Print Number of Device Channels */
    charCnt += fprintf(outstream, "%3" PRInumChan " | ",
                       device->numChan);
    /* Print SW Protocol version */
    charCnt += fprintf(outstream, "%2" PRIswVer "\n", device->version);
    
    return charCnt;

}

extern int printDevicesHex(FILE* outstream,
                           struct SWDeviceEntry* devices,
                           const int numDevices){

    /* Function Vars */
    const int errorVal = -1;
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
        printDeviceHex(outstream, &devices[i], i);
     }

    return charCnt;
}

extern int printDeviceHex(FILE* outstream,
                          struct SWDeviceEntry* device, int cnt){
    
    /* local vars */
    int charCnt = 0;
    const int errorVal = -1;

    /* check user input */
    if(device == NULL){
        fprintf(stderr, "printDeviceHex: device must nut be NULL.\n");
        return errorVal;
    }

    /* Write Device Line Output */
    /* Print device count */
    charCnt += fprintf(outstream, "%3.3d| ", cnt);
    /* Print SW Address */
    charCnt += fprintf(outstream, "0x%4.4" PRIxSWAddr " | ",
                       device->swAddr);
    /* Print IP Address */
    charCnt += fprintf(outstream, "0x%8.8" PRIxIPAddr " | ",
                       device->ipAddr);
    /* Print SW Device Type */
    charCnt += fprintf(outstream, "0x%16.16" PRIxDevType " | ",
                       device->devTypes);
    /* Print Number of Device Channels */
    charCnt += fprintf(outstream, "0x%2.2" PRIxNumChan " | ",
                       device->numChan);
    /* Print SW Protcol Version */
    charCnt += fprintf(outstream, "0x%2.2" PRIxSWVer " | ",
                       device->version);
    /* Print SW Device Unique ID */
    charCnt += fprintf(outstream, "0x%16.16" PRIxDevUID "\n",
                       device->uid);
    
    return charCnt;
}


extern int printDevicesRaw(FILE* outstream,
                           struct SWDeviceEntry* devices,
                           const int numDevices){
    
    /* Function Vars */
    const int errorVal = -1;
    int charCnt = 0;
    int i = 0;
    
    /* Check Input */
    if(devices == NULL){
        fprintf(stderr, "printDevices: devices must not be NULL.\n");
        return errorVal;
    }
    
    /* Print Devices */
    for(i = 0; i < numDevices; i++){
        printDeviceRaw(outstream, &devices[i], i);
    }
    
    return charCnt;
}

extern int printDeviceRaw(FILE* outstream,
                          struct SWDeviceEntry* device, int cnt){

    /* Local vars */
    int charCnt = 0;
    const int errorVal = -1;

    /* check user input */
    if(device == NULL){
        fprintf(stderr, "printDevice: device must not be NULL.\n");
        return errorVal;
    }

    /* Write Device Line Output */
    /* Print device count */
    charCnt += fprintf(outstream, "%3.3d ", cnt);
    /* Print SW Address */
    charCnt += fprintf(outstream, "0x%4.4" PRIxSWAddr " ",
                       device->swAddr);
    /* Print IP Address */
    charCnt += fprintf(outstream, "0x%8.8" PRIxIPAddr " ",
                       device->ipAddr);
    /* Print SW Device Type */
    charCnt += fprintf(outstream, "0x%16.16" PRIxDevType " ",
                       device->devTypes);
    /* Print Number of Device Channels */
    charCnt += fprintf(outstream, "0x%2.2" PRIxNumChan " ",
                       device->numChan);
    /* Print SW Protcol Version */
    charCnt += fprintf(outstream, "0x%2.2" PRIxSWVer " ",
                       device->version);
    /* Print SW Device Unique ID */
    charCnt += fprintf(outstream, "0x%16.16" PRIxDevUID "\n",
                       device->uid);
    
    return charCnt;

}
