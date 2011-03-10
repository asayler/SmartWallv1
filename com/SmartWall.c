/* Andy Sayler
 * SmartWall Project
 * SmartWall Function Implementation
 * SmartWall.c
 * Created 2/21/11
 *
 * Change Log:
 * 02/21/11 - Created
 * ---
 */

#include "SmartWall.h"

#define ERROR_VAL -1

extern swLength_t writeSWMsg(uint8_t* msg,
                             const swLength_t maxLength,
                             const struct SmartWallDev* source,
                             const struct SmartWallDev* destination,
                             const devType_t targetType,
                             const msgScope_t msgScope,
                             const msgType_t msgType,
                             const swOpcode_t opcode,
                             const void* body,
                             const swLength_t bodyLength){

    /* Local Vars */
    swLength_t length = 0;
    swLength_t calcLength = 0;
    swLength_t tmpLength = 0;

    /* Local Structs */
    struct SmartWallHeader swHeader;

    /* Mem Init */
    memset(&swHeader, 0, sizeof(swHeader));

    /* Check Input */
    /* Check msg */
    if(msg == NULL){
        fprintf(stderr, "writeSWMsg: Input 'msg' must not be null!\n");
        return ERROR_VAL;
    }
    /* Check Body */
    if(bodyLength > 0){
        if(bodyLength > maxLength){
            fprintf(stderr, "writeSWMsg Error: "
                    "bodyLength greater than max!\n");
            return ERROR_VAL;
        }
        else {
            if(body == NULL){
                fprintf(stderr, "writeSWMsg: "
                        "Input 'body' must not be null!\n");
                return ERROR_VAL;
            }
        }
    }
    /* Check Source + Destination */
    if(source != NULL){
        if(destination != NULL){
            if(source->groupID != destination->groupID){
                fprintf(stderr, "writeSWMsg: groupID mismatch!\n");
                return ERROR_VAL;
            }
        }
        else {
            fprintf(stderr, "writeSWMsg: destination must not be null!\n");
            return ERROR_VAL;
        }
    }
    else {
        fprintf(stderr, "writeSWMsg: source must not be null!\n");
        return ERROR_VAL;
    }
        
    /* Calculate Length */
    calcLength = 0;
    calcLength += sizeof(swHeader);
    calcLength += bodyLength;
    
    /* Check Length Against Max */
    if(calcLength > maxLength){
        fprintf(stderr, "writeSWMsg Error: Length greater than max!\n");
        return ERROR_VAL;
    }

    /* Assemble SW Header */
    swHeader.version = SW_VERSION;
    swHeader.msgScope = msgScope;
    swHeader.msgType = msgType;
    swHeader.groupID = destination->groupID;
    swHeader.sourceAddress = source->address;
    swHeader.destAddress = destination->address;
    swHeader.sourceTypes = source->types;
    swHeader.targetType = targetType;
    swHeader.opcode = opcode;
    swHeader.totalLength = calcLength;

    /* Zero Length */
    length = 0;

    /* Copy SW Header to Msg */
    tmpLength = sizeof(swHeader);
    memcpy((msg + length), &swHeader, tmpLength);
    length += tmpLength;

    /* Copy SW Body to Msg */
    tmpLength = bodyLength;
    memcpy((msg + length), body, tmpLength);
    length += tmpLength;

    /* Final Length Check */
    if(calcLength != length){
        fprintf(stderr, "writeMsg Error: calcLength not equal to length !\n");
	fprintf(stderr, "calcLength: %" PRIswLength "\n", calcLength);
	fprintf(stderr, "length: %" PRIswLength "\n", length);
        return ERROR_VAL;
    }

    return length;
    
}

extern swLength_t writeSWChannelBody(uint8_t* msg,
                                     const swLength_t maxLength,
                                     const struct SWChannelData* data){
    
    /* Local Vars */
    swLength_t length = 0;
    swLength_t calcLength = 0;
    swLength_t tmpLength = 0;    
    int i;
    
    /* Check Input */
    /* Check Msg */
    if(msg == NULL){
        fprintf(stderr, "writeSWChannelBody: "
                "Input 'msg' must not be null!\n");
        return ERROR_VAL;
    }
    /* Check Data */
    if(data == NULL){
        fprintf(stderr, "writeSWChannelBody: "
                "data must not be null!\n");
        return ERROR_VAL;
    }
    
    /* Calculate Length */
    calcLength = 0;
    calcLength += sizeof(data->header);
    calcLength += (data->header).numChan * sizeof(data->data[0].chanTop);
    calcLength += (data->header).numChan * (data->header).dataLength;
    
    /* Check Length Against Max */
    if(calcLength > maxLength){
        fprintf(stderr, "writeSWChannelBody: "
                "Length greater than max!\n");
        return ERROR_VAL;
    }

    /* Zero Length */
    length = 0;

    /* Copy SW Channel Scope Header to Msg */
    tmpLength = sizeof(data->header);
    memcpy((msg + length), &(data->header), tmpLength);
    length += tmpLength;

    /* Check Data */
    if(data->data == NULL){
        fprintf(stderr,
                "writeSWChannelBody: data->data must not be NULL!\n");
        return ERROR_VAL;
    }

    /* Copy Data to Msg */
    for(i=0; i<((data->header).numChan); i++){
        /* Copy chanTop */
        tmpLength = sizeof(data->data[i].chanTop);
        memcpy((msg + length), &((data->data[i]).chanTop), tmpLength);
        length += tmpLength;
        /* Check value */
        if((data->data)[i].chanValue == NULL){
            fprintf(stderr,
                    "writeSWChannelBody: "
                    "chanValue must not be NULL!\n");
            return ERROR_VAL;
        }
        /* Copy chanValue */
        tmpLength = (data->header).dataLength;
        memcpy((msg + length), (data->data[i]).chanValue, tmpLength);
        length += tmpLength;
    }

    /* Final Length Check */
    if(calcLength != length){
        fprintf(stderr, "writeSWChannelBody: "
                "calcLength not equal to length !\n");
	fprintf(stderr, "calcLength: %" PRIswLength "\n", calcLength);
	fprintf(stderr, "length: %" PRIswLength "\n", length);
        return ERROR_VAL;
    }

    return length;
}

