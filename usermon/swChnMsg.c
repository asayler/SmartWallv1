/* Andy Sayler
 * SmartWall Project
 * SmartWall Master
 * swChnMsg.c
 * Created 3/12/11
 *
 * Change Log:
 * 03/12/11 - Created
 * ---
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <limits.h>

#include <fcntl.h>
#include <netdb.h>

#include "../master/swMaster.h"
#include "../com/comTools.h"

#define SENDPORT 4333 /* 4329 to 4339 Free as of 2/1/2011 */

#define MYSWADDRESS 0x0001

#define PGMNAME "swChnMsg"

/* Private Functions */

/* Option enum */
enum OPTIONS{
    HELP, ERROR, SEND
};

/* Input Format */
/* ./swChnMsg <SW Dest Address> <SW Msg Type> <SW Tgt Type> <SW Opcode> */
/*            <Chn Arg Size (bytes)> <Chn#> <Chn Arg> ...               */

/* Main Execution Entrance */
int main(int argc, char *argv[]){

    /* Local Vars */
    enum OPTIONS mode;
    unsigned long utemp = 0;
    int i = 0;
    unsigned int cnt = 0;

    /* Setup SW Message Vars */
    const msgScope_t msgScope = SW_SCP_CHANNEL;
    msgType_t msgType = 0;
    swOpcode_t opcode = 0;
    long int args[SW_MAX_CHN];
    memset(&args, 0, sizeof(args));
    struct SWChannelEntry tgtChnEntries[SW_MAX_CHN];
    memset(&tgtChnEntries, 0, sizeof(tgtChnEntries));
    struct SWChannelData tgtChnData;
    memset(&tgtChnData, 0, sizeof(tgtChnData));
    tgtChnData.data = tgtChnEntries;
    uint8_t msg[SW_MAX_MSG_LENGTH];
    memset(&msg, 0, sizeof(msg));
    swLength_t msgLen = 0;
    uint8_t body[SW_MAX_BODY_LENGTH];
    memset(&body, 0, sizeof(body));
    swLength_t bodyLen = 0;

    /* Setup My SW Vars */
    const swAddress_t mySWAddress = MYSWADDRESS;
    struct SWDeviceEntry* myDeviceEntry = NULL;
    
    /* Setup Target SW vars */
    devType_t tgtSWType = 0;
    swAddress_t tgtSWAddress = 0;
    struct SWDeviceEntry* tgtDeviceEntry = NULL;
 
    /* Setup SW File Vras */
    char devFilename[MAX_FILENAME_LENGTH] = "";
    FILE* devFile = NULL;
    struct SWDeviceEntry devices[MASTER_MAXDEVICES];
    memset(&devices, 0, sizeof(devices));
    int numDevices = 0;

    /* Setup Network Vars */
    

    /* Setup Socket Vars */
    struct sockaddr_in si_me, si_tgt;
    int s;
    int a, b, r;
    unsigned int slen = sizeof(si_tgt);

    /* Handel Input */

    if(argc < 8){
        /* Must have at least 8 args (name + 6) */
        mode = ERROR;
        (void) argv;
    }
    else if (argc % 2 != 0){
        /* Must have even number of args */
        mode = ERROR;
        (void) argv;
    }
    else{
        mode = SEND;
        /* Parse and Check Input */
        /* Target Address */
        utemp = strtoul(argv[1], NULL, 0);
        if(utemp == 0){
            fprintf(stderr, "%s: Could not convert 1st arg to long int.\n",
                    PGMNAME);
            exit(EXIT_FAILURE);
        }
        if(utemp == ULONG_MAX){
            perror(PGMNAME);
            fprintf(stderr, "%s: 1st arg out of range.\n",
                    PGMNAME);
            exit(EXIT_FAILURE);
        }
        if(utemp > SWADDRESS_MAX){
            fprintf(stderr, "%s: Target Address out of range.\n",
                    PGMNAME);
            exit(EXIT_FAILURE);
        }
        tgtSWAddress = utemp;
        /* Message Type */
        if(strToMT(argv[2], strlen(argv[2]), &msgType) < 0){
            fprintf(stderr, "%s: Invalid Message Type in 2nd arg.\n",
                    PGMNAME);
            exit(EXIT_FAILURE);
        }
        /* Target Device Type */
        if(strToDT(argv[3], strlen(argv[3]), &tgtSWType) < 0){
            fprintf(stderr, "%s: Invalid Target Device Type in 3rd arg.\n",
                    PGMNAME);
            exit(EXIT_FAILURE);
        }
        /* Opcode */
        utemp = strtoul(argv[4], NULL, 0);
        if(utemp == 0){
            fprintf(stderr, "%s: Could not convert 4th arg to long int.\n",
                    PGMNAME);
            exit(EXIT_FAILURE);
        }
        if(utemp == ULONG_MAX){
            perror(PGMNAME);
            fprintf(stderr, "%s: 4th arg out of range.\n",
                    PGMNAME);
            exit(EXIT_FAILURE);
        }
        if(utemp > SWOPCODE_MAX){
            fprintf(stderr, "%s: Opcode out of range.\n",
                    PGMNAME);
            exit(EXIT_FAILURE);
        }
        opcode = utemp;

        /* Argument Size */
        utemp = strtoul(argv[5], NULL, 0);
        if(utemp == 0){
            fprintf(stderr, "%s: Could not convert 5th arg to long int.\n",
                    PGMNAME);
            exit(EXIT_FAILURE);
        }
        if(utemp == ULONG_MAX){
            perror(PGMNAME);
            fprintf(stderr, "%s: 5th arg out of range.\n",
                    PGMNAME);
            exit(EXIT_FAILURE);
        }
        if(utemp > (SW_MAX_MSG_LENGTH - sizeof(struct SmartWallHeader) -
                    sizeof(struct SmartWallChannelHeader) -
                    (sizeof(struct SmartWallChannelTop) * ((argc - 6) / 2 )))){
            fprintf(stderr, "%s: Arg Size exceeds max possible arg bytes.\n",
                    PGMNAME);
            exit(EXIT_FAILURE);
        }
        /* Limit to long ints */
        /* TODO: Remove this limit and allow arbitrary sizes*/
        if(utemp > sizeof(long int)){
            fprintf(stderr, "%s: Arg Size currently limited to "
                    "sizeof(long int) which is %lu bytes.\n",
                    PGMNAME, sizeof(long int));
            exit(EXIT_FAILURE);
        }
        tgtChnData.header.dataLength = sizeof(long int);

        /* Loop Through Args */
        cnt = 0;
        tgtChnData.header.numChan = 0;
        for(i = 6; i < argc; i++){
            if((i % 2) == 0){
                /* Channel Number */
                if(!isnumeric(argv[i])){
                    fprintf(stderr, "%s: Arg %d is not a number.\n",
                            PGMNAME, i);
                    exit(EXIT_FAILURE);
                }
                utemp = strtoul(argv[i], NULL, 0);
                if(utemp == ULONG_MAX){
                    perror(PGMNAME);
                    fprintf(stderr, "%s: Arg %d out of range.\n",
                            PGMNAME, i);
                    exit(EXIT_FAILURE);
                }
                if(utemp > (SW_MAX_CHN - 1)){
                    fprintf(stderr, "%s: Arg %d exceeds max chn number.\n",
                            PGMNAME, i);
                    exit(EXIT_FAILURE);
                }
                if(cnt < SW_MAX_CHN){
                    tgtChnData.header.numChan++;
                    tgtChnData.data[cnt].chanTop.chanNum = utemp;
                    fprintf(stderr, "%d: utemp = %lu\n", cnt, utemp);
                    fprintf(stderr, "%d: chanNum = %" PRInumChan "\n", cnt,
                            tgtChnData.data[cnt].chanTop.chanNum);
                }
                else{
                    fprintf(stderr, "%s: Max channels exceeded. " 
                            "Ignoring Arg %d and all following args.\n",
                            PGMNAME, i);
                    break;
                }
            }
            else{
                /* Channel Argument */
                if((msgType == SW_MSG_SET) || (msgType == SW_MSG_REPORT) ||
                   (msgType == SW_MSG_ERROR)){
                    /* Has Argument */
                    if(!isnumeric(argv[i])){
                        fprintf(stderr, "%s: Arg %d is not a number.\n",
                                PGMNAME, i);
                        exit(EXIT_FAILURE);
                    }
                    args[cnt] = strtoul(argv[i], NULL, 0);
                    if(args[cnt] == LONG_MIN){
                        perror(PGMNAME);
                        fprintf(stderr,  "%s: Arg %d under range.\n",
                                PGMNAME, i);
                        exit(EXIT_FAILURE);
                    }
                    if(args[cnt] == LONG_MAX){
                        perror(PGMNAME);
                        fprintf(stderr, "%s: Arg %d over range.\n",
                                PGMNAME, i);
                        exit(EXIT_FAILURE);
                    }
                }
                else {
                    /* Null Argument */
                    args[cnt] = 0;
                }
                tgtChnData.data[cnt].chanValue = &args[cnt];
                cnt++;
            }
        }
    }
    
    /* Execute Mode */

    if((mode == ERROR) | (mode == HELP)){
        /* TODO: Remove argument long int size restriction */
        fprintf(stderr, "%s: Usage Format\n"
                "<SW Dest Address> <SW Msg Type> <SW Tgt Type> <SW Opcode> "
                "<Chn Arg Size (bytes)> <Chn#> <Chn Arg (long int)> ...\n",
                PGMNAME);
    }
    
    if(mode == SEND){
        /* Retrieve Device Data From File*/
        if(buildDevFileName(devFilename, MAX_FILENAME_LENGTH) < 0){
            fprintf(stderr, "%s: Error building device filename.\n", PGMNAME);
            exit(EXIT_FAILURE);
        }
        devFile = openDevFile(devFilename);
        if(devFile == NULL){
            fprintf(stderr, "%s: Error opening device file.\n", PGMNAME);
            exit(EXIT_FAILURE);
        }
        numDevices = getDevices(devices, MASTER_MAXDEVICES, devFile);
        if(numDevices <= 0){
            fprintf(stderr, "%s: Error getting SW device list.\n", PGMNAME);
            exit(EXIT_FAILURE);
        }
        if(closeDevFile(devFile) < 0){
            fprintf(stderr, "%s: Error closing device file.\n", PGMNAME);
            exit(EXIT_FAILURE);
        }
        devFile = NULL;
        if(sortDevices(devices, numDevices) < 0){
            fprintf(stderr, "%s: Error sorting SW device list.\n", PGMNAME);
            exit(EXIT_FAILURE);
        }
        /* Lookup My Device Info */
        myDeviceEntry = findDevice(mySWAddress, devices, numDevices);
        if(myDeviceEntry == NULL){
            fprintf(stderr, "%s: Error finding my SW device with address "
                    "0x%4.4" PRIxSWAddr ".\n", PGMNAME, mySWAddress);
            exit(EXIT_FAILURE);
        }
        /* Lookup Target Device Info */
        tgtDeviceEntry = findDevice(tgtSWAddress, devices, numDevices);
        if(tgtDeviceEntry == NULL){
            fprintf(stderr, "%s: Error finding tgt SW device with address "
                    "0x%4.4" PRIxSWAddr ".\n", PGMNAME, tgtSWAddress);
            exit(EXIT_FAILURE);
        }
        
        /* Print Test Data */
        fprintf(stderr, "Num Channels Effected: %d\n",
                tgtChnData.header.numChan);
        for(i = 0; i < tgtChnData.header.numChan; i++){
            fprintf(stderr, "Entry %d: args[%d] = %ld\n", i, i, args[i]);
            fprintf(stderr, "Entry %d: Chn %" PRInumChan " = %ld\n", i,
                    tgtChnData.data[i].chanTop.chanNum,
                    *((long int*) tgtChnData.data[i].chanValue));
        }

        /* Assemble Message Body */
        bodyLen = writeSWChannelBody(body, SW_MAX_BODY_LENGTH, &tgtChnData);
        if(bodyLen == SWLENGTH_MAX){
            fprintf(stderr, "%s: Error generating message body.\n", PGMNAME);
            exit(EXIT_FAILURE);
        }

        /* Assemble Message */
        msgLen = writeSWMsg(msg, SW_MAX_MSG_LENGTH, &(myDeviceEntry->devInfo),
                            &(tgtDeviceEntry->devInfo), tgtSWType, msgScope,
                            msgType, opcode, body, bodyLen);
        if(msgLen == SWLENGTH_MAX){
            fprintf(stderr, "%s: Error generating message.\n", PGMNAME);
            exit(EXIT_FAILURE);
        }        
                
    }

    return 0;

}
