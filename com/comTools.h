/* Andy Sayler
 * SmartWall Project
 * SmartWall Function Tests
 * comTest.h
 * Created 03/03/11 
 *
 * On print_hex_ascii_line:
 * On print_payload:
 * With examples taken from Tim Carstens' "Programming with pcap"
 * and from the assciated example sniffer "sniffex.c" used
 * as per license.
 *
 * Change Log:
 * 03/03/11 - Created
 * ---
 */

#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "SmartWall.h"

/* print packet payload data */
extern int print_payload(const uint8_t* payload, int len);

/* print SW Header data */
extern int print_swHeader(const struct SmartWallHeader* header);

/* print SW Device data */
extern int print_swDev(const struct SmartWallDev* dev);

/* print SW Channel Message data */
extern int print_swChanMsgBody(const struct SWChannelData* data,
                               const swLength_t maxNumChan,
                               const swLength_t maxDataLength);

/* Function to check if input is a valid number in hex, octal, or decimal */
extern int isnumeric(char* input);

/* Functions to convert N bit numbers between host and network byte order */
extern uint16_t hton16(uint16_t v);
extern uint16_t ntoh16(uint16_t v);
extern uint32_t hton32(uint32_t v);
extern uint32_t ntoh32(uint32_t v);
extern uint64_t hton64(uint64_t v);
extern uint64_t ntoh64(uint64_t v);
