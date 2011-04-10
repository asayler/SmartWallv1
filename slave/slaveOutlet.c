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
#include "../com/comTools.h"
#include "../com/comPrint.h"
#include "swOutlet.h"

#define SENDPORT SWINPORT
#define LISTENPORT SWOUTPORT

#define MYSWUID 0x0001000000000011
#define MYSWGROUP 0x01
#define MYSWADDRESS 0x0011
#define MYSWTYPE SW_TYPE_OUTLET | SW_TYPE_UNIVERSAL
#define MYSWCHAN 0x02

#define CHAN0_INITSTATE OUTLET_CHAN_ON;
#define CHAN1_INITSTATE OUTLET_CHAN_ON;

int main(int argc, char *argv[]){

    /* Handel Input */
    (void) argc;
    (void) argv;
    
    /* Temp Vars */
    int i;
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

    /* Setup Socket Vars */
    struct sockaddr_in si_me, si_tgt;
    memset(&si_me, 0, sizeof(si_me));
    memset(&si_tgt, 0, sizeof(si_tgt));
    socklen_t slen = sizeof(struct sockaddr_in);
    int in, out;
    int b, r;

    /* My IP */
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(LISTENPORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    
    /* TGT IP */
    si_tgt.sin_family = AF_INET;
    si_tgt.sin_port = htons(SENDPORT);
    
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
    b = bind(in, (struct sockaddr*) &si_me, sizeof(si_me));
    if (b < 0){
        perror("in bind");
        exit(EXIT_FAILURE);
    }

    while(1){
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
        fprintf(stdout, "Received packet from %s:%d\n",
                inet_ntoa(si_tgt.sin_addr), ntohs(si_tgt.sin_port));
        fprintf(stdout, "Message Size: %d\n", msgLen);
        print_payload(msg, msgLen);

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
                                si_tgt.sin_port = htons(SENDPORT);
                                /* Send Message */
                                r = sendto(out, msg, msgLen, 0,
                                           (struct sockaddr*) &si_tgt,
                                           sizeof(si_tgt));
                                if(r < 0){
                                    perror("sendto");
                                    exit(EXIT_FAILURE);
                                }
                                /* Print Info (TEST) */
                                fprintf(stdout, "Sent packet to %s:%d\n",
                                        inet_ntoa(si_tgt.sin_addr),
                                        ntohs(si_tgt.sin_port));
                                fprintf(stdout, "Message Size: %d\n", r);

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
                    " - msgLen outside of normal limits.\n", argv[0]);
            continue;
        }

    }
    
    close(in);
    close(out);
    
    return 0;
}
