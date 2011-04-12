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

enum handelerState outletChnHandeler(const swOpcode_t chnOpcode,
                                     const msgType_t msgType,
                                     const struct SWChannelData* input,
                                     struct SWChannelData* output,
                                     struct outletDeviceState* deviceState,
                                     msgScope_t* errorScope,
                                     swOpcode_t* errorOpcode);


int main(int argc, char *argv[]){

    /* Handel Input */
    (void) argc;
    (void) argv;
    
    /* Temp Vars */
    unsigned int i;
    
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
    
    /* Setup Temporary Vars */
    struct SWChannelEntry tmpChnEntries[MYSWCHAN];
    memset(&tmpChnEntries, 0, sizeof(tmpChnEntries));
    struct SWChannelData tmpChnData;
    memset(&tmpChnData, 0, sizeof(tmpChnData));
    outletChanState_t tmpState[MYSWCHAN];
    memset(&tmpState, 0, sizeof(tmpState));
    tmpChnData.data = tmpChnEntries;
    /* Init chanData Buffer */
    for(i = 0; i < MYSWCHAN; i++){
        tmpChnData.data[i].chanValue = &(tmpState[i]);
    }
    struct SWChannelLimits limits;
    limits.maxNumChan = MYSWCHAN;
    limits.maxDataLength = sizeof(*chState);
    
    /* Setup Temp State Machine Vars */
    struct SWProcessor processors[1];
    memset(processors, 0, sizeof(processors));
    processors[0].processorScope = SW_SCP_CHANNEL;
    processors[0].data = &tmpChnData;
    processors[0].dataLimits = &limits;
    processors[0].decoder = (readSWBody)readSWChannelBody;
    processors[0].handeler = (swHandeler)outletChnHandeler;
    processors[0].encoder = (writeSWBody)writeSWChannelBody;

    /* Setup SW Message Vars */
    struct SmartWallDev sourceDeviceInfo;
    memset(&sourceDeviceInfo, 0, sizeof(sourceDeviceInfo));
    struct SmartWallDev destDeviceInfo;
    memset(&destDeviceInfo, 0, sizeof(destDeviceInfo));
    
    devType_t targetType = 0;
    msgScope_t msgScope = 0;
    msgType_t msgType = 0;
    swOpcode_t opcode = 0;
    swOpcode_t errorOpcode = 0;
    msgScope_t errorScope = 0;
        
    uint8_t msg[SW_MAX_MSG_LENGTH];
    memset(&msg, 0, sizeof(msg));
    swLength_t msgLen = 0;
    uint8_t body[SW_MAX_BODY_LENGTH];
    memset(&body, 0, sizeof(body));
    swLength_t bodyLen = 0;
    
    /* Setup SW Vars */
    
    myDevice.devInfo.swAddr = MYSWADDRESS;
    myDevice.devInfo.devTypes = MYSWTYPE;
    myDevice.devInfo.numChan = MYSWCHAN;
    myDevice.devInfo.version = SW_VERSION;
    myDevice.devInfo.uid = MYSWUID;
    myDevice.devInfo.groupID = MYSWGROUP;
    
    struct SWDeviceInfo sourceDevice;
    memset(&sourceDevice, 0, sizeof(sourceDevice));
    struct SWDeviceInfo msgDevice;
    memset(&msgDevice, 0, sizeof(msgDevice));

    /* Setup Socket Vars */
    int in, out;
    int b, r;

    /* My IP */
    myDevice.devIP.sin_family = AF_INET;
    myDevice.devIP.sin_port = htons(LISTENPORT);
    myDevice.devIP.sin_addr.s_addr = htonl(INADDR_ANY);
        
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

        msgLen = swListen(in, msg, SW_MAX_BODY_LENGTH, &sourceDevice);
        if(!(msgLen > 0)){
            continue;
        }

        if(swReceive(msg, msgLen, &sourceDevice, &msgDevice, &targetType,
                     &msgScope, &msgType, &opcode, body, &bodyLen,
                     SW_MAX_BODY_LENGTH)){
            continue;
        }

        if(swCheck(&myDevice, &msgDevice, targetType, &errorScope,
                   &errorOpcode)){
            continue;
        }
        
        /* Switch on Message Scope */
        switch(msgScope){
        case SW_SCP_CHANNEL:
            {
                fprintf(stdout, "Received Channel Message\n");
                /* TODO: Remove 8 byte limit*/
                bodyLen = readSWChannelBody(body, bodyLen, &tmpChnData,
                                            &limits);
                if(bodyLen == SWLENGTH_MAX){
                    fprintf(stderr, "%s: Could not read SW body\n",
                            argv[0]);
                    exit(EXIT_FAILURE);
                }
                
                if(outletChnHandeler(opcode, msgType, &tmpChnData, 
                                     &tmpChnData, &myState,
                                     &errorScope, &errorOpcode)){
                    continue;
                }
                
                /* Assemble Message Body */
                bodyLen=writeSWChannelBody(body,
                                           SW_MAX_BODY_LENGTH,
                                           &tmpChnData);
                if(bodyLen == SWLENGTH_MAX){
                    fprintf(stderr, "%s: Error generating "
                            "message body.\n", argv[0]);
                    exit(EXIT_FAILURE);
                }

                /* Compose Message */
                if(errorScope != 0){
                    opcode = errorOpcode;
                    msgType = SW_MSG_ERROR;
                }
                else{
                    msgType = SW_MSG_REPORT;
                }
                msgLen = swCompose(msg, SW_MAX_MSG_LENGTH,
                                   &myDevice, &sourceDevice,
                                   SW_TYPE_OUTLET,
                                   SW_SCP_CHANNEL,
                                   msgType, opcode,
                                   body, bodyLen);

                if(!(msgLen > 0)){
                    continue;
                }

                /* Set Port */
                sourceDevice.devIP.sin_port = htons(SENDPORT);

                /* Send Message */
                r = swSend(out, msg, msgLen, &sourceDevice);
                if(!(r > 0)){
                    continue;
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
        
    close(in);
    close(out);

    return 0;
}

enum handelerState outletChnHandeler(const swOpcode_t chnOpcode,
                                     const msgType_t msgType,
                                     const struct SWChannelData* input,
                                     struct SWChannelData* output,
                                     struct outletDeviceState* deviceState,
                                     msgScope_t* errorScope,
                                     swOpcode_t* errorOpcode){
    
    /* Local vars */
    unsigned int i;
    numChan_t chn;
    *errorScope = 0;
    *errorOpcode = 0;

    /* Switch on Opcode */
    switch(chnOpcode){
    case OUTLET_CH_OP_STATE:
        {
            /* Switch on Message Type: SET OR QUERY */
            switch(msgType){
            case SW_MSG_SET:
                /* Set State */
                for(i = 0; i < input->header.numChan; i++){
                    chn = input->data[i].chanTop.chanNum;
                    if(chn < deviceState->myDev->devInfo.numChan){
                        /* TODO: Check valid state */
                        deviceState->chState[chn] =
                            *((outletChanState_t*)input->data[i].chanValue);
#ifdef SWDEBUG
                        fprintf(stdout, "Ch %u Set to %lu\n",
                                chn, deviceState->chState[chn]);
#endif
                    }
                    else{
                        fprintf(stderr,
                                "%s: Invalid chanNum\n", "outletChnHandeler");
                        *errorScope = SW_SCP_CHANNEL;
                        *errorOpcode = UNIVERSAL_CHN_OP_ERROR_BADCHN;
                        return HANDELER_SUCCESS; 
                    }
                }
            case SW_MSG_QUERY:
                /* Report State */
                output->header = input->header;
                for(i = 0; i < input->header.numChan; i++){
                    chn = input->data[i].chanTop.chanNum;
                    if(chn < deviceState->myDev->devInfo.numChan){
                        output->data[i].chanTop.chanNum = chn;
                        *((outletChanState_t*)output->data[i].chanValue) =
                          deviceState->chState[chn];
                    }
                    else{
                        fprintf(stderr,
                                "%s: Invalid chanNum\n", "outletChnHandeler");
                        *errorScope = SW_SCP_CHANNEL;
                        *errorOpcode = UNIVERSAL_CHN_OP_ERROR_BADCHN;
                        return HANDELER_SUCCESS; 
                    }
                }
                return HANDELER_SUCCESS;
                break;
            default:
                fprintf(stderr, "%s: Unhandeled msgType\n",
                        "outletChnHandeler");
                *errorScope = SW_SCP_CHANNEL;
                *errorOpcode = UNIVERSAL_CHN_OP_ERROR_BADMSGTYPE;
                return HANDELER_SUCCESS;
            }
        }
    default:
        {
            fprintf(stderr, "%s: Unhandeled Opcode\n",
                    "outletChnHandeler");
            *errorScope = SW_SCP_CHANNEL;
            *errorOpcode = UNIVERSAL_CHN_OP_ERROR_BADOPCODE;
            return HANDELER_SUCCESS;
        }
    }
    
    return HANDELER_ERROR;

}
