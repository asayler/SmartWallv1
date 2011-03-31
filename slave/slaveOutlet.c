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
#include "swOutlet.h"

#define SENDPORT SWINPORT
#define LISTENPORT SWOUTPORT

#define MYSWUID 0x0001000000000001
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

    /* Setup State Vars */
    uint8_t ch0State = CHAN0_INITSTATE;
    uint8_t ch1State = CHAN1_INITSTATE;

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

    /* Init chanData Buffer */
    for(i = 0; i < MYSWCHAN; i++){
        tgtChnData.data[i].chanValue = &(args[i]);
        tgtChnData.header.numChan++;
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
    
    /*Print Base Data */
    /*
    fprintf(stdout, "I am a SW_TYPE_OUTLET.\n");
    fprintf(stdout, "SmartWall UID:     0x%" PRIxDevUID "\n", mySWUID);
    fprintf(stdout, "Device Type:       0x%" PRIxDevType "\n", mySWType);
    fprintf(stdout, "SmartWall Version: 0x%" PRIxSWVer "\n", mySWVersion);
    fprintf(stdout, "SmartWall Address: 0x%" PRIxSWAddr "\n", mySWAddress);
    fprintf(stdout, "SmartWall Group:   0x%" PRIxGrpID "\n", mySWGroup);

    fprintf(stdout, "Channel 1 State:   %u\n", ch1State);
    fprintf(stdout, "Channel 2 State:   %u\n", ch2State);
    */

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
            /* Switch on Message Scope */
            switch(msgScope){
            case SW_SCP_CHANNEL:
                {
                    fprintf(stderr, "Received Channel Message\n");
                    /* TODO: Remove 8 byte limit*/
                    bodyLen = readSWChannelBody(body, bodyLen, &tgtChnData,
                                                myDeviceInfo.numChan, 8);
                    if(bodyLen == SWLENGTH_MAX){
                        fprintf(stderr, "%s: Could not read SW body\n",
                                argv[0]);
                        exit(EXIT_FAILURE);
                    }
                    break;
                }
            default:
                {
                    fprintf(stderr, "%s: Unhandeled Message Scope\n", argv[0]);
                    continue;
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
