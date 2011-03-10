/* Andy Sayler
 * SmartWall Project
 * SmartWall Function Tests
 * comTest.c
 * Created 2/27/11
 *
 * Change Log:
 * 02/27/11 - Created
 * ---
 */

#include "SmartWall.h"
#include "comTools.h"
#include "string.h"

#define COM_TEST_SWVERSION 0x01
#define COM_TEST_GID 0x01
#define COM_TEST_SADDR 0x01
#define COM_TEST_STYPES SW_TYPE_MASTER | SW_TYPE_UNIVERSAL
#define COM_TEST_DADDR 0x03
#define COM_TEST_DTYPES SW_TYPE_OUTLET | SW_TYPE_UNIVERSAL
#define COM_TEST_TRGTYPE SW_TYPE_OUTLET
#define COM_TEST_MSGTYPE SW_MSG_REQUEST
#define COM_TEST_OPCODE 0x01
#define COM_TEST_SCOPE SW_SCP_CHANNEL
#define COM_TEST_DATASIZE 8
#define COM_TEST_NUMCHN 3
#define COM_TEST_PAYLOAD0 "test000"
#define COM_TEST_PAYLOAD1 "test111"
#define COM_TEST_PAYLOAD2 "test222"

/* Local Functions */
static swLength_t genSWChannelRefMsg(uint8_t* msg, swLength_t maxLength);

/* Option enum */
enum OPTIONS{
    WRITE, READ, RW, REF, MEMSIZE
};

