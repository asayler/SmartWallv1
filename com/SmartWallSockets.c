/* Andy Sayler
 * SmartWall Project
 * SmartWall Berkeley Sockets Interface Implmentation
 * SmartWallSockets.c
 * Created 3/12/11
 *
 * Change Log:
 * 03/12/11 - Created
 * ---
 */

#include "SmartWallSockets.h"

int swListen(int inSocket, uint8_t* buffer, swLength_t bufferSize,
             struct SWDeviceInfo* sourceDev){
    
    /* Temp vars */
    int r;
    unsigned addressSize = 0;
    
    r = recvfrom(inSocket, buffer, bufferSize, 0,
                 (struct sockaddr*) &(sourceDev->devIP),
                 &addressSize);
    if(r < 0){
        perror("recvFunc");
        return -1;
    }
    else{
        if(addressSize != sizeof(sourceDev->devIP))
            {
                fprintf(stderr, "%s: recvfrom address size mismatch\n", 
                        "swListen");
                return -1;
            }
        else if(((size_t)r > sizeof(struct SmartWallHeader)) &&
                ((size_t)r < bufferSize)){
#ifdef SWDEBUG
            /* Print Info (DEBUG) */
            fprintf(stderr, "Received packet from 0x%" PRIx32 ":%" PRId16 "\n",
                    ntoh32(sourceDev->devIP.sin_addr.s_addr),
                    ntoh16(sourceDev->devIP.sin_port));
            fprintf(stderr, "Message Size: %d\n", r);
            print_payload(buffer, r);
#endif
            return r;
        }
        else {
            fprintf(stderr, "%s: Malformatted message"
                    " - msgLen outside of normal limits: %d.\n", "swListen",
                    r);
            return -1;
        }
    }
}

int swSend(const int outSocket,
           const uint8_t* msg, const swLength_t msgSize,
           const struct SWDeviceInfo* destDev){
 
    int s;
   
    s = sendto(outSocket, msg, msgSize, 0,
               (struct sockaddr*)
               &(destDev->devIP),
               sizeof(destDev->devIP));
    if(s < 0){
        perror("sendto");
        return -1;
    }

    return s;
}

enum swReceiveState swReceive(const uint8_t* msg, const swLength_t msgLen,
                              struct SWDeviceInfo* sourceDev,
                              struct SWDeviceInfo* msgDev,
                              devType_t* targetType,
                              msgScope_t* msgScope,
                              msgType_t* msgType,
                              swOpcode_t* opcode,
                              void* body,
                              swLength_t* bodyLength,
                              const swLength_t maxBodyLength){
    
    int readLen = 0;

    readLen = readSWMsg(msg, msgLen,
                        &(sourceDev->devInfo), &(msgDev->devInfo),
                        targetType, msgScope, msgType, opcode,
                        body, bodyLength, maxBodyLength);
    if(msgLen == SWLENGTH_MAX){
        fprintf(stderr, "%s: Could not read SW message\n", "swReceive");
        return SWRECEIVE_ERROR;
    }
    if(readLen != msgLen){
        fprintf(stderr, "%s: Message length does not match read length\n",
                "swReceive");
        return SWRECEIVE_ERROR;
    }

    return SWRECEIVE_SUCCESS;

}

int swCompose(uint8_t* buffer, const swLength_t bufferSize,
              const struct SWDeviceInfo* myDev,
              const struct SWDeviceInfo* destDev,
              const devType_t targetType,
              const msgScope_t msgScope,
              const msgType_t msgType,
              const swOpcode_t opcode,
              const void* body,
              const swLength_t bodyLength){
    
    int w = 0;

    /* Assemble Message */
    w = writeSWMsg(buffer, bufferSize,
                   &(myDev->devInfo),
                   &(destDev->devInfo),
                   targetType,
                   msgScope,
                   msgType, opcode,
                   body, bodyLength);
    if(w == SWLENGTH_MAX){
        fprintf(stderr, "%s: Error generating message.\n", "swCompose");
        return -1;
    }
    else {
        return w;
    }
    
}

