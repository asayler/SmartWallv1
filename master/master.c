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

#include "../com/SmartWall.h"

#define SENDPORT 4333 /* 4329 to 4339 Free as of 2/1/2011 */
#define BUFLEN 1024 /* 1 kB datagram buffer */
#define SLAVEIP "127.0.0.1"

#define MYSWGROUP 0x01
#define MYSWADDRESS 0x0001
#define MYSWVER 0x1
#define MYSWTYPE SW_TYPE_MASTER
#define MYSWCHAN 0x00

int main(int argc, char *argv[]){

    /* Handel Input */
    (void) argc;
    (void) argv;

    /* Setup State Vars */
    
    /* Setup SW Vars */
    const uint8_t  mySWVersion = MYSWVER;
    const uint8_t  mySWType    = MYSWTYPE;
    uint16_t mySWAddress = MYSWADDRESS;
    uint8_t  mySWGroup   = MYSWGROUP;

    /* Setup Network Vars */
    struct SmartWallHeader myHeader;
    uint8_t buf[BUFLEN];

    /* Setup Socket Vars */
    struct sockaddr_in si_me, si_other;
    int s;
    int a, b, r;
    unsigned int slen = sizeof(si_other);
    
    /*Print Base Data */
    fprintf(stdout, "I am a SW_TYPE_MASTER.\n");
    fprintf(stdout, "Device Type:       0x%x\n", mySWType);
    fprintf(stdout, "SmartWall Version: 0x%x\n", mySWVersion);
    fprintf(stdout, "SmartWall Address: 0x%x\n", mySWAddress);
    fprintf(stdout, "SmartWall Group:   0x%x\n", mySWGroup);
    
    /* Print Test Data */
    fprintf(stdout, "SmartWall Header Size: %lu\n", sizeof(myHeader));

    /* Setup Socket */
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if(s < 0){
        perror("socket");
        exit(1);
    }

    /* Zero and Init Structs */
    memset((char*) &si_me, 0, sizeof(si_me));
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
    myHeader.verType = (mySWVersion << 4) | (uint8_t) 0x01;
    myHeader.sourceType = mySWType;
    myHeader.destType = SW_TYPE_OUTLET;
    myHeader.groupID = mySWGroup;
    myHeader.sourceAddress = mySWAddress;
    myHeader.destAddress = 0x0010;

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
