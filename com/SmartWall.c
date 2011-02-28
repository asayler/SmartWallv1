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

extern swLength_t writeSWChannelMsg(char* msg, const swLength_t maxLength,
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

    /* Local Structs and Mem Init */
    struct SmartWallHeader swHeader;
    memset(&swHeader, 0, sizeof(swHeader));
    memset(msg, 0, maxLength);

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
    fprintf(stderr, "0: calcLength = %" PRIswLength  "\n", calcLength);
    calcLength += sizeof(swHeader);
    fprintf(stderr, "1: calcLength = %" PRIswLength  "\n", calcLength);
    calcLength += sizeof(data->header);
    fprintf(stderr, "2: calcLength = %" PRIswLength  "\n", calcLength);
    calcLength += (data->header).numChan * sizeof(data->data[0].chanHead);
    fprintf(stderr, "3a: calcLength = %" PRIswLength  "\n", calcLength);    
    calcLength += (data->header).numChan * (data->header).dataLength;
    fprintf(stderr, "3b: calcLength = %" PRIswLength  "\n", calcLength);

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

    fprintf(stderr, "0: length = %" PRIswLength  "\n", length);

    /* Copy SW Header to Msg */
    tmpLength = sizeof(swHeader);
    memcpy((msg + length), &swHeader, tmpLength);
    length += tmpLength;

    fprintf(stderr, "1: length = %" PRIswLength  "\n", length);

    /* Copy SW Channel Scope Header to Msg */
    tmpLength = sizeof(data->header);
    memcpy((msg + length), &(data->header), tmpLength);
    length += tmpLength;

    fprintf(stderr, "2: length = %" PRIswLength  "\n", length);

    /* Copy Data to Msg */
    for(i=0; i<((data->header).numChan); i++){
        tmpLength = sizeof(data->data[i].chanHead);
        memcpy((msg + length), &(data->data[i]).chanHead, tmpLength);
        length += tmpLength;
	fprintf(stderr, "3a: length = %" PRIswLength  "\n", length);
        tmpLength += (data->header).dataLength;
        memcpy((msg + length), &(data->data[i]).chanValue, tmpLength);
        length += tmpLength;
	fprintf(stderr, "3b: length = %" PRIswLength  "\n", length);
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
