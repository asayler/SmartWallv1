/* Andy Sayler
 * SmartWall Project
 * SmartWall Function Tests
 * comPrint.c
 * Created 04/03/11 
 *
 * On print_hex_ascii_line:
 * On print_payload:
 * With examples taken from Tim Carstens' "Programming with pcap"
 * and from the assciated example sniffer "sniffex.c" used
 * as per license.
 *
 * Change Log:
 * 03/04/11 - Created
 * ---
 */

#include "comPrint.h"

/*** Local Prototypes ***/

/* print line of payload data (avoid printing binary data) */
static int print_hex_ascii_line(const uint8_t *payload, int len, int offset);
/* print channel header */
static int print_swChanHeader(const struct SmartWallChannelHeader* header);

/*** External Implmentation ***/

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

/* print SW Header data */
extern int print_swHeader(const struct SmartWallHeader* header){
    int num = 0;
    FILE* stream = stdout;

    num += fprintf(stream, "Version:   0x%" PRIxSWVer "\n",
                   header->version);
    num += fprintf(stream, "Msg Scope: 0x%" PRIxMsgScope "\n",
                   header->msgScope);
    num += fprintf(stream, "Msg Type:  0x%" PRIxMsgType "\n",
                   header->msgType);
    num += fprintf(stream, "Group ID:  0x%" PRIxGrpID  "\n",
                   header->groupID);
    num += fprintf(stream, "Src Addr:  0x%" PRIxSWAddr  "\n",
                   header->sourceAddress);
    num += fprintf(stream, "Dest Addr: 0x%" PRIxSWAddr  "\n",
                   header->destAddress);
    num += fprintf(stream, "Src Types: 0x%" PRIxDevType "\n",
                   header->sourceTypes);
    num += fprintf(stream, "Trgt Type: 0x%" PRIxDevType "\n",
                   header->targetType);
    num += fprintf(stream, "Opcode:    0x%" PRIxSWOpcode "\n",
                   header->opcode);
    num += fprintf(stream, "Tot Lng:   0x%" PRIxSWLength "\n",
                   header->totalLength);
    
    return num;
}

/* print SW Device data */
extern int print_swDev(const struct SmartWallDev* dev){

    /* Loval Vars */
    int cnt = 0;
    
    /* Input Check */
    if(dev == NULL){
        cnt += fprintf(stderr, "print_swDev: dev must not be NULL!\n");
        return (-1 * cnt);
    }

    /* Print */
    cnt += fprintf(stdout, "version: 0x%2.2" PRIxSWVer "\n", dev->version);
    cnt += fprintf(stdout, "address: 0x%4.4" PRIxSWAddr "\n", dev->swAddr);
    cnt += fprintf(stdout, "groupID: 0x%2.2" PRIxGrpID "\n", dev->groupID);
    cnt += fprintf(stdout, "types:   0x%16.16" PRIxDevType "\n",
                   dev->devTypes);

    return cnt;
}

/* print SW Channel Message data */
extern int print_swChanMsgBody(const struct SWChannelData* data,
                               const swLength_t maxNumChan,
                               const swLength_t maxDataLength){
    
    /* Local Vars */
    int cnt = 0;
    int i = 0;
    
    /* Input Check */
    if(data == NULL){
        cnt += fprintf(stderr,
                       "print_swChanMsgBody: data must not be NULL!\n");
        return (-1 * cnt);
    }

    /* Size Check */
    if((data->header).numChan > maxNumChan){
        cnt += fprintf(stderr, "print_swChanMsgBody: numChan exceeds max!\n");
        return (-1 * cnt);
    }
    if((data->header).dataLength > maxDataLength){
        cnt += fprintf(stderr,
                       "print_swChanMsgBody: dataLength exceeds max!\n");
        return (-1 * cnt);
    }

    /* Print Chan Header */
    cnt += print_swChanHeader(&(data->header));
    
    /* Check Data */
    if(data->data == NULL){
        cnt += fprintf(stderr,
                       "print_swChanMsgBody: data->data must not be NULL!\n");
        return (-1 * cnt);
    }

    /* Print Data */
    for(i = 0; i < (data->header).numChan; i++){
        cnt += fprintf(stdout, "Chan #: 0x%2.2" PRIxNumChan "\n",
                       (data->data)[i].chanTop.chanNum);
        /* Check value */
        if((data->data)[i].chanValue == NULL){
            cnt += fprintf(stderr,
                           "print_swChanMsgBody: "
                           "chanValue must not be NULL!\n");
            return (-1 * cnt);
        }
        cnt += fprintf(stdout, "Value:\n");
        cnt += print_hex_ascii_line((data->data)[i].chanValue,
                                    (data->header).dataLength,
                                    0);
    }

    return 0;
}

/*** Local Implmentation ***/

/* print line of payload data (avoid printing binary data) */
static int print_hex_ascii_line(const uint8_t* payload, int len, int offset) {
    
    /* local vars */
    int i;
    int gap;
    int count = 0;
    const uint8_t *ch;
    
    /* input check */
    if(payload == NULL){
        count += fprintf(stderr, "print_hex_ascii_line: "
                         "payload must not be NULL!\n");
        return (-1 * count);
    }

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

/* print channel header */
static int print_swChanHeader(const struct SmartWallChannelHeader* header){
   
    /* Local vars */
    int cnt = 0;

    /* Input Check */
    if(header == NULL){
        cnt += fprintf(stderr,
                       "print_swChanHeader: header must not be NULL!\n");
        return (-1 * cnt);
    }

    /* Print */
    cnt += fprintf(stdout, "numChan: 0x%2.2" PRIxNumChan "\n",
                   header->numChan);
    cnt += fprintf(stdout, "dataLength: 0x%4.4" PRIxSWLength "\n",
                   header->dataLength);

    return cnt;
}
