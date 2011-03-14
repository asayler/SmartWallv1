/* Andy Sayler
 * SmartWall Project
 * UI: swls
 * swls.c
 * Created 3/13/11
 *
 * Change Log:
 * 03/13/11 - Created
 * ---
 */

#include "usermon.h"
#include <stdlib.h>

/* Option enum */
enum OPTIONS{
    HEX, DECIMAL, RAW, HELP
};

int main(int argc, char* argv[]){

    /* local vars */
    enum OPTIONS mode;
    char filename[MAX_FILENAME_LENGTH];
    FILE* devFile = NULL;
    struct SWDeviceEntry devices[USERMON_MAXDEVICES];
    int devCnt = 0;

    /* input options */
    if(argc < 2){
        mode = HEX;
        (void) argv;
    }
    else{
        if(strncmp(argv[1], "-hex", 4) == 0){
            mode = HEX;
        }
        else if(strncmp(argv[1], "-dec", 4) == 0){
            mode = DECIMAL;
        }
        else if(strncmp(argv[1], "-raw", 4) == 0){
            mode = RAW;
        }
        else if(strcmp(argv[1], "-h") == 0){
            mode = HELP;
        }
        else{
            fprintf(stderr, "Unknown option: %5s\n Try '-h'\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    /* Switch on Mode*/
    if((mode == HEX) || (mode == DECIMAL) || (mode == RAW)){
        
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

        /* Print Device Data */
        if(mode == HEX){
            if(printDevicesHex(stdout, devices, devCnt) < 0){
                fprintf(stderr, "swls: Error listing devices in HEX mode.\n");
                exit(EXIT_FAILURE);
            }
        }
        else if(mode == DECIMAL){
            if(printDevices(stdout, devices, devCnt) < 0){
                fprintf(stderr, "swls: Error listing devices in HEX mode.\n");
                exit(EXIT_FAILURE);
            }
        }
        else if(mode == RAW){
            fprintf(stderr, "swls: RAW mode not yet implmented.\n");
            exit(EXIT_FAILURE);
        }
        else{
            fprintf(stderr, "swls: Unhandeled output mode: %d\n", mode);
            exit(EXIT_FAILURE);
        }
    }
    else if(mode == HELP){
        fprintf(stdout, "swls: List SmartWall devices\n"
                "Options:\n"
                "-hex: Output in Hex Mode\n"
                "-dec: Output in Decimal Mode\n"
                "-raw: Output in Raw Mode\n"
                "-h: Print this menu\n");
    }
    else {
        fprintf(stderr, "swls: Unhandeled mode: %d\n", mode);
        exit(EXIT_FAILURE);
    }
    
    return 0;
}
