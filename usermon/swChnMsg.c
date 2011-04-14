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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <limits.h>

#include "../com/SmartWall.h"
#include "../com/SmartWallSockets.h"
#include "../com/comTools.h"
#include "../com/comPrint.h"
#include "../master/swMaster.h"

#define SENDPORT SWOUTPORT
#define LISTENPORT SWINPORT

#define MYSWADDRESS 0x0001

#define MAXARGSIZE 16ul

/* Option enum */
enum OPTIONS{
    HELP, ERROR, SEND
};

enum PROCESSORS{
    OUTLET_CHN_PROC = 0
};
#include "../slave/swOutlet.h"

/* Input Format */
/* ./swChnMsg <SW Dest Address> <SW Msg Type> <SW Tgt Type> <SW Opcode> */
/*            <Chn Arg Size (bytes)> <Chn#> <Chn Arg> ...               */

/* Main Execution Entrance */
int main(int argc, char *argv[]){

    /* Local Vars */
    enum OPTIONS mode;
    long temp = 0;
    unsigned long uTemp = 0;
    int i = 0;
    numChan_t j = 0;
    unsigned int cnt = 0;
    char mtStrTemp[10];
    char dtStrTemp[10];
    int s;
    union outletChanArg tempArg;

    /* Setup Temporary Processor Storage Vars */
    /* Outlet Chan Storage */
    struct SWChannelEntry tmpChanEntries[SW_MAX_CHN];
    memset(&tmpChanEntries, 0, sizeof(tmpChanEntries));
    struct SWChannelData tmpChanData;
    memset(&tmpChanData, 0, sizeof(tmpChanData));
    uint8_t tmpChanArgs[SW_MAX_CHN][MAXARGSIZE];
    memset(&tmpChanArgs, 0, sizeof(tmpChanArgs));
    tmpChanData.data = tmpChanEntries;
    /* Init chanData Buffer */
    for(i = 0; i < SW_MAX_CHN; i++){
        tmpChanData.data[i].chanValue = tmpChanArgs[i];
    }
    struct SWChannelLimits limits;
    limits.maxNumChan = SW_MAX_CHN;
    limits.maxDataLength = sizeof(*tmpChanArgs);

    /* Master Device Vars */
    char devFilename[MAX_FILENAME_LENGTH];    
    FILE* devFile = NULL;
    struct SWDeviceEntry* tmpDeviceEntry;
    int numDevices = 0;
    struct SWDeviceEntry devices[MASTER_MAXDEVICES];
    
    /* Declare SmartWall Vars */
    struct SWDeviceInfo sourceDevice;
    struct SWDeviceInfo msgDevice;
    struct SWDeviceInfo myDevice;
    struct SWDeviceInfo tgtDevice;    
    devType_t targetType;
    msgScope_t msgScope;
    msgType_t msgType;
    swOpcode_t opcode;
    swOpcode_t errorOpcode;
    msgScope_t errorScope;

    /* Declare SW Buffers */
    uint8_t msg[SW_MAX_MSG_LENGTH];
    swLength_t msgLen;
    uint8_t body[SW_MAX_BODY_LENGTH];
    swLength_t bodyLen;

    /* Declare Socket vars */
    int in, out;

    /* Setup SW Vars */
    memset(&sourceDevice, 0, sizeof(sourceDevice));
    memset(&msgDevice, 0, sizeof(msgDevice));
    memset(&myDevice, 0, sizeof(myDevice));
    memset(&tgtDevice, 0, sizeof(tgtDevice));
    targetType = 0;
    msgScope = SW_SCP_CHANNEL;
    msgType = 0;
    opcode = 0;
    errorOpcode = 0;
    errorScope = 0;

    /* Setup SW Buffers */
    msgLen = 0;
    memset(&msg, 0, sizeof(msg));
    bodyLen = 0;
    memset(&body, 0, sizeof(body));

    /* Setup Socket Vars */
    in = out = 0;

    /* Populate SW Device Info */
    myDevice.devInfo.swAddr = MYSWADDRESS;

    /* Populate IP Info */
    myDevice.devIP.sin_family = AF_INET;
    myDevice.devIP.sin_port = htons(LISTENPORT);
    myDevice.devIP.sin_addr.s_addr = htonl(INADDR_ANY);
    tgtDevice.devIP.sin_family = AF_INET;
    tgtDevice.devIP.sin_port = htons(SENDPORT);
    tgtDevice.devIP.sin_addr.s_addr = htonl(INADDR_ANY);
    
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
        if(!isnumeric(argv[1])){
            fprintf(stderr, "%s: 1st arg is not a number.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        uTemp = strtoul(argv[1], NULL, 0);
        if(uTemp == ULONG_MAX){
            perror(argv[0]);
            fprintf(stderr, "%s: 1st arg out of range.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        if((swAddress_t)uTemp >= SWADDRESS_MAX){
            fprintf(stderr, "%s: Target Address out of range.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        tgtDevice.devInfo.swAddr = (swAddress_t)uTemp;
        /* Message Type */
        if(strToMT(argv[2], strlen(argv[2]), &msgType) < 0){
            fprintf(stderr, "%s: Invalid Message Type in 2nd arg.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        /* Target Device Type */
        if(strToDT(argv[3], strlen(argv[3]), &targetType) < 0){
            fprintf(stderr, "%s: Invalid Target Device Type in 3rd arg.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        /* Opcode */
        if(!isnumeric(argv[4])){
            fprintf(stderr, "%s: 4th arg is not a number.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        uTemp = strtoul(argv[4], NULL, 0);
        if(uTemp == ULONG_MAX){
            perror(argv[0]);
            fprintf(stderr, "%s: 4th arg out of range.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        if((swOpcode_t)uTemp >= SWOPCODE_MAX){
            fprintf(stderr, "%s: Opcode out of range.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        opcode = (swOpcode_t)uTemp;

        /* Argument Size */
        if(!isnumeric(argv[5])){
            fprintf(stderr, "%s: 5th arg is not a number.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        uTemp = strtoul(argv[5], NULL, 0);
        if(uTemp == ULONG_MAX){
            perror(argv[0]);
            fprintf(stderr, "%s: 5th arg out of range.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        if((swLength_t)uTemp > (SW_MAX_MSG_LENGTH -
                                sizeof(struct SmartWallHeader) -
                                sizeof(struct SmartWallChannelHeader) -
                                (sizeof(struct SmartWallChannelTop) * 
                                 ((argc - 6) / 2 )))){
            fprintf(stderr, "%s: Arg Size exceeds max possible arg bytes.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        /* Limit to Max Size */
        if(uTemp > MAXARGSIZE){
            fprintf(stderr, "%s: Arg Size currently limited to %lu.\n",
                    argv[0], MAXARGSIZE);
            exit(EXIT_FAILURE);
        }
        tmpChanData.header.dataLength = uTemp;
        
        /* Loop Through Args */
        cnt = 0;
        tmpChanData.header.numChan = 0;
        for(i = 6; i < argc; i++){
            if((i % 2) == 0){
                /* Channel Number */
                if(!isnumeric(argv[i])){
                    fprintf(stderr, "%s: Arg %d is not a number.\n",
                            argv[0], i);
                    exit(EXIT_FAILURE);
                }
                uTemp = strtoul(argv[i], NULL, 0);
                if(uTemp == ULONG_MAX){
                    perror(argv[0]);
                    fprintf(stderr, "%s: Arg %d out of range.\n",
                            argv[0], i);
                    exit(EXIT_FAILURE);
                }
                if((numChan_t)uTemp > (SW_MAX_CHN - 1)){
                    fprintf(stderr, "%s: Arg %d exceeds max chn number.\n",
                            argv[0], i);
                    exit(EXIT_FAILURE);
                }
                if(cnt < SW_MAX_CHN){
                    tmpChanData.header.numChan++;
                    tmpChanData.data[cnt].chanTop.chanNum = (numChan_t)uTemp;
                }
                else{
                    fprintf(stderr, "%s: Max channels exceeded. " 
                            "Ignoring Arg %d and all following args.\n",
                            argv[0], i);
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
                                argv[0], i);
                        exit(EXIT_FAILURE);
                    }
                    temp = strtol(argv[i], NULL, 0);
                    if(temp == LONG_MIN || temp == LONG_MAX){
                        perror(argv[0]);
                        fprintf(stderr,  "%s: Arg %d out of range.\n",
                                argv[0], i);
                        exit(EXIT_FAILURE);
                    }
                }
                else {
                    /* Null Argument */
                    temp = 0;
                }
                if(cnt < SW_MAX_CHN){
                    if(tmpChanData.header.dataLength > sizeof(long)){
                        memcpy(tmpChanData.data[cnt].chanValue, &temp,
                               sizeof(long));
                    }
                    else{
                        memcpy(tmpChanData.data[cnt].chanValue, &temp,
                               tmpChanData.header.dataLength);
                    }
                }
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
                argv[0]);
    }
    
    if(mode == SEND){
        /* Retrieve Device Data From File*/
        if(buildDevFileName(devFilename, MAX_FILENAME_LENGTH) < 0){
            fprintf(stderr, "%s: Error building device filename.\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        devFile = openDevFile(devFilename);
        if(devFile == NULL){
            fprintf(stderr, "%s: Error opening device file.\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        numDevices = getDevices(devices, MASTER_MAXDEVICES, devFile);
        if(numDevices <= 0){
            fprintf(stderr, "%s: Error getting SW device list.\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        if(closeDevFile(devFile) < 0){
            fprintf(stderr, "%s: Error closing device file.\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        devFile = NULL;
        if(sortDevices(devices, numDevices) < 0){
            fprintf(stderr, "%s: Error sorting SW device list.\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        /* Lookup My Device Info */
        tmpDeviceEntry = findDevice(myDevice.devInfo.swAddr, devices,
                                    numDevices);
        if(tmpDeviceEntry == NULL){
            fprintf(stderr, "%s: Error finding my SW device with address "
                    "0x%4.4" PRIxSWAddr ".\n", argv[0],
                    myDevice.devInfo.swAddr);
            exit(EXIT_FAILURE);
        }
        memcpy(&(myDevice.devInfo), &(tmpDeviceEntry->devInfo),
               sizeof(myDevice.devInfo));
        /* Lookup Target Device Info */
        tmpDeviceEntry = findDevice(tgtDevice.devInfo.swAddr, devices,
                                    numDevices);
        if(tmpDeviceEntry == NULL){
            fprintf(stderr, "%s: Error finding tgt SW device with address "
                    "0x%4.4" PRIxSWAddr ".\n", argv[0],
                    tgtDevice.devInfo.swAddr);
            exit(EXIT_FAILURE);
        }
        memcpy(&(tgtDevice.devInfo), &(tmpDeviceEntry->devInfo),
               sizeof(tgtDevice.devInfo));
        
        /* Check for Channel Mismatch */
        if(tmpChanData.header.numChan > tgtDevice.devInfo.numChan){
            fprintf(stderr, "%s: Target device has %" PRInumChan
                    " channel%s.\n" "You gave arguments for %" PRInumChan
                    " channel%s.\n", argv[0],
                    tgtDevice.devInfo.numChan,
                    (tgtDevice.devInfo.numChan == 1) ? "":"s",
                    tmpChanData.header.numChan,
                    (tmpChanData.header.numChan == 1) ? "":"s");
            exit(EXIT_FAILURE);
        }
        /* Setup SW Comms */
        if(swSetup(&in, &out, &myDevice)){
            fprintf(stderr, "swSetup Error\n");
            exit(EXIT_FAILURE);
        }
        /* Build Body Message */
        bodyLen = writeSWChannelBody(body, SW_MAX_BODY_LENGTH, &tmpChanData);
        if(bodyLen == SWLENGTH_MAX){
            fprintf(stderr, "writeSwChannelBody Error\n");
            exit(EXIT_FAILURE);
        }
        /* Compose Message */
        msgLen = swCompose(msg, SW_MAX_MSG_LENGTH,
                           &myDevice, &tgtDevice,
                           targetType, msgScope, msgType, opcode,
                           body, bodyLen);
        /* Send Message */
        s = swSend(out, msg, msgLen, &tgtDevice);
        if(s <= 0){
            fprintf(stderr, "swSend Error\n");
            exit(EXIT_FAILURE);
        }
        /* Listen for Response */
        msgLen = swListen(in, msg, SW_MAX_MSG_LENGTH, &sourceDevice);
        if(msgLen <= 0){
            fprintf(stderr, "swListen Error\n");
            exit(EXIT_FAILURE);
        }
        /* Receive Message */
        if(swReceive(msg, msgLen, &sourceDevice, &msgDevice,
                     &targetType, &msgScope, &msgType, &opcode,
                     body, &bodyLen, SW_MAX_BODY_LENGTH)){
            fprintf(stderr, "swReceive Error\n");
            exit(EXIT_FAILURE);
        }
        /* Check Message */
        if(swCheck(&myDevice, &msgDevice, targetType,
                   &errorScope, &errorOpcode)){
            fprintf(stderr, "swCheck Error\n");
            exit(EXIT_FAILURE);
        }
        /* Zero Data */
        if(msgScope != SW_SCP_CHANNEL){
            fprintf(stderr, "Unhandeled msgScope\n");
            exit(EXIT_FAILURE);
        }
        memset(&tmpChanEntries, 0, sizeof(tmpChanEntries));
        memset(&tmpChanData, 0, sizeof(tmpChanData));
        memset(&tmpChanArgs, 0, sizeof(tmpChanArgs));
        tmpChanData.data = tmpChanEntries;
        /* Init chanData Buffer */
        for(i = 0; i < SW_MAX_CHN; i++){
            tmpChanData.data[i].chanValue = tmpChanArgs[i];
        }
        /* Decode Message */
        readSWChannelBody(body, bodyLen, &tmpChanData, &limits);
        /* Print Message */
        
        /* Switch on Message Type: REPORT OR ERROR */
        switch(msgType){
        case SW_MSG_REPORT:
        case SW_MSG_ERROR:
            /* Report State */
            MTtoStr(mtStrTemp, sizeof(mtStrTemp),
                    &msgType); 
            DTtoStr(dtStrTemp, sizeof(dtStrTemp),
                    &targetType); 
            fprintf(stdout, "0x%4.4" PRIxSWAddr
                    " %s %s 0x%4.4" PRIxSWOpcode,
                    sourceDevice.devInfo.swAddr,
                    mtStrTemp, dtStrTemp, opcode);
            for(i = 0; i < tmpChanData.header.numChan; i++){
                j = tmpChanData.data[i].chanTop.chanNum;
                if(j < tgtDevice.devInfo.numChan){
                    if(tmpChanData.header.dataLength <= sizeof(tempArg)){
                        memcpy(&tempArg, tmpChanData.data[i].chanValue,
                               sizeof(tempArg));
                        if(opcode == OUTLET_CH_OP_POWER){
                            fprintf(stdout, " %" PRIxNumChan 
                                    " %f", j, tempArg.chanPower);
                        }
                        else if(opcode == OUTLET_CH_OP_STATE){
                            fprintf(stdout, " %" PRIxNumChan 
                                    " %ld", j, tempArg.chanState);
                        }
                        else{
                            fprintf(stdout, " %" PRIxNumChan 
                                    " %ld", j, tempArg.chanState);
                        }
                    }
                    else{
                        fprintf(stderr,
                                "\n%s: Arg Size Too Large\n",
                                argv[0]);
                    }
                }
                else{
                    fprintf(stderr,
                            "\n%s: Invalid chanNum: %u\n",
                            argv[0], j);
                }
            }
            fprintf(stdout, "\n");
            break;
        default:
            fprintf(stderr, "%s: Unhandeled msgType\n",
                    argv[0]);
            exit(EXIT_FAILURE);
            break;
        }
	/* Close Sockets */
	close(in);
	close(out);
    }
    
    return 0;
    
}
