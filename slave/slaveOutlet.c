 /* Andy Sayler
 * SmartWall Project
 * SmartWall Slave Outlet Sim
 * slaveOutlet.c
 * Created 2/2/11
 *
 * Change Log:
 * 02/02/11 - Created
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

#include <fcntl.h>
#include <netdb.h>

#include "../com/SmartWall.h"
#include "../com/SmartWallSockets.h"
#include "../com/comTools.h"
#include "../com/comPrint.h"
#include "swOutlet.h"

#define SENDPORT SWINPORT
#define LISTENPORT SWOUTPORT

#define MYSWUID 0x0001000000000011ull
#define MYSWGROUP 0x01u
#define MYSWADDRESS 0x0011u
#define MYSWTYPE SW_TYPE_OUTLET | SW_TYPE_UNIVERSAL
#define MYSWCHAN 0x02u

#define CHAN0_INITSTATE OUTLET_CHAN_ON;
#define CHAN1_INITSTATE OUTLET_CHAN_ON;

#define SWDEBUG

int swListen(int inSocket, uint8_t* buffer, swLength_t bufferSize,
             struct SWDeviceInfo* fromDevInfo);

int main(int argc, char *argv[]){

    /* Handel Input */
    (void) argc;
    (void) argv;
    
    /* Temp Vars */
    unsigned int i;
    numChan_t j;

    /* Setup State Vars */
    unsigned long chState[MYSWCHAN];
    unsigned long* temp = NULL;
    memset(&chState, 0, sizeof(chState));
    
    /* Setup SW Message Vars */
    struct SmartWallDev sourceDeviceInfo;
    memset(&sourceDeviceInfo, 0, sizeof(sourceDeviceInfo));
    struct SmartWallDev destDeviceInfo;
    memset(&destDeviceInfo, 0, sizeof(destDeviceInfo));
    devType_t targetType = 0;
    msgScope_t msgScope = 0;
    msgType_t msgType = 0;
    swOpcode_t opcode = 0;
    long int args[MYSWCHAN];
    memset(&args, 0, sizeof(args));
    struct SWChannelEntry tgtChnEntries[MYSWCHAN];
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

    /* Init chanData Buffer */
    for(i = 0; i < MYSWCHAN; i++){
        tgtChnData.data[i].chanValue = &(args[i]);
    }

    /* Setup SW Vars */
    struct SmartWallDev myDeviceInfo;    
    memset(&myDeviceInfo, 0, sizeof(myDeviceInfo));
    myDeviceInfo.swAddr = MYSWADDRESS;
    myDeviceInfo.devTypes = MYSWTYPE;
    myDeviceInfo.numChan = MYSWCHAN;
    myDeviceInfo.version = SW_VERSION;
    myDeviceInfo.uid = MYSWUID;
    myDeviceInfo.groupID = MYSWGROUP;
    struct SWDeviceInfo myDevice;
    memset(&myDevice, 0, sizeof(myDevice));
    myDevice.devInfo.swAddr = MYSWADDRESS;
    myDevice.devInfo.devTypes = MYSWTYPE;
    myDevice.devInfo.numChan = MYSWCHAN;
    myDevice.devInfo.version = SW_VERSION;
    myDevice.devInfo.uid = MYSWUID;
    myDevice.devInfo.groupID = MYSWGROUP;
    
    struct SWDeviceInfo tgtDevice;
    memset(&tgtDevice, 0, sizeof(tgtDevice));

    /* Setup Socket Vars */
    int in, out;
    int b, r;

    /* My IP */
    myDevice.devIP.sin_family = AF_INET;
    myDevice.devIP.sin_port = htons(LISTENPORT);
    myDevice.devIP.sin_addr.s_addr = htonl(INADDR_ANY);
    
    /* TGT IP */
    tgtDevice.devIP.sin_family = AF_INET;
    tgtDevice.devIP.sin_port = htons(SENDPORT);
    
    /* Setup Sockets */
    in = socket(AF_INET, SOCK_DGRAM, 0);
    if(in < 0){
        perror("in socket");
        exit(EXIT_FAILURE);
    }
    out = socket(AF_INET, SOCK_DGRAM, 0);
    if(out < 0){
        perror("out socket");
        exit(EXIT_FAILURE);
    }

    /* Bind In Socket */
    b = bind(in, (struct sockaddr*) &(myDevice.devIP),
             sizeof(myDevice.devIP));
    if (b < 0){
        perror("in bind");
        exit(EXIT_FAILURE);
    }

    while(1){

        msgLen = swListen(in, msg, SW_MAX_BODY_LENGTH, &tgtDevice);
        
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
                continue;
            }
            if((myDeviceInfo.swAddr != destDeviceInfo.swAddr) && 
               (destDeviceInfo.swAddr != SW_ADDR_BROADCAST)){
                fprintf(stderr, "%s: Address mismatch\n", argv[0]);
                fprintf(stderr, "My swAddr: 0x%x\n", myDeviceInfo.swAddr);
                fprintf(stderr, "Dest swAddr: 0x%x\n", destDeviceInfo.swAddr);
                continue;
            }
            if(!(myDeviceInfo.devTypes & targetType)){
                fprintf(stderr, "%s: Type mismatch\n", argv[0]);
                continue;
            }
            /* TODO: Confirm it's from a master */
            /* Switch on Message Scope */
            switch(msgScope){
            case SW_SCP_CHANNEL:
                {
                    fprintf(stdout, "Received Channel Message\n");
                    /* TODO: Remove 8 byte limit*/
                    bodyLen = readSWChannelBody(body, bodyLen, &tgtChnData,
                                                myDeviceInfo.numChan,
                                                sizeof(unsigned long));
                    if(bodyLen == SWLENGTH_MAX){
                        fprintf(stderr, "%s: Could not read SW body\n",
                                argv[0]);
                        exit(EXIT_FAILURE);
                    }
                    /* Switch on Opcode */
                    switch(opcode){
                    case OUTLET_CH_OP_STATE:
                        {
                            /* Switch on Message Type: SET OR QUERY */
                            switch(msgType){
                            case SW_MSG_SET:
                                /* Set State */
                                for(i = 0; i < tgtChnData.header.numChan; i++){
                                    j = tgtChnData.data[i].chanTop.chanNum;
                                    if(j < myDeviceInfo.numChan){
                                        /* TODO: Check valid state */
                                        temp = tgtChnData.data[i].chanValue;
                                        chState[j] = *temp;
                                        fprintf(stdout, "Ch %u Set to %lu\n",
                                                j, chState[j]);
                                    }
                                    else{
                                        fprintf(stderr,
                                                "%s: Invalid chanNum\n",
                                                argv[0]);
                                    }
                                }
                            case SW_MSG_QUERY:
                                /* Report State */
                                for(i = 0; i < tgtChnData.header.numChan; i++){
                                    j = tgtChnData.data[i].chanTop.chanNum;
                                    if(j < myDeviceInfo.numChan){
                                        temp = tgtChnData.data[i].chanValue;
                                        *temp = chState[j];
                                    }
                                    else{
                                        fprintf(stderr,
                                                "%s: Invalid chanNum\n",
                                                argv[0]);
                                    }
                                }
                                /* TODO: Remove 8 byte limit */
                                /*tgtChnData.header.dataLength = 
                                  sizeof(unsigned long);*/
                                /* Assemble Message Body */
                                bodyLen=writeSWChannelBody(body,
                                                           SW_MAX_BODY_LENGTH,
                                                           &tgtChnData);
                                if(bodyLen == SWLENGTH_MAX){
                                    fprintf(stderr, "%s: Error generating "
                                            "message body.\n", argv[0]);
                                    exit(EXIT_FAILURE);
                                }
                                /* Assemble Message */
                                msgLen = writeSWMsg(msg, SW_MAX_MSG_LENGTH,
                                                    &myDeviceInfo,
                                                    &sourceDeviceInfo,
                                                    SW_TYPE_OUTLET,
                                                    SW_SCP_CHANNEL,
                                                    SW_MSG_REPORT, opcode,
                                                    body, bodyLen);
                                if(msgLen == SWLENGTH_MAX){
                                    fprintf(stderr, "%s: Error generating "
                                            "message.\n", argv[0]);
                                    exit(EXIT_FAILURE);
                                }
                                /* Print Test payload */
                                fprintf(stdout, "Response: \n");
                                print_payload(msg, msgLen);
                                /* Set Port */
                                tgtDevice.devIP.sin_port = htons(SENDPORT);
                                /* Send Message */
                                r = sendto(out, msg, msgLen, 0,
                                           (struct sockaddr*)
                                           &(tgtDevice.devIP),
                                           sizeof(tgtDevice.devIP));
                                if(r < 0){
                                    perror("sendto");
                                    exit(EXIT_FAILURE);
                                }
                                
                                break;
                            default:
                                fprintf(stderr, "%s: Unhandeled msgType\n",
                                        argv[0]);
                                continue;
                                break;
                            }
                            break;
                        }
                    default:
                        {
                            fprintf(stderr, "%s: Unhandeled Opcode\n",
                                    argv[0]);
                            continue;
                            break;
                        }
                    }
                    break;
                }
            default:
                {
                    fprintf(stderr, "%s: Unhandeled Message Scope\n", argv[0]);
                    continue;
                    break;
                }
            }
        }
        else{
            fprintf(stderr, "%s: Malformatted message"
                    " - msgLen outside of normal limits: %d.\n", argv[0],
                    msgLen);
            continue;
        }

    }
    
    close(in);
    close(out);
    
    return 0;
}

int swListen(int inSocket, uint8_t* buffer, swLength_t bufferSize,
             struct SWDeviceInfo* fromDevInfo){

    /* Temp vars */
    int r;
    unsigned addressSize;
        
    r = recvfrom(inSocket, buffer, bufferSize, 0,
                 (struct sockaddr*) &(fromDevInfo->devIP),
                 &addressSize);

    if(r < 0){
        perror("recvFunc");
        return -1;
    }
    else{
        if(addressSize != sizeof(fromDevInfo->devIP))
            {
                fprintf(stderr, "revfrom address size mismatch\n");
                return -1;
            }
        else {
#ifdef SWDEBUG
            /* Print Info (TEST) */
            fprintf(stderr, "Received packet from 0x%" PRIx32 ":%" PRId16 "\n",
                    ntoh32(fromDevInfo->devIP.sin_addr.s_addr),
                    ntoh16(fromDevInfo->devIP.sin_port));
            fprintf(stderr, "Message Size: %d\n", r);
            print_payload(buffer, r);
            return r;
        }
#endif
    }
}
