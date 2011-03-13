/* Andy Sayler
 * SmartWall Project
 * SmartWall Master
 * swChnMsg.c
 * Created 3/12/11
 *
 * Change Log:
 * 03/12/11 - Created
 * ---
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <limits.h>

#include <fcntl.h>
#include <netdb.h>

#include "../master/swMaster.h"

#define SENDPORT 4333 /* 4329 to 4339 Free as of 2/1/2011 */
#define BUFLEN 1024 /* 1 kB datagram buffer */
#define SLAVEIP "127.0.0.1"

#define MYSWUID 0x0001000000000001
#define MYSWGROUP 0x01
#define MYSWADDRESS 0x0001
#define MYSWVER 0x1
#define MYSWTYPE SW_TYPE_MASTER | SW_TYPE_UNIVERSAL
#define MYSWCHAN 0x00

#define PGMNAME "swChnMsg"

/* Private Functions */

/* Option enum */
enum OPTIONS{
    HELP, ERROR, SEND
};

/* Input Format */
/* ./swChnMsg <SW Dest Address> <SW Msg Type> <SW Tgt Type> <SW Opcode> */
/*            <Chn Arg Size (bytes)> <Chn#> <Chn Arg> ...               */

/* Main Execution Entrance */
int main(int argc, char *argv[]){

    /* Local Vars */
    enum OPTIONS mode;
    unsigned long temp = 0;

    /* Setup SW Vars */
    const devUID_t mySWUID = MYSWUID;
    const swVersion_t  mySWVersion = MYSWVER;
    const devType_t  mySWType    = MYSWTYPE;
    devType_t tgtSWType;
    const swAddress_t mySWAddress = MYSWADDRESS;
    swAddress_t tgtSWAddress;
    groupID_t  tgtSWGroup;
    const msgScope_t msgScope = SW_SCP_CHANNEL;
    msgType_t msgType;
    swOpcode_t opcode;

    /* Setup Network Vars */
    uint8_t buf[BUFLEN];

    /* Setup Socket Vars */
    struct sockaddr_in si_me, si_tgt;
    int s;
    int a, b, r;
    unsigned int slen = sizeof(si_tgt);

    /* Handel Input */
    if(argc < 8){
        /* Must have at least 8 args (name + 6) */
        mode = ERROR;
        (void) argv;
    }
    else if (argc % 2 != 0){
        /* Must have even number of args */
        mode = ERROR;
        (void) argv;
    }
    else{
        mode = SEND;
        /* Parse and Check Input */
        /* Target Address */
        temp = strtoul(argv[1], NULL, 0);
        if(temp == 0){
            fprintf(stderr, "%s: Could not convert 1st argument to int.\n",
                    PGMNAME);
            exit(EXIT_FAILURE);
        }
        if(temp == ULONG_MAX){
            perror(PGMNAME);
            fprintf(stderr, "%s: 1st argument out of range.\n",
                    PGMNAME);
            exit(EXIT_FAILURE);
        }
        if(temp > SWADDRESS_MAX){
            fprintf(stderr, "%s: Target address out of range.\n",
                    PGMNAME);
            exit(EXIT_FAILURE);
        }
        tgtSWAddress = temp;
        /* Message Type */
        /* Target Device Type */
        /* Opcode */
        /* Argument Size */
        
    }

    if((mode == ERROR) | (mode == HELP)){
        fprintf(stderr, "%s: Usage Format\n"
                "<SW Dest Address> <SW Msg Type> <SW Tgt Type> <SW Opcode> "
                "<Chn Arg Size (bytes)> <Chn#> <Chn Arg> ...\n", PGMNAME);
    }
    
    return 0;

}
