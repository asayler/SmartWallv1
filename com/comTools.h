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

/* print packet payload data (avoid printing binary data) */
extern int print_payload(const uint8_t* payload, int len);
