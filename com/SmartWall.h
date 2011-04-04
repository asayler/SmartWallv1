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

/* Local Includes */
#include "comTools.h"

/* Constants */
#define SW_MAX_MSG_LENGTH 1024 /* In Bytes */
#define SW_MAX_BODY_LENGTH (1024 - sizeof(struct SmartWallHeader))
#define SW_MAX_CHN 60 /* Max number of channels on a single device */
#define SWOUTPORT 4333 /* 4329 to 4339 Free as of 2/1/2011 */
#define SWINPORT 4334 /* 4329 to 4339 Free as of 2/1/2011 */
#define SWCFGPORT 4339 /* 4329 to 4339 Free as of 2/1/2011 */

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
#define ntohswVersion(x) (x)
#define htonswVersion(x) (x)

typedef uint8_t msgScope_t;
#define PRImsgScope  PRIu8
#define PRIxMsgScope PRIx8
#define SCNmsgScope  SCNu8
#define SCNxMsgScope SCNx8
#define ntohmsgScope(x) (x)
#define htonmsgScope(x) (x)

typedef uint8_t msgType_t;
#define PRImsgType  PRIu8
#define PRIxMsgType PRIx8
#define SCNmsgType  SCNu8
#define SCNxMsgType SCNx8
#define ntohmsgType(x) (x)
#define htonmsgType(x) (x)

typedef uint8_t groupID_t;
#define PRIgrpID  PRIu8
#define PRIxGrpID PRIx8
#define SCNgrpID  SCNu8
#define SCNxGrpID SCNx8
#define ntohgroupID(x) (x)
#define htongroupID(x) (x)

typedef uint16_t swAddress_t;
#define PRIswAddr  PRIu16
#define PRIxSWAddr PRIx16
#define SCNswAddr  SCNu16
#define SCNxSWAddr SCNx16
#define SWADDRESS_MAX 65535
#define ntohswAddress(x) ntoh16(x)
#define htonswAddress(x) ntoh16(x)

typedef uint64_t devType_t;
#define PRIdevType  PRIu64
#define PRIxDevType PRIx64
#define SCNdevType  SCNu64
#define SCNxDevType SCNx64
#define ntohdevType(x) ntoh64(x)
#define htondevType(x) ntoh64(x)

typedef uint16_t swOpcode_t;
#define PRIswOpcode  PRIu16
#define PRIxSWOpcode PRIx16
#define SCNswOpcode  SCNu16
#define SCNxSWOpcode SCNx16
#define SWOPCODE_MAX 65535
#define ntohswOpcode(x) ntoh16(x)
#define htonswOpcode(x) ntoh16(x)

typedef uint16_t swLength_t;
#define PRIswLength  PRIu16
#define PRIxSWLength PRIx16
#define SCNswLength  SCNu16
#define SCNxSWLength SCNx16
#define SWLENGTH_MAX UINT16_MAX
#define ntohswLength(x) ntoh16(x)
#define htonswLength(x) ntoh16(x)

typedef uint8_t numChan_t;
#define PRInumChan  PRIu8
#define PRIxNumChan PRIx8
#define SCNnumChan  SCNu8
#define SCNxNumChan SCNx8
#define ntohnumChan(x) (x)
#define htonnumChan(x) (x)

typedef uint64_t devUID_t;
#define PRIdevUID  PRIu64
#define PRIxDevUID PRIx64
#define SCNdevUID  SCNu64
#define SCNxDevUID SCNx64
#define ntohdevUID(x) ntoh64(x)
#define htondevUID(x) ntoh64(x)

/* SmartWall Reserved Addresses */
#define SW_ADDR_BROADCAST 0xffff
#define SW_ADDR_NETWORK   0x0000

/* SmartWall Message Types */
#define SW_MSG_SET     0x01
#define SW_MSG_REQUEST 0x02
#define SW_MSG_QUERY   0x03
#define SW_MSG_REPORT  0x04
#define SW_MSG_ERROR   0xff
extern int strToMT(const char* typeStr, const size_t maxLength,
                   msgType_t* type);
extern int MTtoStr(char* typeStr, const size_t maxLength,
                   const msgType_t* type);

/* SmartWall Message Scope */
#define SW_SCP_NETWORK 0x01
#define SW_SCP_DEVICE  0x02
#define SW_SCP_CHANNEL 0x03
#define SW_SCP_ERROR   0xff

