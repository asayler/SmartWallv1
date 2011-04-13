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

enum swSetup swSetup(int* inSocket, int* outSocket,
                     const struct SWDeviceInfo* myDev){
    
    /* Temp Vars */
    int b = 0;

    /* Setup Sockets */
    *inSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(*inSocket < 0){
        perror("socket - in");
        return SWSETUP_ERROR;
    }
    *outSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(*outSocket < 0){
        perror("socket - out");
        return SWSETUP_ERROR;
    }
    
    /* Bind In Socket */
    b = bind(*inSocket, (struct sockaddr*) &(myDev->devIP),
             sizeof(myDev->devIP));
    if (b < 0){
        perror("in bind");
        return SWSETUP_ERROR;
    }

    return SWSETUP_SUCCESS;
    
}

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
    else if(r == 0){
		return r;	
	}	 
    else{
        if(((size_t)r > sizeof(struct SmartWallHeader)) &&
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

enum SWReceiverState swReceiverStateMachine(enum SWReceiverState machineState,
                                            const struct SWDeviceInfo* myDev,
                                            const struct SWDeviceInfo* tgtDev,
                                            void* deviceState,
                                            struct SWProcessor* processors,
                                            const int numProcessors){

    /* Local Temp Vars */
    int r = 0;

    /* Declare Static SW Vars */
    static struct SWDeviceInfo sourceDevice;
    static struct SWDeviceInfo msgDevice;    
    static devType_t targetType;
    static msgScope_t msgScope;
    static msgType_t msgType;
    static swOpcode_t opcode;
    static swOpcode_t errorOpcode;
    static msgScope_t errorScope;
    
    /* Declare Static SW Buffers */
    static uint8_t msg[SW_MAX_MSG_LENGTH];
    static swLength_t msgLen;
    static uint8_t body[SW_MAX_BODY_LENGTH];
    static swLength_t bodyLen;

    /* Declare Static Socket vars */
    static int in, out;

    switch(machineState){
    case RST_SETUP:

        /* Setup Static SW Vars */
        memset(&sourceDevice, 0, sizeof(sourceDevice));
        memset(&msgDevice, 0, sizeof(msgDevice));
        targetType = 0;
        msgScope = 0;
        msgType = 0;
        opcode = 0;
        errorOpcode = 0;
        errorScope = 0;    

        /* Setup Static SW Buffers */
        msgLen = 0;
        memset(&msg, 0, sizeof(msg));
        bodyLen = 0;
        memset(&body, 0, sizeof(body));

        if(swSetup(&in, &out, myDev)){
            return RST_SETUP;
        }
        else{
            return RST_LISTEN;    
        }
        break;

    case RST_LISTEN:

        /* Listen for New Message */
        msgLen = swListen(in, msg, SW_MAX_BODY_LENGTH, &sourceDevice);
        if(msgLen > 0){
            return RST_RECEIVE;
        }
        else{
            return RST_LISTEN;
        }
        break;

    case RST_RECEIVE:

        /* Receive Message */
        if(swReceive(msg, msgLen, &sourceDevice, &msgDevice, &targetType,
                     &msgScope, &msgType, &opcode, body, &bodyLen,
                     SW_MAX_BODY_LENGTH)){
            return RST_LISTEN;
        }
        else{
            return RST_CHECK;
        }
        break;

    case RST_CHECK:
        
        /* Check Received Message */
        if(swCheck(myDev, &msgDevice, targetType, &errorScope,
                   &errorOpcode)){
            return RST_LISTEN;
        }
        else{
            return RST_PROCESS;
        }
        break;

    case RST_PROCESS:
        
        /* Process Message */
        if(swProcess(msgScope, msgType, opcode,
                     processors, numProcessors,
                     body, bodyLen,
                     body, &bodyLen, SW_MAX_BODY_LENGTH,
                     deviceState, &errorScope, &errorOpcode)){
            return RST_LISTEN;
        }
        else{
            return RST_COMPOSE;
        }
        break;

    case RST_COMPOSE:

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
                           myDev, &sourceDevice,
                           targetType,
                           msgScope,
                           msgType, opcode,
                           body, bodyLen);
        if(msgLen > 0){
            return RST_SEND;
        }
        else {
            return RST_LISTEN;
        }
        break;
        
    case RST_SEND:
        
        /* Set Port */
        sourceDevice.devIP.sin_port = tgtDev->devIP.sin_port;
        /* Send Message */
        r = swSend(out, msg, msgLen, &sourceDevice);
        if(r > 0){
            return RST_LISTEN;
        }
        else {
            return RST_SEND;
        }
        break;

    case RST_CLEANUP:
        
        /* Close Sockets */
#ifndef PIC32_ENET_SK_DM320004_INTERNAL_ETHERNET
        close(in);
        close(out);
#endif
#ifdef PIC32_ENET_SK_DM320004_INTERNAL_ETHERNET
		closesocket(in);
		closesocket(out);
#endif
        return RST_SETUP;
        break;

    default:
        return RST_SETUP;
    }
}