int main(int argc, char* argv[]){

    /* Local vars */
    int pcnt;
    enum OPTIONS mode;
    uint8_t msg[SW_MAX_MSG_LENGTH];
    uint8_t msgBody[SW_MAX_MSG_LENGTH - sizeof(struct SmartWallHeader)];
    uint8_t refmsg[SW_MAX_MSG_LENGTH];
    swLength_t msgSize = 0;
    swLength_t bodySize = 0;
    struct SmartWallDev source;
    struct SmartWallDev destination;
    struct SWChannelData data;
    struct SWChannelEntry dataEntries[COM_TEST_NUMCHN];
    char payload0[COM_TEST_DATASIZE] = COM_TEST_PAYLOAD0;
    char payload1[COM_TEST_DATASIZE] = COM_TEST_PAYLOAD1;
    char payload2[COM_TEST_DATASIZE] = COM_TEST_PAYLOAD2;
    devType_t targetType = COM_TEST_TRGTYPE;
    msgType_t msgType = COM_TEST_MSGTYPE;
    swOpcode_t opcode = COM_TEST_OPCODE;
    msgScope_t msgScope = COM_TEST_SCOPE;

    /* input options */
    if(argc < 2){
        mode = WRITE;
        (void) argv;
    }
    else{
        if(strcmp(argv[1], "-w") == 0){
            mode = WRITE;
        }
        else if(strcmp(argv[1], "-r") == 0){
            mode = READ;
        }
        else if(strcmp(argv[1], "-rw") == 0){
            mode = RW;
        }
        else if(strcmp(argv[1], "-ref") == 0){
            mode = REF;
        }
        else if(strcmp(argv[1], "-mem") == 0){
            mode = MEMSIZE;
        }
        else{
            fprintf(stderr, "Unknown option: %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }
    
    /* mode */
    if(mode == WRITE){
        /* Zero Structs */
        memset(&msg, 0, SW_MAX_MSG_LENGTH);
        memset(&refmsg, 0, SW_MAX_MSG_LENGTH);
        memset(&data, 0, sizeof(data));
        memset(&dataEntries[0], 0, sizeof(dataEntries[0]));
        memset(&dataEntries[1], 0, sizeof(dataEntries[1]));
        memset(&dataEntries[2], 0, sizeof(dataEntries[2]));
        
        /* Setup Structures */
        dataEntries[0].chanTop.chanNum = 0;
        dataEntries[0].chanValue = payload0;
        dataEntries[1].chanTop.chanNum = 1;
        dataEntries[1].chanValue = payload1;
        dataEntries[2].chanTop.chanNum = 2;
        dataEntries[2].chanValue = payload2;
        
        data.header.numChan = COM_TEST_NUMCHN;
        data.header.dataLength = COM_TEST_DATASIZE;
        data.data = dataEntries;
        
        source.version = COM_TEST_SWVERSION;
        source.address = COM_TEST_SADDR;
        source.groupID = COM_TEST_GID;
        source.types = COM_TEST_STYPES;
        
        destination.version = COM_TEST_SWVERSION;
        destination.address = COM_TEST_DADDR;
        destination.groupID = COM_TEST_GID;
        destination.types = COM_TEST_DTYPES;
        
        bodySize = writeSWChannelBody(msgBody, (SW_MAX_MSG_LENGTH -
                                             sizeof(struct SmartWallHeader)),
                                      &data);
        msgSize = writeSWMsg(msg, SW_MAX_MSG_LENGTH,
                             &source, &destination,
                             targetType, msgScope, msgType, opcode,
                             msgBody, bodySize);
     
        fprintf(stdout, "msgSize: %" PRIswLength "\n",
                msgSize);
        pcnt = print_payload(msg, msgSize);
        fprintf(stdout, "print_payload returned: %d\n", pcnt);
    }
    else if(mode == READ){
        /* Zero Structs */
        memset(&msg, 0, SW_MAX_MSG_LENGTH);
        memset(&refmsg, 0, SW_MAX_MSG_LENGTH);
        memset(&data, 0, sizeof(data));
        memset(&dataEntries[0], 0, sizeof(dataEntries[0]));
        memset(&dataEntries[1], 0, sizeof(dataEntries[1]));
        memset(&dataEntries[2], 0, sizeof(dataEntries[2]));
        memset(payload0, 0, COM_TEST_DATASIZE);
        memset(payload1, 0, COM_TEST_DATASIZE);
        memset(payload2, 0, COM_TEST_DATASIZE);
        targetType = 0;
        msgType = 0;
        opcode = 0;
        
        /* Setup Inner Structs */
        data.data = dataEntries;
        dataEntries[0].chanValue = payload0;
        dataEntries[1].chanValue = payload1;
        dataEntries[2].chanValue = payload2;
        
        /* Generate Ref Message */
        msgSize = genSWChannelRefMsg(refmsg, SW_MAX_MSG_LENGTH);
        
        /* Read Ref Message */
        msgSize = readSWChannelMsg(refmsg, msgSize,
                                   &source, &destination,
                                   &targetType, &msgType, &opcode,
                                   &data,
                                   COM_TEST_NUMCHN,
                                   COM_TEST_DATASIZE);
        
        /* Check Output */

        /* Print Message */
        fprintf(stdout, "-----Source  Device-----\n");
        print_swDev(&source);
        fprintf(stdout, "---Destination Device---\n");
        print_swDev(&destination);
        fprintf(stdout, "------Message Info------\n");
        fprintf(stdout, "targetType: 0x%4.4" PRIxDevType "\n", targetType);
        fprintf(stdout, "opcode:     0x%4.4" PRIxSWOpcode "\n", opcode);
        fprintf(stdout, "-------Body  Data-------\n");
        print_swChanMsgBody(&data, COM_TEST_NUMCHN, COM_TEST_DATASIZE);
    }
    else if(mode == RW){
        /* Zero Structs */
        memset(&msg, 0, SW_MAX_MSG_LENGTH);
        memset(&refmsg, 0, SW_MAX_MSG_LENGTH);
        memset(&data, 0, sizeof(data));
        memset(&dataEntries[0], 0, sizeof(dataEntries[0]));
        memset(&dataEntries[1], 0, sizeof(dataEntries[1]));
        memset(&dataEntries[2], 0, sizeof(dataEntries[2]));
        memset(payload0, 0, COM_TEST_DATASIZE);
        memset(payload1, 0, COM_TEST_DATASIZE);
        memset(payload2, 0, COM_TEST_DATASIZE);
        targetType = 0;
        msgType = 0;
        opcode = 0;
        
        /* Setup Inner Structs */
        data.data = dataEntries;
        dataEntries[0].chanValue = payload0;
        dataEntries[1].chanValue = payload1;
        dataEntries[2].chanValue = payload2;
        
        /* Generate Ref Message */
        msgSize = genSWChannelRefMsg(refmsg, SW_MAX_MSG_LENGTH);
        
        /* Read Ref Message */
        msgSize = readSWChannelMsg(refmsg, msgSize,
                                   &source, &destination,
                                   &targetType, &msgType, &opcode,
                                   &data,
                                   COM_TEST_NUMCHN,
                                   COM_TEST_DATASIZE);

        /* Write Message */     
        bodySize = writeSWChannelBody(msgBody, (SW_MAX_MSG_LENGTH -
                                             sizeof(struct SmartWallHeader)),
                                      &data);
        msgSize = writeSWMsg(msg, SW_MAX_MSG_LENGTH,
                             &source, &destination,
                             targetType, msgScope, msgType, opcode,
                             msgBody, bodySize);
     
        /* Read Written Message */
        msgSize = readSWChannelMsg(msg, msgSize,
                                   &source, &destination,
                                   &targetType, &msgType, &opcode,
                                   &data,
                                   COM_TEST_NUMCHN,
                                   COM_TEST_DATASIZE);


        /* Write Message */
        bodySize = writeSWChannelBody(msgBody, (SW_MAX_MSG_LENGTH -
                                             sizeof(struct SmartWallHeader)),
                                      &data);
        msgSize = writeSWMsg(msg, SW_MAX_MSG_LENGTH,
                             &source, &destination,
                             targetType, msgScope, msgType, opcode,
                             msgBody, bodySize);
     
        /* Print Output */
        fprintf(stdout, "msgSize: %" PRIswLength "\n",
                msgSize);
        pcnt = print_payload(msg, msgSize);
        fprintf(stdout, "print_payload returned: %d\n", pcnt);
    }
    else if(mode == REF){
        msgSize = genSWChannelRefMsg(refmsg, SW_MAX_MSG_LENGTH);
        fprintf(stdout, "msgSize: %" PRIswLength "\n",
                msgSize);
        pcnt = print_payload(refmsg, msgSize);
        fprintf(stdout, "print_payload returned: %d\n", pcnt);
    }
    else if(mode == MEMSIZE){

        /* System Size Test */
        uint64_t test64 = -1;
        uint32_t test32 = -1;
        fprintf(stdout, "uint64_t is %lu bytes\n", sizeof(test64));
        fprintf(stdout, "Max value of uint64_t is 0x%" PRIx64 "\n", test64);
        fprintf(stdout, "uint32_t is %lu bytes\n", sizeof(test32));
        fprintf(stdout, "Max value of uint32_t is 0x%" PRIx32 "\n", test32);

    }
   
    return EXIT_SUCCESS;
}

swLength_t genSWChannelRefMsg(uint8_t* refmsg, const swLength_t maxLength){
    
    /* Local Vars */
    swLength_t length = 0;
    swLength_t calcLength = 0;
    swLength_t tmpLength = 0;

    /* Check Input */
    if(refmsg == NULL){
        fprintf(stderr, "genMsg Error: Input 'refmsg' must not be null!\n");
        return EXIT_FAILURE;
    }

    /* Mem Init */
    memset(refmsg, 0, maxLength);

    /* Calculate Correct Length */
    calcLength = 0;
    calcLength += sizeof(struct SmartWallHeader);
    calcLength += sizeof(struct SmartWallChannelHeader);
    calcLength += COM_TEST_NUMCHN * sizeof(struct SmartWallChannelTop);
    calcLength += COM_TEST_NUMCHN * COM_TEST_DATASIZE;

    /* Check Length Against Max */
    if(calcLength > maxLength){
        fprintf(stderr, "genMsg Error: Length greater than max!\n");
        return EXIT_FAILURE;
    }
 
    /* Build ref message */
    /* SW Header */
    swVersion_t refSWver = COM_TEST_SWVERSION;
    tmpLength = sizeof(refSWver);
    memcpy((refmsg + length), &refSWver, tmpLength);
    length += tmpLength;
    msgScope_t refMsgScope = SW_SCP_CHANNEL;
    tmpLength = sizeof(refMsgScope);
    memcpy((refmsg + length), &refMsgScope, tmpLength);
    length += tmpLength;
    msgType_t refMsgType = COM_TEST_MSGTYPE;
    tmpLength = sizeof(refMsgType);
    memcpy((refmsg + length), &refMsgType, tmpLength);
    length += tmpLength;
    groupID_t refgid = COM_TEST_GID;
    tmpLength = sizeof(refgid);
    memcpy((refmsg + length), &refgid, tmpLength);
    length += tmpLength;
    swAddress_t refSAddr = COM_TEST_SADDR;
    tmpLength = sizeof(refSAddr);
    memcpy((refmsg + length), &refSAddr, tmpLength);
    length += tmpLength;
    swAddress_t refDAddr = COM_TEST_DADDR;
    tmpLength = sizeof(refDAddr);
    memcpy((refmsg + length), &refDAddr, tmpLength);
    length += tmpLength;
    devType_t refSTypes = COM_TEST_STYPES;
    tmpLength = sizeof(refSTypes);
    memcpy((refmsg + length), &refSTypes, tmpLength);
    length += tmpLength;
    devType_t refTType = COM_TEST_TRGTYPE;
    tmpLength = sizeof(refTType);
    memcpy((refmsg + length), &refTType, tmpLength);
    length += tmpLength;
    swOpcode_t refOpcode = COM_TEST_OPCODE;
    tmpLength = sizeof(refOpcode);
    memcpy((refmsg + length), &refOpcode, tmpLength);
    length += tmpLength;
    swLength_t refLength = calcLength;
    tmpLength = sizeof(refLength);
    memcpy((refmsg + length), &refLength, tmpLength);
    length += tmpLength;
    uint32_t unused0 = 0;
    tmpLength = sizeof(unused0);
    memcpy((refmsg + length), &unused0, tmpLength);
    length += tmpLength;
    /* Chan Scope Header */
    numChan_t refNumChn = COM_TEST_NUMCHN;
    tmpLength = sizeof(refNumChn);
    memcpy((refmsg + length), &refNumChn, tmpLength);
    length += tmpLength;
    uint8_t unused1 = 0;
    tmpLength = sizeof(unused1);
    memcpy((refmsg + length), &unused1, tmpLength);
    length += tmpLength;
    swLength_t refDataLen = COM_TEST_DATASIZE;
    tmpLength = sizeof(refDataLen);
    memcpy((refmsg + length), &refDataLen, tmpLength);
    length += tmpLength;
    uint32_t unused2 = 0;
    tmpLength = sizeof(unused2);
    memcpy((refmsg + length), &unused2, tmpLength);
    length += tmpLength;
    /* Chan 0 Head */
    numChan_t refChnNum0 = 0;
    tmpLength = sizeof(refChnNum0);
    memcpy((refmsg + length), &refChnNum0, tmpLength);
    length += tmpLength;
    uint8_t unused3 = 0;
    tmpLength = sizeof(unused3);
    memcpy((refmsg + length), &unused3, tmpLength);
    length += tmpLength;
    uint16_t unused4 = 0;
    tmpLength = sizeof(unused4);
    memcpy((refmsg + length), &unused4, tmpLength);
    length += tmpLength;
    uint32_t unused5 = 0;
    tmpLength = sizeof(unused5);
    memcpy((refmsg + length), &unused5, tmpLength);
    length += tmpLength;
    /* Chan 0 Data */
    char* payload0 = COM_TEST_PAYLOAD0;
    tmpLength = COM_TEST_DATASIZE;
    memcpy((refmsg + length), payload0, tmpLength);
    length += tmpLength;
    /* Chan 1 Head */
    numChan_t refChnNum1 = 1;
    tmpLength = sizeof(refChnNum1);
    memcpy((refmsg + length), &refChnNum1, tmpLength);
    length += tmpLength;
    uint8_t unused6 = 0;
    tmpLength = sizeof(unused6);
    memcpy((refmsg + length), &unused6, tmpLength);
    length += tmpLength;
    uint16_t unused7 = 0;
    tmpLength = sizeof(unused7);
    memcpy((refmsg + length), &unused7, tmpLength);
    length += tmpLength;
    uint32_t unused8 = 0;
    tmpLength = sizeof(unused8);
    memcpy((refmsg + length), &unused8, tmpLength);
    length += tmpLength;
    /* Chan 1 Data */
    char* payload1 = COM_TEST_PAYLOAD1;
    tmpLength = COM_TEST_DATASIZE;
    memcpy((refmsg + length), payload1, tmpLength);
    length += tmpLength;
    /* Chan 2 Head */
    numChan_t refChnNum2 = 2;
    tmpLength = sizeof(refChnNum2);
    memcpy((refmsg + length), &refChnNum2, tmpLength);
    length += tmpLength;
    uint8_t unused9 = 0;
    tmpLength = sizeof(unused9);
    memcpy((refmsg + length), &unused9, tmpLength);
    length += tmpLength;
    uint16_t unused10 = 0;
    tmpLength = sizeof(unused10);
    memcpy((refmsg + length), &unused10, tmpLength);
    length += tmpLength;
    uint32_t unused11 = 0;
    tmpLength = sizeof(unused11);
    memcpy((refmsg + length), &unused11, tmpLength);
    length += tmpLength;
    /* Chan 1 Data */
    char* payload2 = COM_TEST_PAYLOAD2;
    tmpLength = COM_TEST_DATASIZE;
    memcpy((refmsg + length), payload2, tmpLength);
    length += tmpLength;

    return length;
}
