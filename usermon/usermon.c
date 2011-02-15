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

/* Function to return an array of structs describing
 * active SmartWall devices */
int getDevices(struct SWDeviceEntry* devices, int maxDevices){

    /* Function Vars */
    char deviceFileName[MAX_FILENAME_LENGTH];
    const int errorVal = -1;
    FILE* deviceFile = NULL;
    int devCnt = 0;    

    /* Temp Vars */
    struct SWDeviceEntry deviceTmp;

    /* Build filenmae */
    strcpy(deviceFileName, USERMON_DEVICE_FILE_BASE);
    strcat(deviceFileName, USERMON_DEVICE_FILE_EXTENSION);

    /* TODO: Add Semephore access control to common SW state file */

    /* Open File */
    deviceFile = fopen(deviceFileName, "r");
    if(deviceFile == NULL){
        fprintf(stderr, "Could not open %s\n", deviceFileName);
        perror("deviceFile fopen");
        return errorVal;
    }
    
    /* Read File */
    while(!feof(deviceFile)){
        /* Read From File */
        if(readDevice(&deviceTmp, deviceFile) < 0){
            fprintf(stderr, "Line %d of %s has bad data.\n",
                    devCnt, deviceFileName);
            devCnt = errorVal;
            break;
        }

        /* Add device info to struct in array*/
        if(devCnt < maxDevices){
            devices[devCnt].swAddr = deviceTmp.swAddr;
            devices[devCnt].ipAddr = deviceTmp.ipAddr;
            devices[devCnt].devType = deviceTmp.devType;
            devices[devCnt].numChan = deviceTmp.numChan;
            devices[devCnt].version = deviceTmp.version;
            devices[devCnt].uid = deviceTmp.uid;
        }
        else{
            fprintf(stderr, "Max number of devices exceeded.\n");
            devCnt = errorVal;
            break;
        }

        /* Increment */
        devCnt++;
    }

    /* TODO: Add Semephore access control to common SW state file */
    fclose(deviceFile);
    
    return devCnt;
}

int printDevices(FILE* outstream){

    /* Function Vars */
    const int errorVal = -1;
    int charCnt = 0;
    struct SWDeviceEntry devices[USERMON_MAXDEVICES];
    int devCnt = 0;
    int i = 0;

    /* Get Device Array */
    devCnt = getDevices(devices, USERMON_MAXDEVICES);
    if(devCnt < 0){
        fprintf(stderr, "Error gettign device list: getDevices returned %d\n",
                devCnt);
        return errorVal;
    }
    
    /* Print Devices */
    charCnt += fprintf(outstream,
                       "    | %5s | %15s | %3s | %3s | %2s | %31s\n",
                       "SWAdr", "IP Address  ", "Typ",
                       "#Ch", "Vr", "SW Device UID         ");
    charCnt += fprintf(outstream,
                       "----------------------------------------"
                       "----------------------------------------\n");
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
        charCnt += fprintf(outstream, "%3" PRIdevType " | ",
                           devices[i].devType);
        /* Print Number of Device Channels */
        charCnt += fprintf(outstream, "%3" PRInumChan " | ",
                           devices[i].numChan);
        /* Print SW Protocol version */
        charCnt += fprintf(outstream, "%2" PRIswVer " | ", devices[i].version);
        /* Print SW Uniq ID */
        /* Convert SW Uniq ID to dotted decimal string */
        charCnt += fprintf(outstream, "%3.3" PRIdevUID ".",
                           ((devices[i].uid >> 56) & 0xff));
        charCnt += fprintf(outstream, "%3.3" PRIdevUID ".",
                           ((devices[i].uid >> 48) & 0xff));
        charCnt += fprintf(outstream, "%3.3" PRIdevUID ".",
                           ((devices[i].uid >> 40) & 0xff));
        charCnt += fprintf(outstream, "%3.3" PRIdevUID ".",
                           ((devices[i].uid >> 32) & 0xff));
        charCnt += fprintf(outstream, "%3.3" PRIdevUID ".",
                           ((devices[i].uid >> 24) & 0xff));
        charCnt += fprintf(outstream, "%3.3" PRIdevUID ".",
                           ((devices[i].uid >> 16) & 0xff));
        charCnt += fprintf(outstream, "%3.3" PRIdevUID ".",
                           ((devices[i].uid >> 8) & 0xff));
        charCnt += fprintf(outstream, "%3.3" PRIdevUID "\n",
                           ((devices[i].uid >> 0) & 0xff));
    }

    return charCnt;
}

int printDevicesHex(FILE* outstream){

    /* Function Vars */
    const int errorVal = -1;
    int charCnt = 0;
    struct SWDeviceEntry devices[USERMON_MAXDEVICES];
    int devCnt = 0;
    int i = 0;

    /* Get Device Array */
    devCnt = getDevices(devices, USERMON_MAXDEVICES);
    if(devCnt < 0){
        fprintf(stderr, "Error gettign device list: getDevices returned %d\n",
                devCnt);
        return errorVal;
    }
    
    /* Print Devices */
    charCnt += fprintf(outstream,
                       "    | %6s | %10s | %4s | %4s | %3s | %18s\n",
                       "SW Adr", "IP Address", "Type",
                       " #Ch", "Ver", "SW Device UID  ");
    charCnt += fprintf(outstream,
                       "----------------------------------------"
                       "--------------------------\n");
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
        charCnt += fprintf(outstream, "0x%2.2" PRIxDevType " | ",
                           devices[i].devType);
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
