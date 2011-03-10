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

extern swLength_t writeSWChannelMsg(uint8_t* msg, const swLength_t maxLength,
                                    const struct SmartWallDev* source,
                                    const struct SmartWallDev* destination,
                                    const devType_t targetType,
                                    const msgType_t msgType,
                                    const swOpcode_t opcode,
                                    const struct SWChannelData* data){
    
    /* Local Vars */
    swLength_t length = 0;
    swLength_t calcLength = 0;
    int i;
    swLength_t tmpLength = 0;

    /* Local Structs */
    struct SmartWallHeader swHeader;

    /* Mem Init */
    memset(&swHeader, 0, sizeof(swHeader));

    /* Check Input */
    if(msg == NULL){
        fprintf(stderr, "writeMsg Error: Input 'msg' must not be null!\n");
        return ERROR_VAL;
    }

    /* Check Input */
    /* Check Source + Destination */
    if(source != NULL){
        if(destination != NULL){
            if(source->groupID == destination->groupID){
                /* Good Input, Normal Behavior, Continue */
            }
            else {
                fprintf(stderr, "writeMsg Error: groupID mismatch!\n");
                return ERROR_VAL;
            }
        }
        else {
            fprintf(stderr, "writeMsg Error: destination must not be null!\n");
            return ERROR_VAL;
        }
    }
    else {
        fprintf(stderr, "writeMsg Error: source must not be null!\n");
        return ERROR_VAL;
    }
    
    /* Check Data */
    if(data != NULL){
        /* Good Input, Normal Behavior, Continue */
    }
    else {
        fprintf(stderr, "writeMsg Error: data must not be null!\n");
        return ERROR_VAL;
    }
    
    /* Calculate Length */
    calcLength = 0;
    calcLength += sizeof(swHeader);
    calcLength += sizeof(data->header);
    calcLength += (data->header).numChan * sizeof(data->data[0].chanTop);
    calcLength += (data->header).numChan * (data->header).dataLength;
    
    /* Check Length Against Max */
    if(calcLength > maxLength){
        fprintf(stderr, "writeMsg Error: Length greater than max!\n");
        return ERROR_VAL;
    }

    /* Assemble SW Header */
    swHeader.version = SW_VERSION;
    swHeader.msgScope = SW_SCP_CHANNEL;
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

    /* Copy SW Channel Scope Header to Msg */
    tmpLength = sizeof(data->header);
    memcpy((msg + length), &(data->header), tmpLength);
    length += tmpLength;

    /* Copy Data to Msg */
    for(i=0; i<((data->header).numChan); i++){
        tmpLength = sizeof(data->data[i].chanTop);
        memcpy((msg + length), &((data->data[i]).chanTop), tmpLength);
        length += tmpLength;
        tmpLength = (data->header).dataLength;
        memcpy((msg + length), (data->data[i]).chanValue, tmpLength);
        length += tmpLength;
    }

    /* Final Length Check */
    if(calcLength != length){
        fprintf(stderr, "writeMsg Error: calcLength not equal to length !\n");
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