/* SmartWall Device Type Bits */
#define SW_TYPE_MASTER    0x0000000000000001
#define SW_TYPE_OUTLET    0x0000000000000004
#define SW_TYPE_UNIVERSAL 0x8000000000000000
extern int strToDT(const char* typeStr, const size_t maxLength,
                   devType_t* type);
extern int DTtoStr(char* typeStr, const size_t maxLength,
                   const devType_t* type);

/* SmartWall Header Struct  - 32 Bytes */
struct SmartWallHeader {
    swVersion_t version;       /* SmartWall Protocol version */
    msgScope_t msgScope;       /* SmartWall Message Scope */
    msgType_t msgType;         /* SmartWall Message Type */
    groupID_t groupID;         /* SmartWall groupID */
    swAddress_t sourceAddress; /* SmartWall Source Address */
    swAddress_t destAddress;   /* SmartWall Destination Address */
    devType_t sourceTypes;     /* Source Device Types Mask */
    devType_t targetType;      /* Destination Device Type */
    swOpcode_t opcode;         /* SW Opcode */
    swLength_t totalLength;    /* Total Length in bytes (Header + Data) */
    uint32_t unused0;          /* For Allignment, Future CRC32? */
};
extern int ntohSmartWallHeader(struct SmartWallHeader* header);
extern int htonSmartWallHeader(struct SmartWallHeader* header);

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
extern int ntohSmartWallChannelHeader(struct SmartWallChannelHeader* header);
extern int htonSmartWallChannelHeader(struct SmartWallChannelHeader* header);

/* SmartWall Channel Scope Channel Header - 8 Bytes*/
struct SmartWallChannelTop {
    numChan_t chanNum; /* Affected Channel */
    uint8_t unused0;   /* Alignment Padding */
    uint16_t unused1;  /* Alignment Padding */
    uint32_t unused2;  /* Alignment Padding */
};
extern int ntohSmartWallChannelTop(struct SmartWallChannelTop* top);
extern int htonSmartWallChannelTop(struct SmartWallChannelTop* top);

/* SmartWall Channel Entry Struct */
struct SWChannelEntry {
    struct SmartWallChannelTop chanTop;
    void* chanValue;
};

/* SmartWall Channel Data Struct */
struct SWChannelData {
    struct SmartWallChannelHeader header;
    struct SWChannelEntry* data;
};

/* SmartWall Device Struct */
struct SmartWallDev {
    swAddress_t swAddr;  /* SW Address */
    devType_t devTypes;  /* SW Device Types Mask */
    numChan_t numChan;   /* Number of Channels on Device */
    swVersion_t version; /* SW Protocol Version */
    devUID_t uid;        /* SW Unique Device ID */
    groupID_t groupID;   /* SW Group ID */
};

/* Public Functions */
/* Function to compose SW Message */
/* Returns: Message length on success (bytes), SWLENGTH_MAX on failure */
extern swLength_t writeSWMsg(uint8_t* msg,
                             const swLength_t maxLength,
                             const struct SmartWallDev* source,
                             const struct SmartWallDev* destination,
                             const devType_t targetType,
                             const msgScope_t msgScope,
                             const msgType_t msgType,
                             const swOpcode_t opcode,
                             const void* body,
                             const swLength_t bodyLength);

/* Function to read SW Message */
/* Returns: Message length on success (bytes), SWLENGTH_MAX on failure */
extern swLength_t readSWMsg(const uint8_t* msg,
                            const swLength_t msgLength,
                            struct SmartWallDev* source,
                            struct SmartWallDev* destination,
                            devType_t* targetType,
                            msgScope_t* msgScope,
                            msgType_t* msgType,
                            swOpcode_t* opcode,
                            void* body,
                            swLength_t* bodyLength,
                            const swLength_t maxBodyLength);

/* Function to compose SW Channel Body */
/* Returns: Body length on success (bytes), SWLENGTH_MAX on failure */
extern swLength_t writeSWChannelBody(uint8_t* msgBody,
                                     const swLength_t maxLength,
                                     const struct SWChannelData* data); 

/* Function to read SW Channel Body */
/* Returns: Body length on success (bytes), SWLENGTH_MAX on failure */
extern swLength_t readSWChannelBody(const uint8_t* msgBody,
                                    const swLength_t bodyLength,
                                    struct SWChannelData* data,
                                    const swLength_t maxNumChan,
                                    const swLength_t maxDataLength);

#endif
