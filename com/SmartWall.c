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
#define SUCCESS_VAL 0

extern int strToMT(const char* typeStr, const size_t maxLength,
                   msgType_t* type){

    /* check input */
    if(typeStr == NULL){
        fprintf(stderr, "strToMT: 'typeStr' must not be NULL.\n");
        return ERROR_VAL;
    }
    if(maxLength <= 0){
        fprintf(stderr, "strToMT: 'maxLength' must be > 0.\n");
        return ERROR_VAL;
    }
    if(type == NULL){
        fprintf(stderr, "strToMT: 'type' must not be NULL.\n");
        return ERROR_VAL;
    }

    /* convert */
    if(strncmp(typeStr, "SET", maxLength) == 0){
        *type = SW_MSG_SET;
        return SUCCESS_VAL;
    }
    else if(strncmp(typeStr, "REQUEST", maxLength) == 0){
        *type = SW_MSG_REQUEST;
        return SUCCESS_VAL;
    }
    else if(strncmp(typeStr, "QUERY", maxLength) == 0){
        *type = SW_MSG_QUERY;
        return SUCCESS_VAL;
    }
    else if(strncmp(typeStr, "REPORT", maxLength) == 0){
        *type = SW_MSG_REPORT;
        return SUCCESS_VAL;
    }
    else if(strncmp(typeStr, "ERROR", maxLength) == 0){
        *type = SW_MSG_ERROR;
        return SUCCESS_VAL;
    }
    else{
        fprintf(stderr, "strToMT: Unrecognized Message Type: %s\n", typeStr);
        return ERROR_VAL;
    }

    return SUCCESS_VAL;
}

extern int MTtoStr(char* typeStr, const size_t maxLength,
                   const msgType_t* type){

    /* check input */
    if(typeStr == NULL){
        fprintf(stderr, "MTtoStr: 'typeStr' must not be NULL.\n");
        return ERROR_VAL;
    }
    if(maxLength <= 0){
        fprintf(stderr, "MTtoStr: 'maxLength' must be > 0.\n");
        return ERROR_VAL;
    }
    if(type == NULL){
        fprintf(stderr, "MTtoStr: 'type' must not be NULL.\n");
        return ERROR_VAL;
    }

    /* convert */
    if(*type == SW_MSG_SET){
        strncpy(typeStr, "SET", maxLength);
        typeStr[maxLength-1] = '\0';
        return SUCCESS_VAL;
    }
    else if(*type == SW_MSG_REQUEST){
        strncpy(typeStr, "REQUEST", maxLength);
        typeStr[maxLength-1] = '\0';
        return SUCCESS_VAL;
    }
    else if(*type == SW_MSG_QUERY){
        strncpy(typeStr, "QUERY", maxLength);
        typeStr[maxLength-1] = '\0';
        return SUCCESS_VAL;
    }
    else if(*type == SW_MSG_REPORT){
        strncpy(typeStr, "REPORT", maxLength);
        typeStr[maxLength-1] = '\0';
        return SUCCESS_VAL;
    }
    else if(*type == SW_MSG_ERROR){
        strncpy(typeStr, "ERROR", maxLength);
        typeStr[maxLength-1] = '\0';
        return SUCCESS_VAL;
    }
    else{
        fprintf(stderr, "MTtoStr: Unrecognized Message Type: 0x%"
                PRIxMsgType "\n", *type);
        return ERROR_VAL;
    }

    return SUCCESS_VAL;
}

