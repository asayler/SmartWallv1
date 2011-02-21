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

/* Outside Includes */
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

/* SmartWall Message Types */
#define SW_MSG_SET    0x01
#define SW_MSG_QUERY  0x02
#define SW_MSG_REPORT 0x03
#define SW_MSG_ERROR  0xff

/* SmartWall Message Scope */
#define SW_SCP_NETWORK 0x01
#define SW_SCP_DEVICE  0x02
#define SW_SCP_CHANNEL 0x03

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

typedef uint16_t totalLength_t;
#define PRItotLength  PRIu16
#define PRIxTotLength PRIx16
#define SCNtotLength  SCNu16
#define SCNxTotLength SCNx16

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

/* SmartWall Header Struct */
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
    totalLength_t totalLength; /* Total Length in bytes (Header + Data) */
    uint32_t unused0;          /* For Allignment, Future CRC32? */
};

#endif
