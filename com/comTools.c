/* Andy Sayler
 * SmartWall Project
 * SmartWall Function Tests
 * comTest.c
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

#include "comTools.h"

/* print line of payload data (avoid printing binary data) */
static int print_hex_ascii_line(const uint8_t *payload, int len, int offset);

/* print line of payload data (binary data only) */
static int print_hex_line(const uint8_t *payload, int len, int offset);

/* print packet payload data (avoid printing binary data) */
extern int print_payload(const uint8_t* payload, int len){
    
    int len_rem = len;
    int line_width = 8; /* number of bytes per line */
    int line_len;
    int offset = 0;	 /* zero-based offset counter */
    int count = 0;
    const uint8_t* ch = payload;
    
    if(len <= 0){
        return EXIT_FAILURE;
    }
    
    if(len <= line_width){
        /* data fits on one line */
        count += print_hex_ascii_line(ch, len, offset);
    }
    else{
        /* data spans multiple lines */
        for( ;; ){
            /* compute current line length */
            line_len = line_width % len_rem;
            /* print line */
            count += print_hex_ascii_line(ch, line_len, offset);
            /* compute total remaining */
            len_rem = len_rem - line_len;
            /* shift pointer to remaining bytes to print */
            ch = ch + line_len;
            /* add offset */
            offset = offset + line_width;
            /* check if we have line width chars or less */
            if (len_rem <= line_width) {
                /* print last line and get out */
                count += print_hex_ascii_line(ch, len_rem, offset);
                break;
            }
        }
    }
    
    return count;
}

/* print line of payload data (avoid printing binary data) */
static int print_hex_ascii_line(const uint8_t *payload, int len, int offset) {
    
    /* local vars */
    int i;
    int gap;
    int count = 0;
    const uint8_t *ch;
    
    /* offset */
    count += fprintf(stdout, "%05d   ", offset);
    
    /* hex */
    ch = payload;
    for(i = 0; i < len; i++){
        count += fprintf(stdout, "%02" PRIx8 " ", *ch);
        ch++;
    }
        
    /* fill hex gap with spaces if not full line */
    if(len < 8){
        gap = 8 - len;
        for(i = 0; i < gap; i++){
            count += fprintf(stdout, "   ");
        }
    }
    fprintf(stdout, "| ");
    
    /* ascii (if printable) */
    ch = payload;
    for(i = 0; i < len; i++){
        if(isprint(*ch)){
            count += fprintf(stdout, "%c", *ch);
        }
        else{
            count += fprintf(stdout, ".");
        }
        ch++;
    }
    
    count += fprintf(stdout, "\n");
    
    return count;
}

/* print line of payload data (binary only) */
static int print_hex_line(const uint8_t *payload, int len, int offset) {
    
    /* local vars */
    int i;
    int gap;
    int count = 0;
    const uint8_t *ch;
    
    /* offset */
    count += fprintf(stdout, "%05d   ", offset);
    
    /* hex */
    ch = payload;
    for(i = 0; i < len; i++){
        count += fprintf(stdout, "%02" PRIx8 " ", *ch);
        ch++;
    }
        
    /* fill hex gap with spaces if not full line */
    if(len < 8){
        gap = 8 - len;
        for(i = 0; i < gap; i++){
            count += fprintf(stdout, "   ");
        }
    }
    
    count += fprintf(stdout, "\n");
    
    return count;
}
