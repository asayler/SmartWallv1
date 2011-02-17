/* Andy Sayler
 * SmartWall Project
 * SmartWall Structs and Defines
 * SmartWall.h
 * Created 2/2/11
 *
 * Change Log:
 * 02/02/11 - Created
 * ---
 */

#ifndef SMARTWALL_H
#define SMARTWALL_H

/* Outside Includes */
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

/* SmartWall Message Types */
#define SW_MSG_SYSTEM 0x0
#define SW_MSG_ACTION 0x1
#define SW_MSG_QUERY  0x2

/* SmartWall Device Type Bits */
#define SW_TYPE_MASTER    0x0000000000000001
#define SW_TYPE_OUTLET    0x0000000000000010
#define SW_TYPE_UNIVERSAL 0x1000000000000000

/* Data Types */
#define PRIipAddr PRIu32
#define PRIxIPAddr PRIx32
#define SCNipAddr SCNu32
#define SCNxIPAddr SCNx32

typedef uint8_t swVersion_t;
#define PRIswVer PRIu8
#define PRIxSWVer PRIx8
#define SCNswVer SCNu8
#define SCNxSWVer SCNx8

typedef uint8_t msgType_t;
#define PRImsgType PRIu8
#define PRIxMsgType PRIx8
#define SCNmsgType SCNu8
#define SCNxMsgType SCNx8

typedef uint8_t groupID_t;
#define PRIgrpID PRIu8
#define PRIxGrpID PRIx8
#define SCNgrpID SCNu8
#define SCNxGrpID SCNx8

typedef uint16_t swAddress_t;
#define PRIswAddr PRIu16
#define PRIxSWAddr PRIx16
#define SCNswAddr SCNu16
#define SCNxSWAddr SCNx16

typedef uint64_t devType_t;
#define PRIdevType PRIu64
#define PRIxDevType PRIx64
#define SCNdevType SCNu64
#define SCNxDevType SCNx64

typedef uint8_t numChan_t;
#define PRInumChan PRIu8
#define PRIxNumChan PRIx8
#define SCNnumChan SCNu8
#define SCNxNumChan SCNx8

typedef uint64_t devUID_t;
#define PRIdevUID PRIu64
#define PRIxDevUID PRIx64
#define SCNdevUID SCNu64
#define SCNxDevUID SCNx64

/* SmartWall Header Struct */
struct SmartWallHeader {
    swVersion_t version; /* SmartWall Protocol version */
    msgType_t msgType; /* SmartWall Mesage Type */
    uint8_t unused; /* For allignment */
    groupID_t groupID; /* SmartWall groupID */
    swAddress_t sourceAddress; /* SmartWall Source Address */
    swAddress_t destAddress; /* SmartWall Destination Address */
    devType_t sourceType; /* Source Device Type Mask */
    devType_t destType; /* Destination Device Type Mask */
};

#endif
