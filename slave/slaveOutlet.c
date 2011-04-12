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
#include "swUniversal.h"

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

enum SWOutletState{OST_SETUP, OST_LISTEN, OST_RECEIVE, OST_CHECK, OST_PROCESS,
                   OST_COMPOSE, OST_SEND, OST_CLEANUP};

enum SWOutletState swOutletStateMachine(enum SWOutletState machineState,
                                        struct outletDeviceState* myState);

int main(int argc, char *argv[]){

    /* Handel Input */
    (void) argc;
    (void) argv;
    
    /* Setup State Vars */
    struct outletDeviceState myState;
    memset(&myState, 0, sizeof(myState));
    outletChanState_t chState[MYSWCHAN];
    memset(&chState, 0, sizeof(chState));
    outletChanPower_t chPower[MYSWCHAN];
    memset(&chPower, 0, sizeof(chPower));
    struct SWDeviceInfo myDevice;
    memset(&myDevice, 0, sizeof(myDevice));
    myState.myDev = &myDevice;
    myState.chState = chState;
    myState.chPower = chPower;
    enum SWOutletState machineState = OST_SETUP;
    
    /* Setup SW Vars */
    myState.myDev->devInfo.swAddr = MYSWADDRESS;
    myState.myDev->devInfo.devTypes = MYSWTYPE;
    myState.myDev->devInfo.numChan = MYSWCHAN;
    myState.myDev->devInfo.version = SW_VERSION;
    myState.myDev->devInfo.uid = MYSWUID;
    myState.myDev->devInfo.groupID = MYSWGROUP;
    
    /* My IP */
    myState.myDev->devIP.sin_family = AF_INET;
    myState.myDev->devIP.sin_port = htons(LISTENPORT);
    myState.myDev->devIP.sin_addr.s_addr = htonl(INADDR_ANY);

    while(1){

        machineState = swOutletStateMachine(machineState, &myState);
                
    }

    return 0;
}


enum SWOutletState swOutletStateMachine(enum SWOutletState machineState,
                                        struct outletDeviceState* myState){

    /* Local Temp Vars */
    unsigned int i = 0;
    int b, r;

    /* Setup Temporary Storage Vars */
    struct SWChannelEntry tmpChnEntries[MYSWCHAN];
    memset(&tmpChnEntries, 0, sizeof(tmpChnEntries));
    struct SWChannelData tmpChnData;
    memset(&tmpChnData, 0, sizeof(tmpChnData));
    union outletChanArg tmpChanArgs[MYSWCHAN];
    memset(&tmpChanArgs, 0, sizeof(tmpChanArgs));
    tmpChnData.data = tmpChnEntries;
    /* Init chanData Buffer */
    for(i = 0; i < MYSWCHAN; i++){
        tmpChnData.data[i].chanValue = &(tmpChanArgs[i]);
    }
    struct SWChannelLimits limits;
    limits.maxNumChan = MYSWCHAN;
    limits.maxDataLength = sizeof(*tmpChanArgs);
    
    /* Declare Temp State Machine Vars */
    struct SWProcessor processors[NUMOUTLETPROCESSORS];
    memset(processors, 0, sizeof(processors));
    processors[0].processorScope = SW_SCP_CHANNEL;
    processors[0].data = &tmpChnData;
    processors[0].dataLimits = &limits;
    processors[0].decoder = (readSWBody)readSWChannelBody;
    processors[0].handeler = (swHandeler)outletChnHandeler;
    processors[0].encoder = (writeSWBody)writeSWChannelBody;

    /* Declare Static SW Vars */
    static struct SWDeviceInfo sourceDevice;
    static struct SWDeviceInfo msgDevice;    
    static devType_t targetType;
    static msgScope_t msgScope;
    static msgType_t msgType;
    static swOpcode_t opcode;
    static swOpcode_t errorOpcode = 0;
    static msgScope_t errorScope = 0;
    
    /* Declare Static SW Buffers */
    static uint8_t msg[SW_MAX_MSG_LENGTH];
    static swLength_t msgLen;
    static uint8_t body[SW_MAX_BODY_LENGTH];
    static swLength_t bodyLen;

    /* Declare Static Socket vars */
    static int in, out;

    switch(machineState){
    case OST_SETUP:

        /* Setup Static SW Vars */
        memset(&sourceDevice, 0, sizeof(sourceDevice));
        memset(&msgDevice, 0, sizeof(msgDevice));
        targetType = 0;
        msgScope = 0;
        msgType = 0;;
        opcode = 0;

        /* Setup Static SW Buffers */
        msgLen = 0;
        memset(&msg, 0, sizeof(msg));
        bodyLen = 0;
        memset(&body, 0, sizeof(body));

        /* Setup Sockets */
        in = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(in < 0){
            perror("in socket");
            return OST_SETUP;
        }
        out = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(out < 0){
            perror("out socket");
            return OST_SETUP;
        }

        /* Bind In Socket */
        b = bind(in, (struct sockaddr*) &(myState->myDev->devIP),
                 sizeof(myState->myDev->devIP));
        if (b < 0){
            perror("in bind");
            return OST_SETUP;
        }
        
        return OST_LISTEN;
        break;

    case OST_LISTEN:

        /* Listen for New Message */
        msgLen = swListen(in, msg, SW_MAX_BODY_LENGTH, &sourceDevice);
        if(msgLen > 0){
            return OST_RECEIVE;
        }
        else{
            return OST_LISTEN;
        }
        break;

    case OST_RECEIVE:

        /* Receive Message */
        if(swReceive(msg, msgLen, &sourceDevice, &msgDevice, &targetType,
                     &msgScope, &msgType, &opcode, body, &bodyLen,
                     SW_MAX_BODY_LENGTH)){
            return OST_LISTEN;
        }
        else{
            return OST_CHECK;
        }
        break;

    case OST_CHECK:
        
        /* Check Received Message */
        if(swCheck(myState->myDev, &msgDevice, targetType, &errorScope,
                   &errorOpcode)){
            return OST_LISTEN;
        }
        else{
            return OST_PROCESS;
        }
        break;

    case OST_PROCESS:
        
        /* Process Message */
        if(swProcess(msgScope, msgType, opcode,
                     processors, NUMOUTLETPROCESSORS,
                     body, bodyLen,
                     body, &bodyLen, SW_MAX_BODY_LENGTH,
                     myState, &errorScope, &errorOpcode)){
            return OST_LISTEN;
        }
        else{
            return OST_COMPOSE;
        }
        break;

    case OST_COMPOSE:

        /* Check for Error Message */
        if(errorScope != 0){
            opcode = errorOpcode;
            msgType = SW_MSG_ERROR;
            msgScope = errorScope;
        }
        else{
            msgType = SW_MSG_REPORT;
        }
        /* Compose Message */
        msgLen = swCompose(msg, SW_MAX_MSG_LENGTH,
                           myState->myDev, &sourceDevice,
                           targetType,
                           msgScope,
                           msgType, opcode,
                           body, bodyLen);
        if(msgLen > 0){
            return OST_SEND;
        }
        else {
            return OST_LISTEN;
        }
        break;
        
    case OST_SEND:
        
        /* Set Port */
        sourceDevice.devIP.sin_port = htons(SENDPORT);
        /* Send Message */
        r = swSend(out, msg, msgLen, &sourceDevice);
        if(r > 0){
            return OST_LISTEN;
        }
        else {
            return OST_SEND;
        }
        break;

    case OST_CLEANUP:
        
        /* Close Sockets */
        close(in);
        close(out);
        return OST_SETUP;
        break;

    default:
        return OST_SETUP;
    }
}
