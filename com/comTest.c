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

int main(int argc, char* argv[]){

    (void) argc;
    (void) argv;

    char msg[SW_MAX_MSG_LENGTH];
    struct SmartWallDev source;
    struct SmartWallDev destination;
    struct SWChannelData data;
    struct SWChannelEntry dataEntries[3];
    char payload0[10] = "test0";
    char payload1[10] = "test1";
    char payload2[10] = "test2";

    dataEntries[0].chanHead.chanNum = 0;
    dataEntries[0].chanValue = payload0;
    dataEntries[1].chanHead.chanNum = 1;
    dataEntries[1].chanValue = payload1;
    dataEntries[2].chanHead.chanNum = 2;
    dataEntries[2].chanValue = payload2;

    data.header.numChan = 3;
    data.header.dataLength = 10;
    data.data = dataEntries;

    source.version = 0x01;
    source.address = 0x01;
    source.groupID = 0x01;
    source.types = SW_TYPE_MASTER | SW_TYPE_UNIVERSAL;

    destination.version = 0x01;
    destination.address = 0x03;
    destination.groupID = 0x01;
    destination.types = SW_TYPE_OUTLET | SW_TYPE_UNIVERSAL;
    uint64_t test0 = SW_TYPE_UNIVERSAL;
    uint32_t test1 = SW_TYPE_OUTLET;
    unsigned long long int test2 = SW_TYPE_UNIVERSAL;

    fprintf(stdout, "uint64_t is %d bytes\n", sizeof(test0));
    fprintf(stdout, "uint32_t is %d bytes\n", sizeof(test1));
    fprintf(stdout, "unsigned long long int is %d bytes\n", sizeof(test2));
    
    fprintf(stdout, "value of test0: %" PRIxDevType "\n", test0);

    swLength_t msgSize = writeSWChannelMsg(msg, SW_MAX_MSG_LENGTH,
					   &source, &destination,
					   SW_TYPE_OUTLET,
					   SW_MSG_REQUEST,
					   0x01,
					   &data);

    fprintf(stdout, "writeSWChannelMsg msgSize: %" PRIswLength "\n", msgSize);

    return 0;
}
