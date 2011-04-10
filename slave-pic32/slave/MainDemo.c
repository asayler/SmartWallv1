/*********************************************************************
 *
 *  Main Application Entry Point and TCP/IP Stack
 *  SmartWall Slave
 *  Andy Sayler
 *
 *********************************************************************
 * FileName:        MainDemo.c
 *
 */

/*
 * This macro uniquely defines this file as the main entry point.
 * There should only be one such definition in the entire project,
 * and this file must define the AppConfig variable as described below.
 */
#define THIS_IS_STACK_APPLICATION

// Include all headers for any enabled TCPIP Stack functions
#include "TCPIP Stack/TCPIP.h"

// Microchip Perephial Library and Debug
#include <plib.h>

// Include functions specific to this stack application
#include "MainDemo.h"
#include "com/SmartWall.h"
#include "slave/swOutlet.h"

// SmartWall Defines
#define SENDPORT SWINPORT
#define LISTENPORT SWOUTPORT
#define MYSWUID 0x0001000000000012ull
#define MYSWGROUP 0x01u
#define MYSWADDRESS 0x0012u
#define MYSWTYPE SW_TYPE_OUTLET | SW_TYPE_UNIVERSAL
#define MYSWCHAN 0x02u
#define CHAN0_INITSTATE OUTLET_CHAN_ON;
#define CHAN1_INITSTATE OUTLET_CHAN_ON;

// Declare AppConfig structure and some other supporting stack variables
APP_CONFIG AppConfig;
static unsigned short wOriginalAppConfigChecksum;	// Checksum of the ROM defaults for AppConfig
BYTE AN0String[8];

// Private helper functions.
// These may or may not be present in all applications.
static void InitAppConfig(void);
static void InitializeBoard(void);

// C30 and C32 Exception Handlers
// If your code gets here, you either tried to read or write
// a NULL pointer, or your application overflowed the stack
// by having too many local variables or parameters declared.
#if defined(__C30__)
	void _ISR __attribute__((__no_auto_psv__)) _AddressError(void)
	{
	    Nop();
		Nop();
	}
	void _ISR __attribute__((__no_auto_psv__)) _StackError(void)
	{
	    Nop();
		Nop();
	}
	
#elif defined(__C32__)
	void _general_exception_handler(unsigned cause, unsigned status)
	{
		Nop();
		Nop();
	}
#endif