extern int strToDT(const char* typeStr, const size_t maxLength,
                   devType_t* type){
    
    /* check input */
    if(typeStr == NULL){
        fprintf(stderr, "strToDT: 'typeStr' must not be NULL.\n");
        return ERROR_VAL;
    }
    if(maxLength <= 0){
        fprintf(stderr, "strToDT: 'maxLength' must be > 0.\n");
        return ERROR_VAL;
    }
    if(type == NULL){
        fprintf(stderr, "strToDT: 'type' must not be NULL.\n");
        return ERROR_VAL;
    }

    /* convert */
    if(strncmp(typeStr, "MASTER", maxLength) == 0){
        *type = SW_TYPE_MASTER;
        return SUCCESS_VAL;
    }
    else if(strncmp(typeStr, "OUTLET", maxLength) == 0){
        *type = SW_TYPE_OUTLET;
        return SUCCESS_VAL;
    }
    else if(strncmp(typeStr, "UNIVERSAL", maxLength) == 0){
        *type = SW_TYPE_UNIVERSAL;
        return SUCCESS_VAL;
    }
    else{
        fprintf(stderr, "strToDT: Unrecognized Device Type: %s\n", typeStr);
        return ERROR_VAL;
    }

    return SUCCESS_VAL;
}

extern int DTtoStr(char* typeStr, const size_t maxLength,
                   const devType_t* type){

    /* check input */
    if(typeStr == NULL){
        fprintf(stderr, "DTtoStr: 'typeStr' must not be NULL.\n");
        return ERROR_VAL;
    }
    if(maxLength <= 0){
        fprintf(stderr, "DTtoStr: 'maxLength' must be > 0.\n");
        return ERROR_VAL;
    }
    if(type == NULL){
        fprintf(stderr, "DTtoStr: 'type' must not be NULL.\n");
        return ERROR_VAL;
    }

    /* convert */
    if(*type == SW_TYPE_MASTER){
        strncpy(typeStr, "MASTER", maxLength);
        typeStr[maxLength-1] = '\0';
        return SUCCESS_VAL;
    }
    else if(*type == SW_TYPE_OUTLET){
        strncpy(typeStr, "OUTLET", maxLength);
        typeStr[maxLength-1] = '\0';
        return SUCCESS_VAL;
    }
    else if(*type == SW_TYPE_UNIVERSAL){
        strncpy(typeStr, "UNIVERSAL", maxLength);
        typeStr[maxLength-1] = '\0';
        return SUCCESS_VAL;
    }
    else{
        fprintf(stderr, "DTtoStr: Unrecognized Device Type: 0x%16.16"
                PRIxDevType "\n", *type);
        return ERROR_VAL;
    }

    return SUCCESS_VAL;
}

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
        return SWLENGTH_MAX;
    }
    /* Check Body */
    if(bodyLength > 0){
        if(bodyLength > maxLength - sizeof(struct SmartWallHeader)){
            fprintf(stderr, "writeSWMsg Error: "
                    "bodyLength greater than max!\n");
            return SWLENGTH_MAX;
        }
        else {
            if(body == NULL){
                fprintf(stderr, "writeSWMsg: "
                        "Input 'body' must not be null!\n");
                return SWLENGTH_MAX;
            }
        }
    }
    /* Check Source + Destination */
    if(source != NULL){
        if(destination != NULL){
            if(source->groupID != destination->groupID){
                fprintf(stderr, "writeSWMsg: groupID mismatch!\n");
                return SWLENGTH_MAX;
            }
        }
        else {
            fprintf(stderr, "writeSWMsg: destination must not be null!\n");
            return SWLENGTH_MAX;
        }
    }
    else {
        fprintf(stderr, "writeSWMsg: source must not be null!\n");
        return SWLENGTH_MAX;
    }
        
    /* Calculate Length */
    calcLength = 0;
    calcLength += sizeof(swHeader);
    calcLength += bodyLength;
    
    /* Check Length Against Max */
    if(calcLength > maxLength){
        fprintf(stderr, "writeSWMsg Error: Length greater than max!\n");
        return SWLENGTH_MAX;
    }

    /* Assemble SW Header */
    swHeader.version = SW_VERSION;
    swHeader.msgScope = msgScope;
    swHeader.msgType = msgType;
    swHeader.groupID = destination->groupID;
    swHeader.sourceAddress = source->swAddr;
    swHeader.destAddress = destination->swAddr;
    swHeader.sourceTypes = source->devTypes;
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
        return SWLENGTH_MAX;
    }

    return length;
    
}

