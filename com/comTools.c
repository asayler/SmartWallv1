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
 * 04/03/11 - Split print functions out into comPrint.c
 * ---
 */

#include "comTools.h"

/* Local Prototypes */
/* Byte Swap */
static inline uint16_t bswap16(uint16_t v);
static inline uint32_t bswap32(uint32_t v);
static inline uint64_t bswap64(uint64_t v);
/* Endian Test */
static inline int isBigEndian(void);

/*** External Implmentation ***/

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

/*** Local Implmentation */

/* Byte Swaps */
static inline uint16_t bswap16(uint16_t v){
    return (((v & 0x00ffu) << 8) | ((v & 0xff00u ) >> 8));
}

static inline uint32_t bswap32(uint32_t v){
    return
        (((v & 0x000000fful) << 24) | ((v & 0x0000ff00ul) <<  8) |
         ((v & 0x00ff0000ul) >>  8) | ((v & 0xff000000ul) >> 24));
}

static inline uint64_t bswap64(uint64_t v){
    return
        (((v & 0x00000000000000ffull) << 56) |
         ((v & 0x000000000000ff00ull) << 40) |
         ((v & 0x0000000000ff0000ull) << 24) |
         ((v & 0x00000000ff000000ull) <<  8) |
         ((v & 0x000000ff00000000ull) >>  8) |
         ((v & 0x0000ff0000000000ull) >> 24) |
         ((v & 0x00ff000000000000ull) >> 40) |
         ((v & 0xff00000000000000ull) >> 56));
}
