/* Andy Sayler
 * SmartWall Project
 * User Monitor Test Main
 * testMain.c
 * Created 2/5/11
 *
 * Change Log:
 * 02/05/11 - Created
 * ---
 */

#include <stdio.h>
#include <limits.h>
#include "usermon.h"

#define MAXDEVICES MASTER_MAXDEVICES

int main(int argc, char* argv[]){

    /* Local vars */
    swAddress_t delAddress = 0;
    unsigned long temp = 0;

    /* Handel Input */
    if(argc < 2){
        fprintf(stderr, "testMain: Must enter address to remove.\n");
        exit(EXIT_FAILURE);
    }
    /* Target Address */
    temp = strtoul(argv[1], NULL, 0);
    if(temp == 0){
        fprintf(stderr, "%s: Could not convert 1st arg to long int.\n",
                "testMain");
        exit(EXIT_FAILURE);
    }
    if(temp == ULONG_MAX){
        perror("testMain");
        fprintf(stderr, "%s: 1st arg out of range.\n",
                "testMain");
        exit(EXIT_FAILURE);
    }
    if(temp > SWADDRESS_MAX){
        fprintf(stderr, "%s: Target Address out of range.\n",
                "testMain");
        exit(EXIT_FAILURE);
    }
    delAddress = temp;
    
    /* Local vars */
    char filename[MAX_FILENAME_LENGTH];
    FILE* devFile = NULL;
    struct SWDeviceEntry devices[USERMON_MAXDEVICES];
    struct SWDeviceEntry* tempDevPnt;
    int devCnt = 0;

    /* Get Device Filename */
    if(buildDevFileName(filename, MAX_FILENAME_LENGTH) <= 0){
        fprintf(stderr, "testMain: Could not create devFile name.\n");
        exit(EXIT_FAILURE);
    }
    
    /* Open Device File */
    devFile = openDevFile(filename);
    if(devFile == NULL){
        fprintf(stderr, "testMains: Could not open devFile.\n");
        exit(EXIT_FAILURE);
    }
    
    /* Get Device Array */
    devCnt = getDevices(devices, USERMON_MAXDEVICES, devFile);
    if(devCnt < 0){
        fprintf(stderr, "testMain: "
                "Error gettign device list: getDevices returned %d\n",
                devCnt);
        exit(EXIT_FAILURE);
    }
    
    /* Sort Dev Array */
    if(sortDevices(devices, devCnt) != 0){
        fprintf(stderr, "testMain: Sort Error.\n");
        exit(EXIT_FAILURE);
    }
    
    /* Find Device */
    tempDevPnt = findDevice(delAddress, devices, devCnt);
    if(tempDevPnt == NULL){
        fprintf(stderr, "testMain: No such device found.\n");
        exit(EXIT_FAILURE);
    }

    /* Remove Device */
    tempDevPnt->lineNum = -1;
    devFile = updateDevices(devices, &devCnt, devFile);
    if(devCnt < 0){
        fprintf(stderr, "testMain: Error updating devices.\n");
        exit(EXIT_FAILURE);
    }

    /* Close Device File */
    if(closeDevFile(devFile) != 0){
        fprintf(stderr, "printDevices: Could not close devFile.\n");
        exit(EXIT_FAILURE);
    }
    devFile = NULL;
    /*
    fprintf(stdout, "Unsorted:\n");
    
    fprintf(stdout, "\nSW User Mon Test: printDevicesHex returned %d\n\n",
            printDevicesHex(stdout, devices, devCnt));

    fprintf(stdout, "Sorted:\n");
    
    if(sortDevices(devices, devCnt) == 0){
        fprintf(stdout, "\nSW User Mon Test: printDevicesHex returned %d\n\n",
                printDevicesHex(stdout, devices, devCnt));
    }
    else{
        fprintf(stderr, "Sort Error.\n");
    }
    
    fprintf(stdout, "Find Address 0x0013:\n");
    tempDevPnt = findDevice(0x0013, devices, devCnt);
    if(tempDevPnt != NULL){
        printDeviceHex(stdout, tempDevPnt);
    }
    else{
        fprintf(stderr, "No such device found.\n");
    }
    */
    return 0;
}
