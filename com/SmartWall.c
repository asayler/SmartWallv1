/* Andy Sayler
 * SmartWall Project
 * SmartWall Common Code
 * SmartWall.c
 * Created 2/2/11
 *
 * Change Log:
 * 02/02/11 - Created
 * ---
 */

#include "SmartWall.h"

int printSWHeader(FILE* stream, const struct SmartWallHeader* input){
    
    int num = 0;

    num += fprintf(stream, "Version:   0x%" PRIxSWVer "\n",
                   ((input->verType >> 4) & 0x0f));
    num += fprintf(stream, "Msg Type:  0x%" PRIxMsgType "\n",
                   (input->verType & 0x0f));
    num += fprintf(stream, "Src Type:  0x%" PRIxDevType "\n",
                   input->sourceType);
    num += fprintf(stream, "Dest Type: 0x%" PRIxDevType "\n",
                   input->destType);
    num += fprintf(stream, "Group ID:  0x%" PRIxGrpID  "\n",
                   input->groupID);
    num += fprintf(stream, "Src Addr:  0x%" PRIxSWAddr  "\n",
                   input->sourceAddress);
    num += fprintf(stream, "Dest Addr: 0x%" PRIxSWAddr  "\n",
                   input->destAddress);

    return num;
}
