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

int printDevices(FILE* outstream){

    /* Function Vars */
    char filename[MAX_FILENAME_LENGTH];
    FILE* devFile = NULL;
    const int errorVal = -1;
    int charCnt = 0;
    struct SWDeviceEntry devices[USERMON_MAXDEVICES];
    int devCnt = 0;
    int i = 0;

    /* Get Device Filename */
    if(buildDevFileName(filename, MAX_FILENAME_LENGTH) <= 0){
        fprintf(stderr, "printDevices: Could not create devFile name.\n");
        return errorVal;
    }

    /* Open Device File */
    devFile = openDevFile(filename, "r");
    if(devFile == NULL){
        fprintf(stderr, "printDevices: Could not open devFile.\n");
        return errorVal;
    }

    /* Get Device Array */
    devCnt = getDevices(devices, USERMON_MAXDEVICES, devFile);
    if(devCnt < 0){
        fprintf(stderr, "printDevices: "
                "Error gettign device list: getDevices returned %d\n",
                devCnt);
        return errorVal;
    }
    
    /* Close Device File */
    if(closeDevFile(devFile) != 0){
        fprintf(stderr, "printDevices: Could not close devFile.\n");
        return errorVal;
    }
    devFile = NULL;

    /* Print Devices */
    charCnt += fprintf(outstream,
                       "    | %5s | %15s | %18s | %3s | %2s\n",
                       "SWAdr", "IP Address  ", "Types       ",
                       "#Ch", "Vr");
    charCnt += fprintf(outstream,
                       "----------------------------------------"
                       "----------------------\n");
    for(i = 0; i < devCnt; i++){
        /* Write Device Line Output */
        /* Print device count */
        charCnt += fprintf(outstream, "%3.3d | ", i);
        /* Print SW Address */
        charCnt += fprintf(outstream, "%5" PRIswAddr " | ", devices[i].swAddr);
        /* Print IPv4 Address */
        /* Convert IP Address to dotted decimal string */
        charCnt += fprintf(outstream, "%3.3" PRIipAddr ".",
                           ((devices[i].ipAddr >> 24) & 0xff));
        charCnt += fprintf(outstream, "%3.3" PRIipAddr ".",
                           ((devices[i].ipAddr >> 16) & 0xff));
        charCnt += fprintf(outstream, "%3.3" PRIipAddr ".",
                           ((devices[i].ipAddr >> 8) & 0xff));
        charCnt += fprintf(outstream, "%3.3" PRIipAddr " | ",
                           ((devices[i].ipAddr >> 0) & 0xff));
        /* Print SW Device Type */ 
        charCnt += fprintf(outstream, "0x%16.16" PRIxDevType " | ",
                           devices[i].devTypes);
        /* Print Number of Device Channels */
        charCnt += fprintf(outstream, "%3" PRInumChan " | ",
                           devices[i].numChan);
        /* Print SW Protocol version */
        charCnt += fprintf(outstream, "%2" PRIswVer "\n", devices[i].version);
    }

    return charCnt;
}

int printDevicesHex(FILE* outstream){

    /* Function Vars */
    char filename[MAX_FILENAME_LENGTH];
    FILE* devFile = NULL;
    const int errorVal = -1;
    int charCnt = 0;
    struct SWDeviceEntry devices[USERMON_MAXDEVICES];
    int devCnt = 0;
    int i = 0;

    /* Get Device Filename */
    if(buildDevFileName(filename, MAX_FILENAME_LENGTH) <= 0){
        fprintf(stderr, "printDevices: Could not create devFile name.\n");
        return errorVal;
    }

    /* Open Device File */
    devFile = openDevFile(filename, "r");
    if(devFile == NULL){
        fprintf(stderr, "printDevices: Could not open devFile.\n");
        return errorVal;
    }

    /* Get Device Array */
    devCnt = getDevices(devices, USERMON_MAXDEVICES, devFile);
    if(devCnt < 0){
        fprintf(stderr, "printDevices: "
                "Error gettign device list: getDevices returned %d\n",
                devCnt);
        return errorVal;
    }
    
    /* Close Device File */
    if(closeDevFile(devFile) != 0){
        fprintf(stderr, "printDevices: Could not close devFile.\n");
        return errorVal;
    }
    devFile = NULL;
    
    /* Print Devices */
    charCnt += fprintf(outstream,
                       "    | %6s | %10s | %18s | %4s | %3s | %18s\n",
                       "SW Adr", "IP Address", "Types       ",
                       " #Ch", "Ver", "SW Device UID  ");
    charCnt += fprintf(outstream,
                       "----------------------------------------"
                       "----------------------------------------\n");
    for(i = 0; i < devCnt; i++){
        /* Write Device Line Output */
        /* Print device count */
        charCnt += fprintf(outstream, "%3.3d | ", i);
        /* Print SW Address */
        charCnt += fprintf(outstream, "0x%4.4" PRIxSWAddr " | ",
                           devices[i].swAddr);
        /* Print IP Address */
        charCnt += fprintf(outstream, "0x%8.8" PRIxIPAddr " | ",
                           devices[i].ipAddr);
        /* Print SW Device Type */
        charCnt += fprintf(outstream, "0x%16.16" PRIxDevType " | ",
                           devices[i].devTypes);
        /* Print Number of Device Channels */
        charCnt += fprintf(outstream, "0x%2.2" PRIxNumChan " | ",
                           devices[i].numChan);
        /* Print SW Protcol Version */
        charCnt += fprintf(outstream, "0x%1.1" PRIxSWVer " | ",
                           devices[i].version);
        /* Print SW Device Unique ID */
        charCnt += fprintf(outstream, "0x%16.16" PRIxDevUID "\n",
                           devices[i].uid);
    }

    return charCnt;
}
