/* Andy Sayler
 * SmartWall Project
 * SmartWall Function Tests
 * comTools.c
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

/* Local Prototypes */
/* print line of payload data (avoid printing binary data) */
static int print_hex_ascii_line(const uint8_t *payload, int len, int offset);
/* print line of payload data (binary data only) */
static int print_hex_line(const uint8_t *payload, int len, int offset);
/* print channel header */
static int print_swChanHeader(const struct SmartWallChannelHeader* header);
/* Byte Swap */
static inline uint16_t bswap16(uint16_t v);
static inline uint32_t bswap32(uint32_t v);
static inline uint64_t bswap64(uint64_t v);
static inline int isBigEndian(void);

/* External Implmentation */
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
    (void) header;
    fprintf(stderr, "print_swHeader not yet implmented!\n");
    return 0;
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

/* Local Implmentation */
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

/* print line of payload data (binary only) */
static int print_hex_line(const uint8_t *payload, int len, int offset) {
    
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

/* Function to check if input is a valid number in hex, octal, or decimal */
enum numType{OCTAL, DECIMAL, HEX};
extern int isnumeric(char* input){
 
    /* Local Vars */
    enum numType type;
    unsigned int i = 0;
    unsigned int j = 0;
    
    /* read through leading whitespace */
    while(isspace(input[i])){
        i++;
    }

    /* All whitespace or empty */
    if(input[i] == '\0'){
        return 0;
    }

    /* allow negative sign if present */
    if(input[i] == '-'){
        i++;
    }

    /* Lone '-' */
    if(input[i] == '\0'){
        return 0;
    }

    /* determine mode */
    if(input[i] == '0'){
        i++;
        if(input[i] != '\0'){
            if(input[i] == 'x' || input[i] == 'X'){
                i++;
                type = HEX;
            }
            else{
                type = OCTAL;
            }
        }
    }
    else{
        type = DECIMAL;
    }

    /* Loop through input chars and check for legal input */
    for(j = i; j < strlen(input); j++){
        /* Check for legal numeric input */
        if(!isdigit(input[j])){
            /* Not digit */
            switch(type){
            case OCTAL:
                {
                    return 0;
                    break;
                }
            case DECIMAL:
                {
                    return 0;
                    break;
                }
            case HEX:
                {
                    if(isalpha(input[j])){
                        if((toupper(input[j]) < 'A') ||
                           (toupper(input[j]) > 'F')){
                            return 0;
                        }
                    }
                    else{
                        return 0;
                    }
                    break;
                }
            default:
                {
                    fprintf(stderr, "isnumeric: Unhandeled number type!\n");
                    return 0;
                    break;
                }
            }
        }
    }
    
    return 1;
}

/* Byte Swpas */
static inline uint16_t bswap16(uint16_t v){
    return ((v << 8) | (v >> 8));
}

static inline uint32_t bswap32(uint32_t v){
    return
        (((v & 0x000000ff) << 24) | ((v & 0x0000ff00) <<  8) |
         ((v & 0x00ff0000) >>  8) | ((v & 0xff000000) >> 24));
}

static inline uint64_t bswap64(uint64_t v){
    return
        (((v & 0x00000000000000ff) << 56) | ((v & 0x000000000000ff00) << 40) |
         ((v & 0x0000000000ff0000) << 24) | ((v & 0x00000000ff000000) <<  8) |
         ((v & 0x000000ff00000000) >>  8) | ((v & 0x0000ff0000000000) >> 24) |
         ((v & 0x00ff000000000000) >> 40) | ((v & 0xff00000000000000) >> 56));
}

/* Function to detect byte order */
static inline int isBigEndian()
{
    uint16_t word = 0x4321;
    uint8_t* byte = (uint8_t*) &word;
    return (*byte == 0x43);
}

/* Functions to convert N bit numbers between host and network byte order */
extern uint16_t hton16(uint16_t v){
    static int firstRun = 1;
    static int bigEndian = 0;

    if(firstRun){
        bigEndian = isBigEndian();
        firstRun = 0;
        fprintf(stderr, "isBigEndian() = %d\n", bigEndian);
    }

    if(bigEndian){
        return v;
    }
    else{
        return bswap16(v);
    }
    
}

extern uint16_t ntoh16(uint16_t v){
    return hton16(v);
}

extern uint32_t hton32(uint32_t v){
    static int firstRun = 1;
    static int bigEndian = 0;

    if(firstRun){
        bigEndian = isBigEndian();
        firstRun = 0;
        fprintf(stderr, "isBigEndian() = %d\n", bigEndian);
    }

    if(bigEndian){
        return v;
    }
    else{
        return bswap32(v);
    }
    
}

extern uint32_t ntoh32(uint32_t v){
    return hton32(v);
}

extern uint64_t hton64(uint64_t v){
    static int firstRun = 1;
    static int bigEndian = 0;

    if(firstRun){
        bigEndian = isBigEndian();
        firstRun = 0;
        fprintf(stderr, "isBigEndian() = %d\n", bigEndian);
    }

    if(bigEndian){
        return v;
    }
    else{
        return bswap64(v);
    }
    
}

extern uint64_t ntoh64(uint64_t v){
    return hton64(v);
}