extern swLength_t writeSWChannelBody(uint8_t* msgBody,
                                     const swLength_t maxLength,
                                     const struct SWChannelData* data){
    
    /* Local Vars */
    swLength_t length = 0;
    swLength_t calcLength = 0;
    swLength_t tmpLength = 0;    
    int i;
    
    /* Check Input */
    /* Check Msg*/
    if(msgBody == NULL){
        fprintf(stderr, "writeSWChannelBody: "
                "Input 'msgBody' must not be null!\n");
        return SWLENGTH_MAX;
    }
    /* Check Data */
    if(data == NULL){
        fprintf(stderr, "writeSWChannelBody: "
                "data must not be null!\n");
        return SWLENGTH_MAX;
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
        return SWLENGTH_MAX;
    }

    /* Zero Length */
    length = 0;

    /* Copy SW Channel Scope Header to Msg */
    tmpLength = sizeof(data->header);
    memcpy((msgBody + length), &(data->header), tmpLength);
    length += tmpLength;

    /* Check Data */
    if(data->data == NULL){
        fprintf(stderr,
                "writeSWChannelBody: data->data must not be NULL!\n");
        return SWLENGTH_MAX;
    }

    /* Copy Data to Msg*/
    for(i=0; i<((data->header).numChan); i++){
        /* Copy chanTop */
        tmpLength = sizeof(data->data[i].chanTop);
        memcpy((msgBody + length), &((data->data[i]).chanTop), tmpLength);
        length += tmpLength;
        /* Check value */
        if((data->data)[i].chanValue == NULL){
            fprintf(stderr,
                    "writeSWChannelBody: "
                    "chanValue must not be NULL!\n");
            return SWLENGTH_MAX;
        }
        /* Copy chanValue */
        tmpLength = (data->header).dataLength;
        memcpy((msgBody + length), (data->data[i]).chanValue, tmpLength);
        length += tmpLength;
    }

    /* Final Length Check */
    if(calcLength != length){
        fprintf(stderr, "writeSWChannelBody: "
                "calcLength not equal to length !\n");
	fprintf(stderr, "calcLength: %" PRIswLength "\n", calcLength);
	fprintf(stderr, "length: %" PRIswLength "\n", length);
        return SWLENGTH_MAX;
    }

    return length;
}

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
                            const swLength_t maxBodyLength){

    /* Local Vars */
    swLength_t offset = 0;
    swLength_t calcLength = 0;
    swLength_t tmpLength = 0;
    
    /* Local Structs */
    struct SmartWallHeader swHeader;

    /* Mem Init */
    memset(&swHeader, 0, sizeof(swHeader));    

    /* Check Input */
    if(msg == NULL){
        fprintf(stderr, "readSWMsg: Input 'msg' must not be null!\n");
        return SWLENGTH_MAX;
    }
    if(source == NULL){
        fprintf(stderr, "readSWMsg: source must not be null!\n");
        return SWLENGTH_MAX;
    }
    if(destination == NULL){
        fprintf(stderr, "readSWMsg: destination must not be null!\n");
        return SWLENGTH_MAX;
    }    
    if(targetType == NULL){
        fprintf(stderr, "readSWMsg: 'targetType' must not be null!\n");
        return SWLENGTH_MAX;
    }
    if(msgScope == NULL){
        fprintf(stderr, "readSWMsg: 'msgScope' must not be null!\n");
        return SWLENGTH_MAX;
    }
    if(msgType == NULL){
        fprintf(stderr, "readSWMsg: 'msgType' must not be null!\n");
        return SWLENGTH_MAX;
    }
    if(opcode == NULL){
        fprintf(stderr, "readSWMsg: 'opcode' must not be null!\n");
        return SWLENGTH_MAX;
    }
    if(maxBodyLength != 0){
        if(body == NULL){
            fprintf(stderr, "readSWMsg: data must not be null!\n");
            return SWLENGTH_MAX;
        }
    }

    /* Zero Offset */
    offset = 0;

    /* Extract SW Header */
    if((size_t)(msgLength - offset) < sizeof(swHeader)){
        fprintf(stderr, "readSWMsg: msg too small to contain swHeader!\n");
        return SWLENGTH_MAX;
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
        fprintf(stderr, "readSWMsg: msg does not match declared size!\n");
        return SWLENGTH_MAX;
    }

    /* Setup Output Device Structs and Data */
    source->version = swHeader.version;
    source->swAddr = swHeader.sourceAddress;
    source->groupID = swHeader.groupID;
    source->devTypes = swHeader.sourceTypes;
    destination->version = swHeader.version;
    destination->swAddr = swHeader.destAddress;
    destination->groupID = swHeader.groupID;
    *targetType = swHeader.targetType;
    *msgScope = swHeader.msgScope;
    *msgType = swHeader.msgType;
    *opcode = swHeader.opcode;

    /* Check Body Size */
    if((size_t)(msgLength - offset) > maxBodyLength){
        fprintf(stderr, "readSWMsg: Body length exceeds maxBodyLength!\n");
        return SWLENGTH_MAX;
    }
    else{
        /* Copy Msg Body */
        tmpLength = (msgLength - offset);
        memcpy(body, (msg + offset), tmpLength);
        offset += tmpLength;
        *bodyLength = tmpLength;
    }
    
    return offset;
}

