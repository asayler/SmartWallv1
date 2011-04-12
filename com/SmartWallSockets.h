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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

/* Local Includes */
#include "SmartWall.h"
#include "../slave/swUniversal.h"

struct SWDeviceInfo {
    struct SmartWallDev devInfo; /* SW Device Info */
    struct sockaddr_in devIP;
};

/* Functions */
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

enum handelerState{HANDELER_ERROR = -1, HANDELER_SUCCESS = 0};
typedef enum handelerState (*swHandeler) (const swOpcode_t opcode,
                                          const msgType_t msgType,
                                          const void* input,
                                          void* output,
                                          void* deviceState,
                                          msgScope_t* errorScope,
                                          swOpcode_t* errorOpcode);
typedef enum handelerState (*devHandeler) (const swOpcode_t devOpcode,
                                           const msgType_t msgType,
                                           const void* input,
                                           void* output,
                                           void* deviceState,
                                           msgScope_t* errorScope,
                                           swOpcode_t* errorOpcode);

typedef enum handelerState (*chnHandeler) (const swOpcode_t chnOpcode,
                                           const msgType_t msgType,
                                           const struct SWChannelData* input,
                                           struct SWChannelData* output,
                                           void* deviceState,
                                           msgScope_t* errorScope,
                                           swOpcode_t* errorOpcode);

struct SWProcessor{
    msgScope_t processorScope;
    void* data;
    void* dataLimits;
    readSWBody decoder;
    swHandeler handeler;
    writeSWBody encoder;
};

enum handelerState swProcess(const msgScope_t msgScope,
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
                             swOpcode_t* errorOpcode);

#endif
