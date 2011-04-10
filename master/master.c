/* Andy Sayler
 * SmartWall Project
 * SmartWall Master
 * master.c
 * Created 2/2/11
 *
 * Change Log:
 * 02/02/11 - Created
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

#include <fcntl.h>
#include <netdb.h>

#include "swMaster.h"

#define SENDPORT 4333 /* 4329 to 4339 Free as of 2/1/2011 */
#define BUFLEN 1024 /* 1 kB datagram buffer */
#define SLAVEIP "127.0.0.1"

#define MYSWUID 0x0001000000000001ull
#define MYSWGROUP 0x01u
#define MYSWADDRESS 0x0001u
#define MYSWVER 0x1u
#define MYSWTYPE SW_TYPE_MASTER | SW_TYPE_UNIVERSAL
#define MYSWCHAN 0x00u

/* Private Functions */
int initialize(void);

int main(int argc, char *argv[]){

    /* Handel Input */
    (void) argc;
    (void) argv;

    /* Setup State Vars */
    
    /* Setup SW Vars */
    const devUID_t mySWUID = MYSWUID;
    const swVersion_t  mySWVersion = MYSWVER;
    const devType_t  mySWType    = MYSWTYPE;
    swAddress_t mySWAddress = MYSWADDRESS;
    groupID_t  mySWGroup   = MYSWGROUP;
    
    /* Setup Network Vars */
    struct SmartWallHeader myHeader;
    
    /* Setup Socket Vars */
    struct sockaddr_in si_me, si_other;
    int s;
    int a, r;
    unsigned int slen = sizeof(si_other);

    /* Init */
    
    /* IP */
    memset((char*) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    a = inet_aton(SLAVEIP, &si_me.sin_addr);
    if (a < 0){
        perror("inet_aton");
        exit(1);
    }
    
    /*Print Base Data */
    fprintf(stdout, "I am a SW_TYPE_MASTER.\n");
    fprintf(stdout, "SmartWall UID:     0x%" PRIxDevUID "\n", mySWUID);
    fprintf(stdout, "Device Type:       0x%" PRIxDevType "\n", mySWType);
    fprintf(stdout, "SmartWall Version: 0x%" PRIxSWVer "\n", mySWVersion);
    fprintf(stdout, "SmartWall Address: 0x%" PRIxSWAddr "\n", mySWAddress);
    fprintf(stdout, "SmartWall Group:   0x%" PRIxGrpID "\n", mySWGroup);
    
    /* Print Test Data */
    fprintf(stdout, "SmartWall Header Size: %lu\n", sizeof(myHeader));

    /* Setup Socket */
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if(s < 0){
        perror("socket");
        exit(1);
    }

    /* Zero and Init Structs */
    memset((char*) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(SENDPORT);
    a = inet_aton(SLAVEIP, &si_other.sin_addr);
    if (a < 0){
        perror("inet_aton");
        exit(1);
    }
    
    /*
    b = bind(s, (struct sockaddr*) &si_me, sizeof(si_me));
    if (b < 0){
        perror("bind");
        exit(1);
    }
    */

    /* Setup Header */
    myHeader.version = mySWVersion;
    myHeader.msgScope = SW_SCP_CHANNEL;
    myHeader.msgType = SW_MSG_QUERY;
    myHeader.groupID = mySWGroup;
    myHeader.sourceAddress = mySWAddress;
    myHeader.destAddress = 0x0010;
    myHeader.sourceTypes = mySWType;
    myHeader.targetType = SW_TYPE_OUTLET;
    myHeader.opcode = 0x00;
    myHeader.totalLength = sizeof(myHeader) + 0;

    r = sendto(s, (char*) &myHeader, sizeof(myHeader), 0,
               (struct sockaddr*) &si_other, slen);
    if(r < 0){
        perror("sendto");
        exit(1);
    }
    fprintf(stdout, "Sent packet to %s:%d\n",
            inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
    fprintf(stdout, "Message Size: %d\n", r);

    close(s);

    return 0;

}
