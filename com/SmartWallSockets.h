/* Andy Sayler
 * SmartWall Project
 * SmartWall Berkeley Socktes Interface Structs and Defines
 * SmartWallSockets.h
 * Created 3/11/11
 *
 * Change Log:
 * 03/11/11 - Created
 * ---
 */

#ifndef SMARTWALLSOCKETS_H
#define SMARTWALLSOCKETS_H

/* Outside Includes */
#ifndef PIC32_ENET_SK_DM320004_INTERNAL_ETHERNET
#include  <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#ifdef PIC32_ENET_SK_DM320004_INTERNAL_ETHERNET
#include "TCPIP Stack/TCPIP.h"
#endif

/* Local Includes */
#include "SmartWall.h"
#include "../slave/swUniversal.h"

/* Type Defs */
enum processorState{PROCESSOR_ERROR = -1, PROCESSOR_SUCCESS = 0};
typedef enum processorState (*swDevHandeler) (const swOpcode_t opcode,
                                              const msgType_t msgType,
                                              const void* input,
                                              void* output,
                                              void* deviceState,
                                              msgScope_t* errorScope,
                                              swOpcode_t* errorOpcode);

typedef enum processorState (*swMrHandeler)(const swOpcode_t chnOpcode,
                                            const msgType_t msgType,
                                            const struct SWChannelData* input);

/* Structs */
struct SWDeviceInfo {
    struct SmartWallDev devInfo; /* SW Device Info */
    struct sockaddr_in devIP;
};

struct SWDevProcessor{
    msgScope_t processorScope;
    void* data;
    void* dataLimits;
    readSWBody decoder;
    swDevHandeler handeler;
    writeSWBody encoder;
};

/* Functions */

enum swSetup{SWSETUP_ERROR = -1, SWSETUP_SUCCESS = 0};
enum swSetup swSetup(int* inSocket, int* outSocket,
                     const struct SWDeviceInfo* myDev);

/* Return Value: msgSize in bytes */
int swListen(int inSocket, uint8_t* buffer, swLength_t bufferSize,
             struct SWDeviceInfo* sourceDev);

/* Return Value: bytes sent */
int swSend(const int outSocket,
           const uint8_t* msg, const swLength_t msgSize,
           const struct SWDeviceInfo* destDev);

enum swReceiveState{SWRECEIVE_ERROR = -1, SWRECEIVE_SUCCESS = 0};
enum swReceiveState swReceive(const uint8_t* msg, const swLength_t msgLen,
                              struct SWDeviceInfo* sourceDev,
                              struct SWDeviceInfo* msgDev,
                              devType_t* targetType,
                              msgScope_t* msgScope,
                              msgType_t* msgType,
                              swOpcode_t* opcode,
                              void* body,
                              swLength_t* bodyLength,
                              const swLength_t maxBodyLength);

/* Return value: bytes written to buffer */
int swCompose(uint8_t* buffer, const swLength_t bufferSize,
              const struct SWDeviceInfo* myDev,
              const struct SWDeviceInfo* destDev,
              const devType_t targetType,
              const msgScope_t msgScope,
              const msgType_t msgType,
              const swOpcode_t opcode,
              const void* body,
              const swLength_t bodyLength);

enum swCheckState{SWCHECK_ERROR = -1, SWCHECK_FOR_ME = 0,
                  SWCHECK_NOT_FOR_ME = 1};
enum swCheckState swCheck(const struct SWDeviceInfo* myDev,
                          const struct SWDeviceInfo* msgDev,
                          const devType_t targetType,
                          msgScope_t* errorScope,
                          swOpcode_t* errorOpcode);

enum processorState swDevProcess(const msgScope_t msgScope,
                                 const msgType_t msgType,
                                 const swOpcode_t opcode,
                                 struct SWDevProcessor* processors,
                                 const int numProcessors,
                                 const void* inputBody,
                                 const swLength_t inBodyLength,
                                 void* outputBody,
                                 swLength_t* outBodyLength,
                                 const swLength_t maxBodyLength,
                                 void* deviceState,
                                 msgScope_t* errorScope,
                                 swOpcode_t* errorOpcode);

/* TODO: Make Thread Safe */
enum SWReceiverState{RST_SETUP, RST_LISTEN, RST_RECEIVE, RST_CHECK,
                     RST_PROCESS, RST_COMPOSE, RST_SEND, RST_CLEANUP};
enum SWReceiverState swReceiverStateMachine(enum SWReceiverState machineState,
                                            const struct SWDeviceInfo* myDev,
                                            const struct SWDeviceInfo* tgtDev,
                                            void* deviceState,
                                            struct SWDevProcessor* processors,
                                            const int numProcessors);


#endif
