/* Andy Sayler
 * SmartWall Project
 * SmartWall Structs and Defines
 * SmartWall.h
 * Created 2/2/11
 *
 * Change Log:
 * 02/02/11 - Created
 * 02/17/11 - Updated to v1.1 header structure
 * ---
 */

#ifndef SMARTWALL_H
#define SMARTWALL_H

/* Version Number */
#define SW_VERSION 0x01

/* Outside Includes */
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Constatns */
#define SW_MAX_MSG_LENGTH 1024 /* In Bytes */

/* SmartWall Message Types */
#define SW_MSG_SET     0x01
#define SW_MSG_REQUEST 0x02
#define SW_MSG_QUERY   0x03
#define SW_MSG_REPORT  0x04
#define SW_MSG_ERROR   0xff

/* SmartWall Message Scope */
#define SW_SCP_NETWORK 0x01
#define SW_SCP_DEVICE  0x02
#define SW_SCP_CHANNEL 0x03
#define SW_SCP_ERROR   0xff

/* SmartWall Device Type Bits */
#define SW_TYPE_MASTER    0x0000000000000001
#define SW_TYPE_OUTLET    0x0000000000000002
#define SW_TYPE_UNIVERSAL 0x8000000000000000

/* SmartWall Special Addresses */
#define SW_ADDR_NETWORK   0x0000
#define SW_ADDR_BROADCAST 0xffff

/* Data Types */
#define PRIipAddr  PRIu32
#define PRIxIPAddr PRIx32
#define SCNipAddr  SCNu32
#define SCNxIPAddr SCNx32

typedef uint8_t swVersion_t;
#define PRIswVer  PRIu8
#define PRIxSWVer PRIx8
#define SCNswVer  SCNu8
#define SCNxSWVer SCNx8

typedef uint8_t msgScope_t;
#define PRImsgScope  PRIu8
#define PRIxMsgScope PRIx8
#define SCNmsgScope  SCNu8
#define SCNxMsgScope SCNx8

typedef uint8_t msgType_t;
#define PRImsgType  PRIu8
#define PRIxMsgType PRIx8
#define SCNmsgType  SCNu8
#define SCNxMsgType SCNx8

typedef uint8_t groupID_t;
#define PRIgrpID  PRIu8
#define PRIxGrpID PRIx8
#define SCNgrpID  SCNu8
#define SCNxGrpID SCNx8

typedef uint16_t swAddress_t;
#define PRIswAddr  PRIu16
#define PRIxSWAddr PRIx16
#define SCNswAddr  SCNu16
#define SCNxSWAddr SCNx16

typedef uint64_t devType_t;
#define PRIdevType  PRIu64
#define PRIxDevType PRIx64
#define SCNdevType  SCNu64
#define SCNxDevType SCNx64

typedef uint16_t swOpcode_t;
#define PRIswOpcode  PRIu16
#define PRIxSWOpcode PRIx16
#define SCNswOpcode  SCNu16
#define SCNxSWOpcode SCNx16

typedef uint16_t swLength_t;
#define PRIswLength  PRIu16
#define PRIxSWLength PRIx16
#define SCNswLength  SCNu16
#define SCNxSWLength SCNx16

typedef uint8_t numChan_t;
#define PRInumChan  PRIu8
#define PRIxNumChan PRIx8
#define SCNnumChan  SCNu8
#define SCNxNumChan SCNx8

typedef uint64_t devUID_t;
#define PRIdevUID  PRIu64
#define PRIxDevUID PRIx64
#define SCNdevUID  SCNu64
#define SCNxDevUID SCNx64

/* SmartWall Header Struct  - 32 Bytes */
struct SmartWallHeader {
    swVersion_t version;       /* SmartWall Protocol version */
    msgScope_t msgScope;       /* SmartWall Mesage Scope */
    msgType_t msgType;         /* SmartWall Mesage Type */
    groupID_t groupID;         /* SmartWall groupID */
    swAddress_t sourceAddress; /* SmartWall Source Address */
    swAddress_t destAddress;   /* SmartWall Destination Address */
    devType_t sourceTypes;     /* Source Device Types Mask */
    devType_t targetType;      /* Destination Device Type */
    swOpcode_t opcode;         /* SW Opcode */
    swLength_t totalLength;    /* Total Length in bytes (Header + Data) */
    uint32_t unused0;          /* For Allignment, Future CRC32? */
};

/* SmartWall Device Scope Header */
struct SmartWallDeviceHeader {
    swLength_t dataLength; /* Operand Bytes */
    uint16_t unused0;      /* Alignment Padding */
    uint32_t unused1;      /* Alignment Padding */
};

/* SmartWall Channel Scope Header - 8 Bytes*/
struct SmartWallChannelHeader {
    numChan_t numChan;     /* Number of Channels Affected */
    uint8_t unused0;       /* Alignment Padding */
    swLength_t dataLength; /* Operand Bytes per channel */
    uint32_t unused1;      /* Alignment Padding */
};

/* SmartWall Channel Scope Channel Header - 8 Bytes*/
struct SmartWallChannelHead {
    numChan_t chanNum; /* Affected Channel */
    uint8_t unused0;   /* Alignment Padding */
    uint16_t unused1;  /* Alignment Padding */
    uint32_t unused2;  /* Alignment Padding */
};

/* SmartWall Channel Entry Struct */
struct SWChannelEntry {
    struct SmartWallChannelHead chanHead;
    void* chanValue;
};

/* SmartWall Channel Data Struct */
struct SWChannelData {
    struct SmartWallChannelHeader header;
    struct SWChannelEntry* data;
};

/* SmartWall Device Struct */
struct SmartWallDev {
    swVersion_t version;
    swAddress_t address;
    groupID_t groupID;
    devType_t types;
};

/* Public Functions */
extern swLength_t writeSWChannelMsg(char* msg, const swLength_t maxLength,
                                    const struct SmartWallDev* source,
                                    const struct SmartWallDev* destination,
                                    const devType_t targetType,
                                    const msgType_t msgType,
                                    const swOpcode_t opcode,
                                    const struct SWChannelData* data); 

extern swLength_t readSWChannelMsg(const char* msg, const swLength_t maxLength,
                                   struct SmartWallDev* source,
                                   struct SmartWallDev* destination,
                                   devType_t* targetType,
                                   swOpcode_t* opcode,
                                   struct SWChannelData* data); 


#endif
