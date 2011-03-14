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
#include "usermon.h"

#define MAXDEVICES MASTER_MAXDEVICES

int main(int argc, char* argv[]){

    (void) argc;
    (void) argv;

    /* Local vars */
    char filename[MAX_FILENAME_LENGTH];
    FILE* devFile = NULL;
    struct SWDeviceEntry devices[USERMON_MAXDEVICES];
    struct SWDeviceEntry tempDev;
    int devCnt = 0;

    /* Get Device Filename */
    if(buildDevFileName(filename, MAX_FILENAME_LENGTH) <= 0){
        fprintf(stderr, "printDevices: Could not create devFile name.\n");
        exit(EXIT_FAILURE);
    }
    
    /* Open Device File */
    devFile = openDevFile(filename, "r");
    if(devFile == NULL){
        fprintf(stderr, "printDevices: Could not open devFile.\n");
        exit(EXIT_FAILURE);
    }
    
    /* Get Device Array */
    devCnt = getDevices(devices, USERMON_MAXDEVICES, devFile);
    if(devCnt < 0){
        fprintf(stderr, "printDevices: "
                "Error gettign device list: getDevices returned %d\n",
                devCnt);
        exit(EXIT_FAILURE);
    }
    
    /* Close Device File */
    if(closeDevFile(devFile) != 0){
        fprintf(stderr, "printDevices: Could not close devFile.\n");
        exit(EXIT_FAILURE);
    }
    devFile = NULL;

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
    if(findDevice(0x0013, &tempDev, devices, devCnt) == 0){
        printDeviceHex(stdout, &tempDev, 1);
    }
    else{
        fprintf(stderr, "No such device found.\n");
    }

    return 0;
}
