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

#include "../com/SmartWall.h"
#include "../com/comTools.h"
#include "../master/swMaster.h"

#define SENDPORT SWOUTPORT
#define LISTENPORT SWINPORT

#define MYSWADDRESS 0x0001

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
    numChan_t j;
    unsigned int cnt = 0;
    long* temp = NULL;
    char mtStrTemp[10];
    char dtStrTemp[10];

    /* Setup SW Message Vars */
    devType_t targetType = 0;
    msgScope_t msgScope = SW_SCP_CHANNEL;
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
    struct SmartWallDev sourceDeviceInfo;
    memset(&sourceDeviceInfo, 0, sizeof(sourceDeviceInfo));
    struct SmartWallDev destDeviceInfo;
    memset(&destDeviceInfo, 0, sizeof(destDeviceInfo));
    struct SmartWallDev myDeviceInfo;
    memset(&myDeviceInfo, 0, sizeof(myDeviceInfo));
    struct SmartWallDev tgtDeviceInfo;
    memset(&tgtDeviceInfo, 0, sizeof(tgtDeviceInfo));
    struct SWChannelLimits limits;
    limits.maxNumChan = SW_MAX_CHN;
    limits.maxDataLength = sizeof(*args);

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

    /* Setup Socket Vars */
    struct sockaddr_in si_me, si_tgt;
    memset(&si_me, 0, sizeof(si_me));
    memset(&si_tgt, 0, sizeof(si_tgt));
    socklen_t slen = sizeof(struct sockaddr_in);
    int in, out;
    int b, r;

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
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        if(utemp == ULONG_MAX){
            perror(argv[0]);
            fprintf(stderr, "%s: 1st arg out of range.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        if(utemp > SWADDRESS_MAX){
            fprintf(stderr, "%s: Target Address out of range.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        tgtSWAddress = utemp;
        /* Message Type */
        if(strToMT(argv[2], strlen(argv[2]), &msgType) < 0){
            fprintf(stderr, "%s: Invalid Message Type in 2nd arg.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        /* Target Device Type */
        if(strToDT(argv[3], strlen(argv[3]), &tgtSWType) < 0){
            fprintf(stderr, "%s: Invalid Target Device Type in 3rd arg.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        /* Opcode */
        utemp = strtoul(argv[4], NULL, 0);
        if(utemp == 0){
            fprintf(stderr, "%s: Could not convert 4th arg to long int.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        if(utemp == ULONG_MAX){
            perror(argv[0]);
            fprintf(stderr, "%s: 4th arg out of range.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        if(utemp > SWOPCODE_MAX){
            fprintf(stderr, "%s: Opcode out of range.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        opcode = utemp;

        /* Argument Size */
        utemp = strtoul(argv[5], NULL, 0);
        if(utemp == 0){
            fprintf(stderr, "%s: Could not convert 5th arg to long int.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        if(utemp == ULONG_MAX){
            perror(argv[0]);
            fprintf(stderr, "%s: 5th arg out of range.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        if(utemp > (SW_MAX_MSG_LENGTH - sizeof(struct SmartWallHeader) -
                    sizeof(struct SmartWallChannelHeader) -
                    (sizeof(struct SmartWallChannelTop) * ((argc - 6) / 2 )))){
            fprintf(stderr, "%s: Arg Size exceeds max possible arg bytes.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        /* Limit to long ints */
        /* TODO: Remove this limit and allow arbitrary sizes*/
        if(utemp > sizeof(long int)){
            fprintf(stderr, "%s: Arg Size currently limited to "
                    "sizeof(long int) which is %lu bytes.\n",
                    argv[0], sizeof(long int));
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
                            argv[0], i);
                    exit(EXIT_FAILURE);
                }
                utemp = strtoul(argv[i], NULL, 0);
                if(utemp == ULONG_MAX){
                    perror(argv[0]);
                    fprintf(stderr, "%s: Arg %d out of range.\n",
                            argv[0], i);
                    exit(EXIT_FAILURE);
                }
                if(utemp > (SW_MAX_CHN - 1)){
                    fprintf(stderr, "%s: Arg %d exceeds max chn number.\n",
                            argv[0], i);
                    exit(EXIT_FAILURE);
                }
                if(cnt < SW_MAX_CHN){
                    tgtChnData.header.numChan++;
                    tgtChnData.data[cnt].chanTop.chanNum = utemp;
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
                    args[cnt] = strtoul(argv[i], NULL, 0);
                    if(args[cnt] == LONG_MIN){
                        perror(argv[0]);
                        fprintf(stderr,  "%s: Arg %d under range.\n",
                                argv[0], i);
                        exit(EXIT_FAILURE);
                    }
                    if(args[cnt] == LONG_MAX){
                        perror(argv[0]);
                        fprintf(stderr, "%s: Arg %d over range.\n",
                                argv[0], i);
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
        myDeviceEntry = findDevice(mySWAddress, devices, numDevices);
        if(myDeviceEntry == NULL){
            fprintf(stderr, "%s: Error finding my SW device with address "
                    "0x%4.4" PRIxSWAddr ".\n", argv[0], mySWAddress);
            exit(EXIT_FAILURE);
        }
        memcpy(&myDeviceInfo, &(myDeviceEntry->devInfo), sizeof(myDeviceInfo));
        /* Lookup Target Device Info */
        tgtDeviceEntry = findDevice(tgtSWAddress, devices, numDevices);
        if(tgtDeviceEntry == NULL){
            fprintf(stderr, "%s: Error finding tgt SW device with address "
                    "0x%4.4" PRIxSWAddr ".\n", argv[0], tgtSWAddress);
            exit(EXIT_FAILURE);
        }
        memcpy(&tgtDeviceInfo, &(tgtDeviceEntry->devInfo),
               sizeof(tgtDeviceInfo));
        
        /* Check for Channel Mismatch */
        if(tgtChnData.header.numChan > tgtDeviceEntry->devInfo.numChan){
            fprintf(stderr, "%s: Target device has %" PRInumChan
                    " channel%s.\n" "You gave arguments for %" PRInumChan
                    " channel%s.\n", argv[0],
                    tgtDeviceEntry->devInfo.numChan,
                    (tgtDeviceEntry->devInfo.numChan == 1) ? "":"s",
                    tgtChnData.header.numChan,
                    (tgtChnData.header.numChan == 1) ? "":"s");
            exit(EXIT_FAILURE);
        }

        /* Assemble Message Body */
        bodyLen = writeSWChannelBody(body, SW_MAX_BODY_LENGTH, &tgtChnData);
        if(bodyLen == SWLENGTH_MAX){
            fprintf(stderr, "%s: Error generating message body.\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        /* Assemble Message */
        msgLen = writeSWMsg(msg, SW_MAX_MSG_LENGTH, &(myDeviceEntry->devInfo),
                            &(tgtDeviceEntry->devInfo), tgtSWType, msgScope,
                            msgType, opcode, body, bodyLen);
        if(msgLen == SWLENGTH_MAX){
            fprintf(stderr, "%s: Error generating message.\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        /* My IP */
        si_me.sin_family = AF_INET;
        si_me.sin_port = htons(LISTENPORT);
        /* TODO: Scope to specific IP */
        si_me.sin_addr.s_addr = htonl(INADDR_ANY);
        
        /* TGT IP */
        si_tgt.sin_family = AF_INET;
        si_tgt.sin_port = htons(SENDPORT);
        si_tgt.sin_addr.s_addr = htonl(tgtDeviceEntry->ipAddr);

        /* Setup Socket */
        in = socket(AF_INET, SOCK_DGRAM, 0);
        if(in < 0){
            perror("socket");
            exit(EXIT_FAILURE);
        }
        out = socket(AF_INET, SOCK_DGRAM, 0);
        if(out < 0){
            perror("out socket");
            exit(EXIT_FAILURE);
        }
        
        /* Bind In Socket */
        b = bind(in, (struct sockaddr*) &si_me, sizeof(si_me));
        if (b < 0){
            perror("in bind");
            exit(EXIT_FAILURE);
        }

        /* Send Message */
        r = sendto(out, msg, msgLen, 0,
                   (struct sockaddr*) &si_tgt, sizeof(si_tgt));
        if(r < 0){
            perror("sendto");
            exit(EXIT_FAILURE);
        }

        /* Print Info (TEST) */
        /*
        fprintf(stdout, "Sent packet to %s:%d\n",
                inet_ntoa(si_tgt.sin_addr), ntohs(si_tgt.sin_port));
        fprintf(stdout, "Message Size: %d\n", r);
        */
        
        /* Rezero and Init Vars */
        targetType = 0;
        msgScope = 0;
        msgType = 0;
        opcode = 0;
        memset(&args, 0, sizeof(args));
        memset(&tgtChnEntries, 0, sizeof(tgtChnEntries));
        memset(&tgtChnData, 0, sizeof(tgtChnData));
        tgtChnData.data = tgtChnEntries;
        for(i = 0; i < SW_MAX_CHN; i++){
            tgtChnData.data[i].chanValue = &(args[i]);
        }
        memset(&msg, 0, sizeof(msg));
        msgLen = 0;
        memset(&body, 0, sizeof(body));
        bodyLen = 0;

        /* Check Response */
        r = recvfrom(in, msg, SW_MAX_MSG_LENGTH, 0,
                     (struct sockaddr*) &si_tgt, &slen);
        if(r < 0){
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        else{
            msgLen = r;
        }
        
        /* Print Info (TEST) */
        /*
        fprintf(stdout, "Received packet from %s:%d\n",
                inet_ntoa(si_tgt.sin_addr), ntohs(si_tgt.sin_port));
        fprintf(stdout, "Message Size: %d\n", msgLen);
        print_payload(msg, msgLen);
        */

        if((msgLen > sizeof(struct SmartWallHeader)) &&
           (msgLen < SW_MAX_MSG_LENGTH)){
            msgLen = readSWMsg(msg, msgLen,
                               &sourceDeviceInfo, &destDeviceInfo,
                               &targetType, &msgScope, &msgType, &opcode,
                               body, &bodyLen, SW_MAX_BODY_LENGTH);
            if(msgLen == SWLENGTH_MAX){
                fprintf(stderr, "%s: Could not read SW message\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            /* Check if Message is For Me */
            if(myDeviceInfo.groupID != destDeviceInfo.groupID){
                fprintf(stderr, "%s: Group mismatch\n", argv[0]);
                fprintf(stderr, "My groupID: %u\n", myDeviceInfo.groupID);
                fprintf(stderr, "Dest groupID: %u\n", destDeviceInfo.groupID);
                exit(EXIT_FAILURE);
            }
            if((myDeviceInfo.swAddr != destDeviceInfo.swAddr) && 
               (destDeviceInfo.swAddr != SW_ADDR_BROADCAST)){
                fprintf(stderr, "%s: Address mismatch\n", argv[0]);
                fprintf(stderr, "My swAddr: 0x%x\n", myDeviceInfo.swAddr);
                fprintf(stderr, "Dest swAddr: 0x%x\n", destDeviceInfo.swAddr);
                exit(EXIT_FAILURE);
            }
            /* TODO: Confirm it's from a correct slave */
            /* Switch on Message Scope */
            switch(msgScope){
            case SW_SCP_CHANNEL:
                {
                    bodyLen = readSWChannelBody(body, bodyLen, &tgtChnData,
                                                &limits);
                    if(bodyLen == SWLENGTH_MAX){
                        fprintf(stderr, "%s: Could not read SW body\n",
                                argv[0]);
                        exit(EXIT_FAILURE);
                    }
                    /* ToDo: Switch on target device type */
                    /* Switch on Message Type: SET OR QUERY */
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
                                sourceDeviceInfo.swAddr,
                                mtStrTemp, dtStrTemp, opcode);
                        for(i = 0; i < tgtChnData.header.numChan; i++){
                            j = tgtChnData.data[i].chanTop.chanNum;
                            if(j < tgtDeviceInfo.numChan){
                                temp = tgtChnData.data[i].chanValue;
                                fprintf(stdout, " %" PRIxNumChan 
                                        " %ld", j, *temp);
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
                    break;
                }
            default:
                {
                    fprintf(stderr, "%s: Unhandeled Message Scope: %x\n",
                            argv[0], msgScope);
                    exit(EXIT_FAILURE);
                    break;
                }
            }
        }
        else{
            fprintf(stderr, "%s: Malformatted message"
                    " - msgLen outside of normal limits.\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        
        /* Close Socket */
        close(in);
        close(out);
        
    }
    
    return 0;
    
}