enum swCheckState swCheck(const struct SWDeviceInfo* myDev,
                          const struct SWDeviceInfo* msgDev,
                          const devType_t targetType,
                          msgScope_t* errorScope,
                          swOpcode_t* errorOpcode){
    
    *errorScope = 0;
    *errorOpcode = 0;
    
    /* Check if Message is For Me */
    if(myDev->devInfo.groupID != msgDev->devInfo.groupID){
#ifdef SWDEBUG
        fprintf(stderr, "%s: Group mismatch\n", "swCheck");
        fprintf(stderr, "My  groupID: %u\n", myDev->devInfo.groupID);
        fprintf(stderr, "Msg groupID: %u\n", msgDev->devInfo.groupID);
#endif
        return SWCHECK_NOT_FOR_ME;
    }
    if((myDev->devInfo.swAddr != msgDev->devInfo.swAddr) && 
       (msgDev->devInfo.swAddr != SW_ADDR_BROADCAST)){
#ifdef SWDEBUG
        fprintf(stderr, "%s: Address mismatch\n", "swCheck");
        fprintf(stderr, "My  swAddr: 0x%x\n", myDev->devInfo.swAddr);
        fprintf(stderr, "Msg swAddr: 0x%x\n", msgDev->devInfo.swAddr);
#endif
        return SWCHECK_NOT_FOR_ME;
    }
    if(!(myDev->devInfo.devTypes & targetType)){
#ifdef SWDEBUG
        fprintf(stderr, "%s: Type mismatch\n", "swCheck");
#endif
        *errorScope = SW_SCP_DEVICE;
        *errorOpcode = UNIVERSAL_DEV_OP_ERROR_TYPEMISMATCH;
        return SWCHECK_ERROR;
    }

    /* TODO: Confirm it's from a master */

    return SWCHECK_FOR_ME;

}

enum processorState swProcess(const msgScope_t msgScope,
                              const msgType_t msgType,
                              const swOpcode_t opcode,
                              struct SWProcessor* processors,
                              const int numProcessors,
                              const void* inputBody,
                              const swLength_t inBodyLength,
                              void* outputBody,
                              swLength_t* outBodyLength,
                              const swLength_t maxBodyLength,
                              void* deviceState,
                              msgScope_t* errorScope,
                              swOpcode_t* errorOpcode){
    
    /* Local Vars */
    int i = 0;
    swLength_t bodyLength = 0;
    
    /* Loop Through Handelers */
    for(i = 0; i < numProcessors; i++){
        if(msgScope == processors[i].processorScope){
            bodyLength = processors[i].decoder(inputBody, inBodyLength,
                                               processors[i].data,
                                               processors[i].dataLimits);
            if(bodyLength != inBodyLength){
                fprintf(stderr, "%s: 0x%" PRIxMsgScope
                        " scope decoder failed - length mismatch\n",
                        "handelerState", processors[i].processorScope);
                return PROCESSOR_ERROR;
            }
            if(processors[i].handeler(opcode, msgType, processors[i].data,
                                      processors[i].data, deviceState,
                                      errorScope, errorOpcode)){
                fprintf(stderr, "%s: 0x%" PRIxMsgScope
                        " scope handeler failed\n",
                        "handelerState", processors[i].processorScope);
                return PROCESSOR_ERROR;
            }
            if(*errorScope != 0){
                return PROCESSOR_SUCCESS;
            }
            *outBodyLength = processors[i].encoder(outputBody, maxBodyLength,
                                                   processors[i].data);
            if(*outBodyLength == SWLENGTH_MAX){
                fprintf(stderr, "%s: 0x%" PRIxMsgScope
                        " scope encoder failed\n",
                        "handelerState", processors[i].processorScope);
                return PROCESSOR_ERROR;
            }
            return PROCESSOR_SUCCESS;
        }
    }
    return PROCESSOR_ERROR;
}