extern swLength_t readSWChannelBody(const uint8_t* msgBody,
                                    const swLength_t bodyLength,
                                    struct SWChannelData* data,
                                    const swLength_t maxNumChan,
                                    const swLength_t maxDataLength){

    /* Local Vars */
    swLength_t offset = 0;
    swLength_t tmpLength = 0;
    int i = 0;
    
    /* Check Input */
    if(msgBody == NULL){
        fprintf(stderr, "readSWChannelBody: "
                "Input 'msgBody' must not be null!\n");
        return SWLENGTH_MAX;
    }
    if(data == NULL){
        fprintf(stderr, "readSWChannelBody: 'data' must not be null!\n");
        return SWLENGTH_MAX;
    }

    /* Zero Offset */
    offset = 0;

    /* Extract Chan Header */
    if((size_t)(bodyLength - offset) < sizeof(data->header)){
        fprintf(stderr, "readSWChannelBody: "
                "'msgBody' too small to contain channel header!\n");
        return SWLENGTH_MAX;
    }
    else{
        /* Copy Chan Header from Msg */
        tmpLength = sizeof(data->header);
        memcpy(&(data->header), (msgBody + offset), tmpLength);
        offset += tmpLength;
    }
    
    /* Check Limits */
    if((data->header).numChan > maxNumChan){
        fprintf(stderr, "readSWChannelBody: 'numChan' exceeds max!\n");
        return SWLENGTH_MAX;
    }
    if((data->header).dataLength > maxDataLength){
        fprintf(stderr, "readSWChannelBody: 'dataLength' exceeds max!\n");
        return SWLENGTH_MAX;
    }

    /* Check data Pointer */
    if(data->data == NULL){
        fprintf(stderr, "readSWChannelBody: 'data->data' must not be null!\n");
        return SWLENGTH_MAX;
    }

    /* Extract Chan Data */
    for(i = 0; i < (data->header).numChan; i++){
        /* Copy chanTop */
        tmpLength = sizeof(data->data[i].chanTop);
        memcpy(&(data->data[i].chanTop), (msgBody + offset), tmpLength);
        offset += tmpLength;
        /* Check value Pointer */
        if(data->data[i].chanValue == NULL){
            fprintf(stderr, "readSWChannelBody: "
                    "'chanValue' must not be null!\n");
            return SWLENGTH_MAX;
        }
        /* Copy chanValue */
        tmpLength = (data->header).dataLength;
        memcpy((data->data[i].chanValue), (msgBody + offset), tmpLength);
        offset += tmpLength;
    }

    return offset;
}