extern swLength_t readSWChannelMsg(const uint8_t* msg,
                                   const swLength_t msgLength,
                                   struct SmartWallDev* source,
                                   struct SmartWallDev* destination,
                                   devType_t* targetType,
                                   msgType_t* msgType,
                                   swOpcode_t* opcode,
                                   struct SWChannelData* data,
                                   const swLength_t maxNumChan,
                                   const swLength_t maxDataLength){
    /* Local Vars */
    swLength_t offset = 0;
    swLength_t calcLength = 0;
    swLength_t tmpLength = 0;
    int i = 0;

    /* Local Structs */
    struct SmartWallHeader swHeader;

    /* Mem Init */
    memset(&swHeader, 0, sizeof(swHeader));    

    /* Check Input */
    if(msg == NULL){
        fprintf(stderr, "readMsg Error: Input 'msg' must not be null!\n");
        return ERROR_VAL;
    }
    if(targetType == NULL){
        fprintf(stderr, "readMsg Error: 'targetType' must not be null!\n");
        return ERROR_VAL;
    }
    if(opcode == NULL){
        fprintf(stderr, "readMsg Error: 'opcode' must not be null!\n");
        return ERROR_VAL;
    }
    if(source == NULL){
        fprintf(stderr, "readMsg Error: source must not be null!\n");
        return ERROR_VAL;
    }
    if(destination == NULL){
        fprintf(stderr, "readMsg Error: destination must not be null!\n");
        return ERROR_VAL;
    }
    if(data == NULL){
        fprintf(stderr, "readMsg Error: data must not be null!\n");
        return ERROR_VAL;
    }

    /* Zero Offset */
    offset = 0;

    /* Extract SW Header */
    if((size_t)(msgLength - offset) < sizeof(swHeader)){
        fprintf(stderr, "readMsg Error: msg too small to contain swHeader!\n");
        return ERROR_VAL;
    }
    else{
        /* Copy SW Header from Msg */
        tmpLength = sizeof(swHeader);
        memcpy(&swHeader, (msg + offset), tmpLength);
        offset += tmpLength;
    }
    
    /* Calculate and Check Length */
    calcLength = swHeader.totalLength;
    if(msgLength != calcLength){
        fprintf(stderr, "readMsg Error: msg does not match declared size!\n");
        return ERROR_VAL;
    }

    /* Setup Output Device Structs and Data */
    source->version = swHeader.version;
    source->address = swHeader.sourceAddress;
    source->groupID = swHeader.groupID;
    source->types = swHeader.sourceTypes;
    destination->version = swHeader.version;
    destination->address = swHeader.destAddress;
    destination->groupID = swHeader.groupID;
    destination->types = 0;
    *targetType = swHeader.targetType;
    *msgType = swHeader.msgType;
    *opcode = swHeader.opcode;

    /* Extract Chan Header */
    if((size_t)(msgLength - offset) < sizeof(data->header)){
        fprintf(stderr, "readMsg Error: msg too small to contain chHeader!\n");
        return ERROR_VAL;
    }
    else{
        /* Copy Chan Header from Msg */
        tmpLength = sizeof(data->header);
        memcpy(&(data->header), (msg + offset), tmpLength);
        offset += tmpLength;
    }
    
    /* Check Limits */
    if((data->header).numChan > maxNumChan){
        fprintf(stderr, "readMsg Error: numChan exceeds max!\n");
        return ERROR_VAL;
    }
    if((data->header).dataLength > maxDataLength){
        fprintf(stderr, "readMsg Error: dataLength exceeds max!\n");
        return ERROR_VAL;
    }

    /* Check data Pointer */
    if(data->data == NULL){
        fprintf(stderr, "readMsg Error: data->data must not be null!\n");
        return ERROR_VAL;
    }

    /* Extract Chan Data */
    for(i = 0; i < (data->header).numChan; i++){
        /* Copy chanTop */
        tmpLength = sizeof(data->data->chanTop);
        memcpy(&((data->data)[i].chanTop), (msg + offset), tmpLength);
        offset += tmpLength;
        /* Check value Pointer */
        if((data->data)[i].chanValue == NULL){
            fprintf(stderr, "readMsg Error: chanValue must not be null!\n");
            return ERROR_VAL;
        }
        /* Copy chanValue */
        tmpLength = (data->header).dataLength;
        memcpy(((data->data)[i].chanValue), (msg + offset), tmpLength);
        offset += tmpLength;
    }

    return offset;
}
