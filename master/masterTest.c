/* Andy Sayler
 * SmartWall Project
 * SmartWall Master
 * masterTest.c
 * Created 3/15/11
 *
 * Change Log:
 * 03/15/11 - Created
 * ---
 */

#include "swMaster.h"

int main(int argc, char *argv[]){

    /* Local vars */
    char filename[MAX_FILENAME_LENGTH];    
    FILE* devFile = NULL;
    struct SWDeviceEntry newEntry;
    int devCnt = 0;
    struct SWDeviceEntry devices[MASTER_MAXDEVICES];
    struct SWDeviceEntry* findDev = NULL;

    /* Handel Input */
    (void) argc;
    (void) argv;

    /* Get Device Filename */
    if(buildDevFileName(filename, MAX_FILENAME_LENGTH) <= 0){
        fprintf(stderr, "masterTest: Could not create devFile name.\n");
        exit(EXIT_FAILURE);
    }
    
    /* Open Device File */
    devFile = openDevFile(filename);
    if(devFile == NULL){
        fprintf(stderr, "masterTest: Could not open devFile.\n");
        exit(EXIT_FAILURE);
    }

    /* Write Test record */
    newEntry.devInfo.swAddr = 0x0003u;
    newEntry.ipAddr = 0x33333333u;
    newEntry.devInfo.devTypes = SW_TYPE_UNIVERSAL | SW_TYPE_OUTLET;
    newEntry.devInfo.numChan = 0x03u;
    newEntry.devInfo.groupID = 0x01u;
    newEntry.devInfo.version = SW_VERSION;
    newEntry.devInfo.uid = 0x3333333333333333ull;
    newEntry.lineNum = -1;
    if(fseek(devFile, 0, SEEK_END) != 0){
        fprintf(stderr, "masterTest: Could not seek to end of devFile.\n");
        exit(EXIT_FAILURE);
    }
    if(writeDevice(&newEntry, devFile) < 0){
        fprintf(stderr, "masterTest: Error writing new device to devFile.\n");
        exit(EXIT_FAILURE);
    }
    
    /* Get Device Array */
    devCnt = getDevices(devices, MASTER_MAXDEVICES, devFile);
    if(devCnt < 0){
        fprintf(stderr, "masterTest: "
                "Error getting device list: getDevices returned %d\n",
                devCnt);
        exit(EXIT_FAILURE);
    }
    
    /* Sort Device Array */
    if(sortDevices(devices, devCnt) < 0){
        fprintf(stderr, "masterTest: Error sorting device list.\n");
        exit(EXIT_FAILURE);
    }

    /* Find new device in array */
    findDev = findDevice(newEntry.devInfo.swAddr, devices, devCnt);
    if(findDev == NULL){
        fprintf(stderr, "masterTest: Could not find new device.\n");
        exit(EXIT_FAILURE);
    }

    /* Remove new device from devFile */
    findDev->lineNum = -1;
    devFile = updateDevices(devices, &devCnt, devFile);
    if(devFile == NULL){
        fprintf(stderr, "masterTest: Could not update devFile.\n");
        exit(EXIT_FAILURE);
    }

    /* Close Device File */
    if(closeDevFile(devFile) != 0){
        fprintf(stderr, "masterTest: Could not close devFile.\n");
        exit(EXIT_FAILURE);
    }
    devFile = NULL;
        
    return 0;
}
