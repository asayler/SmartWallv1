/* Andy Sayler
 * SmartWall Project
 * SmartWall Device Implementation
 * swDevice.c
 * Created 2/2/11
 *
 * Change Log:
 * 02/02/11 - Created
 * 02/17/11 - Renamed swDevcies.c (formerly SmartWall.c)
 * 02/20/11 - Updated for Protocol Version 1.1.1
 * ---
 */

#include "swDevice.h"

int printSWHeader(FILE* stream, const struct SmartWallHeader* input){
    
    int num = 0;

    num += fprintf(stream, "Version:   0x%" PRIxSWVer "\n",
                   input->version);
    num += fprintf(stream, "Msg Scope: 0x%" PRIxMsgScope "\n",
                   input->version);
    num += fprintf(stream, "Msg Type:  0x%" PRIxMsgType "\n",
                   input->msgType);
    num += fprintf(stream, "Group ID:  0x%" PRIxGrpID  "\n",
                   input->groupID);
    num += fprintf(stream, "Src Addr:  0x%" PRIxSWAddr  "\n",
                   input->sourceAddress);
    num += fprintf(stream, "Dest Addr: 0x%" PRIxSWAddr  "\n",
                   input->destAddress);
    num += fprintf(stream, "Src Types: 0x%" PRIxDevType "\n",
                   input->sourceTypes);
    num += fprintf(stream, "Trgt Type: 0x%" PRIxDevType "\n",
                   input->targetType);
    num += fprintf(stream, "Opcode:    0x%" PRIxSWOpcode "\n",
                   input->opcode);
    num += fprintf(stream, "Tot Lng:   0x%" PRIxTotLength "\n",
                   input->totalLength);
    
    return num;
}
