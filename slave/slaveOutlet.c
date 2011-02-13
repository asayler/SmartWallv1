/* Andy Sayler
 * SmartWall Project
 * SmartWall Slave Outlet Sim
 * slaveOutlet.c
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
#include "swOutlet.h"

#define LISTENPORT 4333 /* 4329 to 4339 Free as of 2/1/2011 */
#define BUFLEN 1024 /* 1 kB datagram buffer */

#define MYSWGROUP 0x01
#define MYSWADDRESS 0x0010
#define MYSWVER 0x1
#define MYSWTYPE SW_TYPE_OUTLET
#define MYSWCHAN 0x02

#define CHAN1_INITSTATE OUTLET_CHAN_ON;
#define CHAN2_INITSTATE OUTLET_CHAN_ON;

int main(int argc, char *argv[]){

    /* Handel Input */
    (void) argc;
    (void) argv;

    /* Setup State Vars */
    uint8_t ch1State = CHAN1_INITSTATE;
    uint8_t ch2State = CHAN2_INITSTATE;

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
    int s, b, r;
    unsigned int slen = sizeof(si_other);
    
    /*Print Base Data */
    fprintf(stdout, "I am a SW_TYPE_OUTLET.\n");
    fprintf(stdout, "Device Type:       0x%x\n", mySWType);
    fprintf(stdout, "SmartWall Version: 0x%x\n", mySWVersion);
    fprintf(stdout, "SmartWall Address: 0x%x\n", mySWAddress);
    fprintf(stdout, "SmartWall Group:   0x%x\n", mySWGroup);
    fprintf(stdout, "Channel 1 State:   %u\n", ch1State);
    fprintf(stdout, "Channel 2 State:   %u\n", ch2State);

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
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(LISTENPORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    b = bind(s, (struct sockaddr*) &si_me, sizeof(si_me));
    if (b < 0){
        perror("bind");
        exit(1);
    }

    while(1){
        r = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*) &si_other, &slen);
        if(r < 0){
            perror("recvfrom");
            exit(1);
        }
        fprintf(stdout, "Received packet from %s:%d\n",
                inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        fprintf(stdout, "Message Size: %d\n", r);
        struct SmartWallHeader rcvHeader;
        memcpy(&rcvHeader, buf, sizeof(rcvHeader));
        printSWHeader(stdout, &rcvHeader);
    }

    close(s);

    return 0;

}