//
// Main application entry point.
//
int main(void)
{
	static DWORD t = 0;

	// Initialize application specific hardware
	InitializeBoard();

 	//Initialize the DB_UTILS IO channel
	/* Must add "PIC32_STARTER_KIT" macro definition to the project
      * build options for the C compiler for debug output to work. */
	DBINIT();
	
    // Display the introduction
    DBPRINTF("Smart Outlet Started\n");
	fprintf(stdout, "stdout up\n");
	fprintf(stderr, "stderr up\n");

	// Initialize stack-related hardware components that may be 
	// required by the UART configuration routines
    TickInit();
	#if defined(STACK_USE_MPFS) || defined(STACK_USE_MPFS2)
	MPFSInit();
	#endif

	// Initialize Stack and application related NV variables into AppConfig.
	InitAppConfig();

	// Initialize core stack layers (MAC, ARP, TCP, UDP) and
	// application modules (HTTP, SNMP, etc.)
    StackInit();

	// Initialize any application-specific modules or functions/
	// SmartWall Init
    /* Temp Vars */
    int i;
    numChan_t j;

    /* Setup State Vars */
    unsigned long chState[MYSWCHAN];
    unsigned long* temp = NULL;
    memset(&chState, 0, sizeof(chState));
    
    /* Setup SW Message Vars */
    struct SmartWallDev sourceDeviceInfo;
    memset(&sourceDeviceInfo, 0, sizeof(sourceDeviceInfo));
    struct SmartWallDev destDeviceInfo;
    memset(&destDeviceInfo, 0, sizeof(destDeviceInfo));
    devType_t targetType = 0;
    msgScope_t msgScope = 0;
    msgType_t msgType = 0;
    swOpcode_t opcode = 0;
    long int args[MYSWCHAN];
    memset(&args, 0, sizeof(args));
    struct SWChannelEntry tgtChnEntries[MYSWCHAN];
    memset(&tgtChnEntries, 0, sizeof(tgtChnEntries));
    struct SWChannelData tgtChnData;
    memset(&tgtChnData, 0, sizeof(tgtChnData));
    tgtChnData.data = tgtChnEntries;
    uint8_t msg[SW_MAX_MSG_LENGTH];
    memset(&msg, 0, sizeof(msg));
    swLength_t msgLen = 0;
    uint8_t body[SW_MAX_BODY_LENGTH];
    memset(&body, 0, sizeof(body));
    swLength_t bodyLen = 0;

    /* Init chanData Buffer */
    for(i = 0; i < MYSWCHAN; i++){
        tgtChnData.data[i].chanValue = &(args[i]);
    }

    /* Setup SW Vars */
    struct SmartWallDev myDeviceInfo;    
    memset(&myDeviceInfo, 0, sizeof(myDeviceInfo));
    myDeviceInfo.swAddr = MYSWADDRESS;
    myDeviceInfo.devTypes = MYSWTYPE;
    myDeviceInfo.numChan = MYSWCHAN;
    myDeviceInfo.version = SW_VERSION;
    myDeviceInfo.uid = MYSWUID;
    myDeviceInfo.groupID = MYSWGROUP;

    /* Setup Socket Vars */
    struct sockaddr_in si_me, si_tgt;
    memset(&si_me, 0, sizeof(si_me));
    memset(&si_tgt, 0, sizeof(si_tgt));
    int slen = sizeof(struct sockaddr_in);
    int in, out;
    int b, r;

    /* My IP */
    si_me.sin_family = AF_INET;
    si_me.sin_port = LISTENPORT;
    si_me.sin_addr.s_addr = hton32(IP_ADDR_ANY);
    
    /* TGT IP */
    si_tgt.sin_family = AF_INET;
    si_tgt.sin_port = SENDPORT;
	si_tgt.sin_addr.s_addr = hton32(0xc0a80264u);
    
    /* Setup Sockets */
    in = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(in == INVALID_SOCKET){
        perror("in socket");
        exit(EXIT_FAILURE);
    }
    out = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(out == INVALID_SOCKET){
        perror("out socket");
        exit(EXIT_FAILURE);
    }

    /* Bind In Socket */
    b = bind(in, (struct sockaddr*) &si_me, sizeof(si_me));
    if (b == SOCKET_ERROR){
        perror("in bind");
        exit(EXIT_FAILURE);
    }
	else {
    	fprintf(stdout, "Listening on 0x%" PRIx32 ":%d\n",
            	ntoh32(si_me.sin_addr.s_addr), si_me.sin_port);
	}

    /* Bind Out Socket */
//    b = bind(out, (struct sockaddr*) &si_tgt, sizeof(si_tgt));
//    if (b == SOCKET_ERROR){
//        perror("out bind");
//        exit(EXIT_FAILURE);
//    }
//	else {
//    	fprintf(stdout, "Sending on 0x%" PRIx32 ":%d\n",
//            	ntoh32(si_tgt.sin_addr.s_addr), si_tgt.sin_port);
//	}

	// Now that all items are initialized, begin the co-operative
	// multitasking loop.  This infinite loop will continuously 
	// execute all stack-related tasks, as well as your own
	// application's functions.  Custom functions should be added
	// at the end of this loop.
    // Note that this is a "co-operative mult-tasking" mechanism
    // where every task performs its tasks (whether all in one shot
    // or part of it) and returns so that other tasks can do their
    // job.
    // If a task needs very long time to do its job, it must be broken
    // down into smaller pieces so that other tasks can have CPU time.
    while(1)
    {
        // Blink LED0 (right most one) every second.
        if(TickGet() - t >= TICK_SECOND/2ul)
        {
            t = TickGet();
            LED0_IO ^= 1;
        }

        // This task performs normal stack task including checking
        // for incoming packet, type of packet and calling
        // appropriate stack entity to process it.
        StackTask();

        // This tasks invokes each of the core stack application tasks
        StackApplications();

		// Process application specific tasks here.
		// Any custom modules or processing you need to do should
		// go here.

		r = recvfrom(in, msg, SW_MAX_MSG_LENGTH, 0,
                     (struct sockaddr*) &si_tgt, &slen);
        if(r == SOCKET_ERROR){
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        else{
            msgLen = r;
        }

		if(msgLen > 0){
        	/* Print Info (TEST) */
        	fprintf(stdout, "Received packet from 0x%" PRIx32 ":%d\n",
            	    ntoh32(si_tgt.sin_addr.s_addr), si_tgt.sin_port);
        	fprintf(stdout, "Message Size: %d\n", msgLen);
        	//print_payload(msg, msgLen);

			if((msgLen > sizeof(struct SmartWallHeader)) &&
	           (msgLen < SW_MAX_MSG_LENGTH)){
	            msgLen = readSWMsg(msg, msgLen,
	                               &sourceDeviceInfo, &destDeviceInfo,
	                               &targetType, &msgScope, &msgType, &opcode,
	                               body, &bodyLen, SW_MAX_BODY_LENGTH);
	            if(msgLen == SWLENGTH_MAX){
	                fprintf(stderr, "Could not read SW message\n");
	                exit(EXIT_FAILURE);
	            }
	            /* Check if Message is For Me */
	            if(myDeviceInfo.groupID != destDeviceInfo.groupID){
	                fprintf(stderr, "Group mismatch\n");
	                fprintf(stderr, "My groupID: %u\n", myDeviceInfo.groupID);
	                fprintf(stderr, "Dest groupID: %u\n", destDeviceInfo.groupID);
	                continue;
	            }
	            if((myDeviceInfo.swAddr != destDeviceInfo.swAddr) && 
	               (destDeviceInfo.swAddr != SW_ADDR_BROADCAST)){
	                fprintf(stderr, "Address mismatch\n");
	                fprintf(stderr, "My swAddr: 0x%x\n", myDeviceInfo.swAddr);
	                fprintf(stderr, "Dest swAddr: 0x%x\n", destDeviceInfo.swAddr);
	                continue;
	            }
	            if(!(myDeviceInfo.devTypes & targetType)){
	                fprintf(stderr, "Type mismatch\n");
	                continue;
	            }
	            /* TODO: Confirm it's from a master */
	            /* Switch on Message Scope */
	            switch(msgScope){
	            case SW_SCP_CHANNEL:
	                {
	                    //fprintf(stdout, "Received Channel Message\n");
	                    /* TODO: Remove 8 byte limit*/
	                    bodyLen = readSWChannelBody(body, bodyLen, &tgtChnData,
	                                                myDeviceInfo.numChan,
	                                                sizeof(unsigned long));
	                    if(bodyLen == SWLENGTH_MAX){
	                        fprintf(stderr, "Could not read SW body\n");
	                        exit(EXIT_FAILURE);
	                    }
	                    /* Switch on Opcode */
	                    switch(opcode){
	                    case OUTLET_CH_OP_STATE:
	                        {
	                            /* Switch on Message Type: SET OR QUERY */
	                            switch(msgType){
	                            case SW_MSG_SET:
	                                /* Set State */
	                                for(i = 0; i < tgtChnData.header.numChan; i++){
	                                    j = tgtChnData.data[i].chanTop.chanNum;
	                                    if(j < myDeviceInfo.numChan){
	                                        /* TODO: Check valid state */
	                                        temp = tgtChnData.data[i].chanValue;
	                                        chState[j] = *temp;
											if(j == 0){
												if(*temp == 1){
													LED1_IO = 1;
												}
												else{
													LED1_IO = 0;	
												}
											}
											else if(j == 1){
												if(*temp == 1){
													LED2_IO = 1;
												}
												else{
													LED2_IO = 0;	
												}	
											}
//	                                        fprintf(stdout, "Ch %u Set to %lu\n",
//	                                                j, chState[j]);
	                                    }
	                                    else{
	                                        fprintf(stderr,
	                                                "Invalid chanNum\n");
	                                    }
	                                }
	                            case SW_MSG_QUERY:
	                                /* Report State */
	                                for(i = 0; i < tgtChnData.header.numChan; i++){
	                                    j = tgtChnData.data[i].chanTop.chanNum;
	                                    if(j < myDeviceInfo.numChan){
	                                        temp = tgtChnData.data[i].chanValue;
	                                        *temp = chState[j];
	                                    }
	                                    else{
	                                        fprintf(stderr,
	                                                "Invalid chanNum\n");
	                                    }
	                                }
	                                /* TODO: Remove 8 byte limit */
	                                /*tgtChnData.header.dataLength = 
	                                  sizeof(unsigned long);*/
	                                /* Assemble Message Body */
	                                bodyLen=writeSWChannelBody(body,
	                                                           SW_MAX_BODY_LENGTH,
	                                                           &tgtChnData);
	                                if(bodyLen == SWLENGTH_MAX){
	                                    fprintf(stderr, "Error generating "
	                                            "message body.\n");
	                                    exit(EXIT_FAILURE);
	                                }
	                                /* Assemble Message */
	                                msgLen = writeSWMsg(msg, SW_MAX_MSG_LENGTH,
	                                                    &myDeviceInfo,
	                                                    &sourceDeviceInfo,
	                                                    SW_TYPE_OUTLET,
	                                                    SW_SCP_CHANNEL,
	                                                    SW_MSG_REPORT, opcode,
	                                                    body, bodyLen);
	                                if(msgLen == SWLENGTH_MAX){
	                                    fprintf(stderr, "Error generating "
	                                            "message.\n");
	                                    exit(EXIT_FAILURE);
	                                }
	                                /* Print Test payload */
	                                //fprintf(stdout, "Response: \n");
	                                //print_payload(msg, msgLen);
	                                /* Set Port */
	                                si_tgt.sin_port = SENDPORT;
	                                /* Send Message */
	                                r = sendto(out, msg, msgLen, 0, (struct sockaddr*) &si_tgt, sizeof(si_tgt));
	                                if(r == SOCKET_ERROR){
	                                    perror("sendto");
										//exit(EXIT_FAILURE);
	                                }
	                                /* Print Info (TEST) */
						        	fprintf(stdout, "Sent packet to 0x%" PRIx32 ":%d\n",
						            	    ntoh32(si_tgt.sin_addr.s_addr), si_tgt.sin_port);
	                                fprintf(stdout, "Message Size: %d\n", r);
	
	                                break;
	                            default:
	                                fprintf(stderr, "Unhandeled msgType\n");
	                                continue;
	                                break;
	                            }
	                            break;
	                        }
	                    default:
	                        {
	                            fprintf(stderr, "Unhandeled Opcode\n");
	                            continue;
	                            break;
	                        }
	                    }
	                    break;
	                }
	            default:
	                {
	                    fprintf(stderr, "Unhandeled Message Scope\n");
	                    continue;
	                    break;
	                }
	            }
	        }
	        else{
	            fprintf(stderr, "Malformatted message"
	                    " - msgLen outside of normal limits.\n");
	            continue;
	        }
		}

	}

	//Clean Up
	closesocket(in);
    closesocket(out);
}

/****************************************************************************
  Function:
    static void InitializeBoard(void)

  Description:
    This routine initializes the hardware.  It is a generic initialization
    routine for many of the Microchip development boards, using definitions
    in HardwareProfile.h to determine specific initialization.

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
static void InitializeBoard(void)
{	
	// LEDs
	LED0_TRIS = 0;
	LED1_TRIS = 0;
	LED2_TRIS = 0;
	LED3_TRIS = 0;
	LED4_TRIS = 0;
	LED5_TRIS = 0;
	LED6_TRIS = 0;
	LED7_TRIS = 0;
	LED_PUT(0x00);


#if defined(__PIC32MX__)
{
	// Enable multi-vectored interrupts
	INTEnableSystemMultiVectoredInt();
	
	// Enable optimal performance
	SYSTEMConfigPerformance(GetSystemClock());
	mOSCSetPBDIV(OSC_PB_DIV_1);				// Use 1:1 CPU Core:Peripheral clocks
	
	// Disable JTAG port so we get our I/O pins back, but first
	// wait 50ms so if you want to reprogram the part with 
	// JTAG, you'll still have a tiny window before JTAG goes away.
	// The PIC32 Starter Kit debuggers use JTAG and therefore must not 
	// disable JTAG.
	DelayMs(50);
	#if !defined(__MPLAB_DEBUGGER_PIC32MXSK) && !defined(__MPLAB_DEBUGGER_FS2)
		DDPCONbits.JTAGEN = 0;
	#endif
	LED_PUT(0x00);				// Turn the LEDs off
	
	CNPUESET = 0x00098000;		// Turn on weak pull ups on CN15, CN16, CN19 (RD5, RD7, RD13), which is connected to buttons on PIC32 Starter Kit boards
}
#endif

AD1CHS = 0;					// Input to AN0 (potentiometer)
AD1PCFGbits.PCFG4 = 0;		// Disable digital input on AN4 (TC1047A temp sensor)
#if defined(__32MX460F512L__) || defined(__32MX795F512L__)	// PIC32MX460F512L and PIC32MX795F512L PIMs has different pinout to accomodate USB module
	AD1PCFGbits.PCFG2 = 0;		// Disable digital input on AN2 (potentiometer)
#else
	AD1PCFGbits.PCFG5 = 0;		// Disable digital input on AN5 (potentiometer)
#endif

// ADC
AD1CON1 = 0x84E4;			// Turn on, auto sample start, auto-convert, 12 bit mode (on parts with a 12bit A/D)
AD1CON2 = 0x0404;			// AVdd, AVss, int every 2 conversions, MUXA only, scan
AD1CON3 = 0x1003;			// 16 Tad auto-sample, Tad = 3*Tcy
#if defined(__32MX460F512L__) || defined(__32MX795F512L__)	// PIC32MX460F512L and PIC32MX795F512L PIMs has different pinout to accomodate USB module
	AD1CSSL = 1<<2;				// Scan pot
#else
	AD1CSSL = 1<<5;				// Scan pot
#endif

// Deassert all chip select lines so there isn't any problem with 
// initialization order.
#if defined(ENC_CS_TRIS)
	ENC_CS_IO = 1;
	ENC_CS_TRIS = 0;
#endif
#if defined(ENC100_CS_TRIS)
	ENC100_CS_IO = (ENC100_INTERFACE_MODE == 0);
	ENC100_CS_TRIS = 0;
#endif
#if defined(EEPROM_CS_TRIS)
	EEPROM_CS_IO = 1;
	EEPROM_CS_TRIS = 0;
#endif
#if defined(SPIRAM_CS_TRIS)
	SPIRAM_CS_IO = 1;
	SPIRAM_CS_TRIS = 0;
#endif
#if defined(SPIFLASH_CS_TRIS)
	SPIFLASH_CS_IO = 1;
	SPIFLASH_CS_TRIS = 0;
#endif

#if defined(SPIRAM_CS_TRIS)
	SPIRAMInit();
#endif
#if defined(EEPROM_CS_TRIS)
	XEEInit();
#endif
#if defined(SPIFLASH_CS_TRIS)
	SPIFlashInit();
#endif
}

/*********************************************************************
 * Function:        void InitAppConfig(void)
 *
 * PreCondition:    MPFSInit() is already called.
 *
 * Input:           None
 *
 * Output:          Write/Read non-volatile config variables.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
// MAC Address Serialization using a MPLAB PM3 Programmer and 
// Serialized Quick Turn Programming (SQTP). 
// The advantage of using SQTP for programming the MAC Address is it
// allows you to auto-increment the MAC address without recompiling 
// the code for each unit.  To use SQTP, the MAC address must be fixed
// at a specific location in program memory.  Uncomment these two pragmas
// that locate the MAC address at 0x1FFF0.  Syntax below is for MPLAB C 
// Compiler for PIC18 MCUs. Syntax will vary for other compilers.
//#pragma romdata MACROM=0x1FFF0
static ROM BYTE SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};
//#pragma romdata

static void InitAppConfig(void)
{
#if defined(EEPROM_CS_TRIS) || defined(SPIFLASH_CS_TRIS)
	unsigned char vNeedToSaveDefaults = 0;
#endif
	
	while(1)
	{
		// Start out zeroing all AppConfig bytes to ensure all fields are 
		// deterministic for checksum generation
		memset((void*)&AppConfig, 0x00, sizeof(AppConfig));
		
		AppConfig.Flags.bIsDHCPEnabled = TRUE;
		AppConfig.Flags.bInConfigMode = TRUE;
		memcpypgm2ram((void*)&AppConfig.MyMACAddr, (ROM void*)SerializedMACAddress, sizeof(AppConfig.MyMACAddr));
//		{
//			_prog_addressT MACAddressAddress;
//			MACAddressAddress.next = 0x157F8;
//			_memcpy_p2d24((char*)&AppConfig.MyMACAddr, MACAddressAddress, sizeof(AppConfig.MyMACAddr));
//		}
		AppConfig.MyIPAddr.Val = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | MY_DEFAULT_IP_ADDR_BYTE4<<24ul;
		AppConfig.DefaultIPAddr.Val = AppConfig.MyIPAddr.Val;
		AppConfig.MyMask.Val = MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2<<8ul | MY_DEFAULT_MASK_BYTE3<<16ul | MY_DEFAULT_MASK_BYTE4<<24ul;
		AppConfig.DefaultMask.Val = AppConfig.MyMask.Val;
		AppConfig.MyGateway.Val = MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2<<8ul | MY_DEFAULT_GATE_BYTE3<<16ul | MY_DEFAULT_GATE_BYTE4<<24ul;
		AppConfig.PrimaryDNSServer.Val = MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2<<8ul  | MY_DEFAULT_PRIMARY_DNS_BYTE3<<16ul  | MY_DEFAULT_PRIMARY_DNS_BYTE4<<24ul;
		AppConfig.SecondaryDNSServer.Val = MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2<<8ul  | MY_DEFAULT_SECONDARY_DNS_BYTE3<<16ul  | MY_DEFAULT_SECONDARY_DNS_BYTE4<<24ul;
	
	
		// SNMP Community String configuration
		#if defined(STACK_USE_SNMP_SERVER)
		{
			BYTE i;
			static ROM char * ROM cReadCommunities[] = SNMP_READ_COMMUNITIES;
			static ROM char * ROM cWriteCommunities[] = SNMP_WRITE_COMMUNITIES;
			ROM char * strCommunity;
			
			for(i = 0; i < SNMP_MAX_COMMUNITY_SUPPORT; i++)
			{
				// Get a pointer to the next community string
				strCommunity = cReadCommunities[i];
				if(i >= sizeof(cReadCommunities)/sizeof(cReadCommunities[0]))
					strCommunity = "";
	
				// Ensure we don't buffer overflow.  If your code gets stuck here, 
				// it means your SNMP_COMMUNITY_MAX_LEN definition in TCPIPConfig.h 
				// is either too small or one of your community string lengths 
				// (SNMP_READ_COMMUNITIES) are too large.  Fix either.
				if(strlenpgm(strCommunity) >= sizeof(AppConfig.readCommunity[0]))
					while(1);
				
				// Copy string into AppConfig
				strcpypgm2ram((char*)AppConfig.readCommunity[i], strCommunity);
	
				// Get a pointer to the next community string
				strCommunity = cWriteCommunities[i];
				if(i >= sizeof(cWriteCommunities)/sizeof(cWriteCommunities[0]))
					strCommunity = "";
	
				// Ensure we don't buffer overflow.  If your code gets stuck here, 
				// it means your SNMP_COMMUNITY_MAX_LEN definition in TCPIPConfig.h 
				// is either too small or one of your community string lengths 
				// (SNMP_WRITE_COMMUNITIES) are too large.  Fix either.
				if(strlenpgm(strCommunity) >= sizeof(AppConfig.writeCommunity[0]))
					while(1);
	
				// Copy string into AppConfig
				strcpypgm2ram((char*)AppConfig.writeCommunity[i], strCommunity);
			}
		}
		#endif
	
		// Load the default NetBIOS Host Name
		memcpypgm2ram(AppConfig.NetBIOSName, (ROM void*)MY_DEFAULT_HOST_NAME, 16);
		FormatNetBIOSName(AppConfig.NetBIOSName);

		// Compute the checksum of the AppConfig defaults as loaded from ROM
		wOriginalAppConfigChecksum = CalcIPChecksum((BYTE*)&AppConfig, sizeof(AppConfig));

		#if defined(EEPROM_CS_TRIS) || defined(SPIFLASH_CS_TRIS)
		{
			NVM_VALIDATION_STRUCT NVMValidationStruct;

			// Check to see if we have a flag set indicating that we need to 
			// save the ROM default AppConfig values.
			if(vNeedToSaveDefaults)
				SaveAppConfig(&AppConfig);
		
			// Read the NVMValidation record and AppConfig struct out of EEPROM/Flash
			#if defined(EEPROM_CS_TRIS)
			{
				XEEReadArray(0x0000, (BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
				XEEReadArray(sizeof(NVMValidationStruct), (BYTE*)&AppConfig, sizeof(AppConfig));
			}
			#elif defined(SPIFLASH_CS_TRIS)
			{
				SPIFlashReadArray(0x0000, (BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
				SPIFlashReadArray(sizeof(NVMValidationStruct), (BYTE*)&AppConfig, sizeof(AppConfig));
			}
			#endif
	
			// Check EEPROM/Flash validitity.  If it isn't valid, set a flag so 
			// that we will save the ROM default values on the next loop 
			// iteration.
			if((NVMValidationStruct.wConfigurationLength != sizeof(AppConfig)) ||
			   (NVMValidationStruct.wOriginalChecksum != wOriginalAppConfigChecksum) ||
			   (NVMValidationStruct.wCurrentChecksum != CalcIPChecksum((BYTE*)&AppConfig, sizeof(AppConfig))))
			{
				// Check to ensure that the vNeedToSaveDefaults flag is zero, 
				// indicating that this is the first iteration through the do 
				// loop.  If we have already saved the defaults once and the 
				// EEPROM/Flash still doesn't pass the validity check, then it 
				// means we aren't successfully reading or writing to the 
				// EEPROM/Flash.  This means you have a hardware error and/or 
				// SPI configuration error.
				if(vNeedToSaveDefaults)
				{
					while(1);
				}
				
				// Set flag and restart loop to load ROM defaults and save them
				vNeedToSaveDefaults = 1;
				continue;
			}
			
			// If we get down here, it means the EEPROM/Flash has valid contents 
			// and either matches the ROM defaults or previously matched and 
			// was run-time reconfigured by the user.  In this case, we shall 
			// use the contents loaded from EEPROM/Flash.
			break;
		}
		#endif
		break;
	}
}

#if defined(EEPROM_CS_TRIS) || defined(SPIFLASH_CS_TRIS)
void SaveAppConfig(const APP_CONFIG *ptrAppConfig)
{
	NVM_VALIDATION_STRUCT NVMValidationStruct;

	// Ensure adequate space has been reserved in non-volatile storage to 
	// store the entire AppConfig structure.  If you get stuck in this while(1) 
	// trap, it means you have a design time misconfiguration in TCPIPConfig.h.
	// You must increase MPFS_RESERVE_BLOCK to allocate more space.
	#if defined(STACK_USE_MPFS) || defined(STACK_USE_MPFS2)
		if(sizeof(NVMValidationStruct) + sizeof(AppConfig) > MPFS_RESERVE_BLOCK)
			while(1);
	#endif

	// Get proper values for the validation structure indicating that we can use 
	// these EEPROM/Flash contents on future boot ups
	NVMValidationStruct.wOriginalChecksum = wOriginalAppConfigChecksum;
	NVMValidationStruct.wCurrentChecksum = CalcIPChecksum((BYTE*)ptrAppConfig, sizeof(APP_CONFIG));
	NVMValidationStruct.wConfigurationLength = sizeof(APP_CONFIG);

	// Write the validation struct and current AppConfig contents to EEPROM/Flash
	#if defined(EEPROM_CS_TRIS)
	    XEEBeginWrite(0x0000);
	    XEEWriteArray((BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
		XEEWriteArray((BYTE*)ptrAppConfig, sizeof(APP_CONFIG));
    #else
		SPIFlashBeginWrite(0x0000);
		SPIFlashWriteArray((BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
		SPIFlashWriteArray((BYTE*)ptrAppConfig, sizeof(APP_CONFIG));
    #endif
}
#endif
