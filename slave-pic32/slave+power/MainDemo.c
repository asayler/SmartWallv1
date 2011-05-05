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
#include "com/SmartWallSockets.h"
#include "com/comTools.h"
#include "slave/swOutlet.h"
#include "slave/swUniversal.h"

/* For Random Seeding */
#include <time.h>
#define CH0POWER 5
#define CH1POWER 5

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

// define channel states
int CHANNEL_1_STATE = 0;
int CHANNEL_2_STATE = 0;
int CHANNEL_1_POWER;
int CHANNEL_2_POWER;

//Local Functions
int updateDeviceState(const struct outletDeviceState* myState);
int SET_CHANNEL_STATE(int, int);
int GET_CHANNEL_STATE(int);
void MEASURE_POWER();
int MEASURE(int);
void CONFIG_3909();
void DelayNs(unsigned int);

#define SWDEBUG
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
	//Hardware Init
    // STEP 2. configure the port registers
    mPORTDSetPinsDigitalOut(BIT_9);
	mPORTESetPinsDigitalOut(BIT_0 | BIT_1 | BIT_2 | BIT_3);
	// STEP 3. initialize the port pin states = outputs low
	mPORTEClearBits(BIT_0);
	// STEP 8. configure SPI port and MCP3909
	//SpiChnEnable(1,0);
	//SpiChnOpen(1, SPI_OPEN_MSTEN | SPI_OPEN_SSEN | SPI_OPEN_SMP_END | SPI_OPEN_FRMEN | SPI_OPEN_FSP_IN | SPI_OPEN_FSP_HIGH | SPI_OPEN_MODE32 | SPI_OPEN_FRM_CNT32 | SPI_OPEN_SIDL, 1);
	//SpiChnEnable(1,1);
//	DBPRINTF("openspi1... \n");
//	//ConfigIntSPI1(SPI_TX_INT_DIS | SPI_RX_INT_DIS | SPI_INT_PRI_2);
//	DBPRINTF("configintspi1... \n");
//	CONFIG_3909();

	// SmartWall Init
	/* Local Temp Vars */
    unsigned int i = 0;

    /* Setup Temporary Processor Storage Vars */
    /* Chan Storage */
    struct SWChannelEntry tmpChnEntries[MYSWCHAN];
    memset(&tmpChnEntries, 0, sizeof(tmpChnEntries));
    struct SWChannelData tmpChnData;
    memset(&tmpChnData, 0, sizeof(tmpChnData));
    union outletChanArg tmpChanArgs[MYSWCHAN];
    memset(&tmpChanArgs, 0, sizeof(tmpChanArgs));
    tmpChnData.data = tmpChnEntries;
    /* Init chanData Buffer */
    for(i = 0; i < MYSWCHAN; i++){
        tmpChnData.data[i].chanValue = &(tmpChanArgs[i]);
    }
    struct SWChannelLimits limits;
    limits.maxNumChan = MYSWCHAN;
    limits.maxDataLength = sizeof(*tmpChanArgs);
    
    /* Setup Processors */
    struct SWDevProcessor processors[NUMOUTLETPROCESSORS];
    memset(processors, 0, sizeof(processors));
    processors[0].processorScope = SW_SCP_CHANNEL;
    processors[0].data = &tmpChnData;
    processors[0].dataLimits = &limits;
    processors[0].decoder = (readSWBody)readSWChannelBody;
    processors[0].handeler = (swDevHandeler)outletChnDevHandeler;
    processors[0].encoder = (writeSWBody)writeSWChannelBody;
    
    /* Setup State Vars */
    struct outletDeviceState myState;
    memset(&myState, 0, sizeof(myState));
    outletChanState_t chState[MYSWCHAN];
    memset(&chState, 0, sizeof(chState));
    outletChanPower_t chPower[MYSWCHAN];
    memset(&chPower, 0, sizeof(chPower));
    struct SWDeviceInfo myDevice;
    memset(&myDevice, 0, sizeof(myDevice));
    myState.myDev = &myDevice;
    myState.chState = chState;
    myState.chPower = chPower;
    enum SWReceiverState machineState = RST_SETUP;
    
    /* Setup SW Vars */
    struct SWDeviceInfo tgtDevice;
    myState.myDev->devInfo.swAddr = MYSWADDRESS;
    myState.myDev->devInfo.devTypes = MYSWTYPE;
    myState.myDev->devInfo.numChan = MYSWCHAN;
    myState.myDev->devInfo.version = SW_VERSION;
    myState.myDev->devInfo.uid = MYSWUID;
    myState.myDev->devInfo.groupID = MYSWGROUP;
    
    /* My IP */
    myState.myDev->devIP.sin_family = AF_INET;
    myState.myDev->devIP.sin_port = LISTENPORT;
    myState.myDev->devIP.sin_addr.s_addr = hton32(INADDR_ANY);
    tgtDevice.devIP.sin_family = AF_INET;
    tgtDevice.devIP.sin_port = SENDPORT;
    tgtDevice.devIP.sin_addr.s_addr = hton32(INADDR_ANY);

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
        machineState = swReceiverStateMachine(machineState,
                                            myState.myDev, &tgtDevice,
                                            &myState,
                                            processors, NUMOUTLETPROCESSORS);

		updateDeviceState(&myState);

	}
}

int updateDeviceState(const struct outletDeviceState* state){

	//Channel 0 State
	if(state->chState[0] == 1){
		LED1_IO = 1;
	}	
	else{
		LED1_IO = 0;
	}
	
	//Channel 1 State
	if(state->chState[1] == 1){
		LED2_IO = 1;
	}	
	else{
		LED2_IO = 0;
	}

	SET_CHANNEL_STATE(state->chState[0], state->chState[1]);
	
	/* Update Power */
    if(state->chState[0]){
        if(rand() % 2){
            state->chPower[0] = CH0POWER + ((rand() % 1024) / 1024.0);
        }
        else{
            state->chPower[0] = CH0POWER - ((rand() % 1024) / 1024.0);
        }
    }
    else{
        state->chPower[0] = 0;
    }
    
    if(state->chState[1]){
        if(rand() % 2){
            state->chPower[1] = CH1POWER + ((rand() % 1024) / 1024.0);
        }
        else{
            state->chPower[1] = CH1POWER - ((rand() % 1024) / 1024.0);
        }
    }
    else{
        state->chPower[1] = 0;
    }

	return 0;
}	

/******************************************************************************
*	DelayNs()
*
*	This functions provides a software millisecond delay
******************************************************************************/
void DelayNs(unsigned int msec)
{
	unsigned int tWait, tStart;
		
    tWait=(GetSystemClock()/20000000)*msec;
    tStart=ReadCoreTimer();
    while((ReadCoreTimer()-tStart)<tWait);		// wait for the time to pass

}




/******************************************************************************
*	SET_CHANNEL_STATE()
*
*	This function sets the state of a specified channel
*
*	returns 0
*
******************************************************************************/
int SET_CHANNEL_STATE(int state_1, int state_2)
{
	// set state of channel 1
	// **********************
	if (state_1 == 1)
	{
		mPORTESetBits(BIT_1);
	}
	if (state_1 == 0)
	{
		mPORTEClearBits(BIT_1);
	}

	// set state of channel 2
	// **********************
	if (state_2 == 1)
	{
		mPORTESetBits(BIT_2);
	}
	if (state_2 == 0)
	{
		mPORTEClearBits(BIT_2);
	}

	return 0;

}





/******************************************************************************
*	GET_CHANNEL_STATE()
*
*	This function returns the current state of a specified channel
*
* 	returns the state of the selected channel
*	returns -1 if an invalid channel is selected
*
******************************************************************************/
int GET_CHANNEL_STATE(int channel)
{
	if (channel == 1)
		{
			return CHANNEL_1_STATE;
		}
	else if (channel == 2)
		{
			return CHANNEL_2_STATE;
		}
	else
		{
			DBPRINTF("IMPROPER CHANNEL SELECTION ERROR.  PLEASE TRY AGAIN. \n");
			return -1;
		}
}



/*****************************************************************************
*	MEARSURE_POWER()
*
*	This function measures how much power is being used by the outlet's load
*	at a given instant
*
*	returns the power being used in Watts as an integer
*
*****************************************************************************/
void MEASURE_POWER()
{
	// CHANNEL 1
	// *********
	if (CHANNEL_1_STATE == 0)
	{
		CHANNEL_1_POWER = 0;
		DBPRINTF("CHANNEL 1 IS OFF \n");
	}
	else
	{
		CHANNEL_1_POWER = MEASURE(1);
		//CHANNEL_1_POWER = 5;
	}

	// CHANNEL 2
	// *********
	if (CHANNEL_2_STATE == 0)
	{
		CHANNEL_2_POWER = 0;
		DBPRINTF("CHANNEL 2 IS OFF \n");
	}
	else
	{
		//CHANNEL_2_POWER = MEASURE(2);
		CHANNEL_2_POWER = 21;
	}
}


/*****************************************************************************
*	MEASURE(int channel)
*
*	This function reads the digitally sampled voltage and current readings
*	from the MCP3909 via the SPI port
*****************************************************************************/
int MEASURE(int channel)
{
	DBPRINTF("MEASURING CHANNEL 1..... \n");
	// local variable declarations
	int MASK = 0x0000FFFF;
	int num_samples = 10;
	int Csamples[num_samples];
	int Vsamples[num_samples];
	int Power = 0;
	int i = 0;

	int data;
	int samples[num_samples];

	// get data
	for (i=0 ; i<num_samples ; i++)
	{
		DBPRINTF("sample....\n");
		mPORTEClearBits(BIT_3);
		data = SpiChnGetC(channel);
		mPORTESetBits(BIT_3);
		samples[i] = data;
	}
	

	// sample the current and voltage enough to calculate avg active power
	for (i=0 ; i<num_samples ; i++)
	{
		Vsamples[i] = samples[i] & MASK;
		Csamples[i] = (samples[i] >> 16) & MASK;
	}

	// process sample data received from MCP3909
	// Pinst = Vinst * Iinst
	for (i=0 ; i<num_samples ; i++)
	{
		Power += Csamples[i] * Vsamples[i];
	}
	
	Power = Power/num_samples;

	return Power;
}


/*****************************************************************************
*	CONFIG_3909()
*
*	This function configures the MCP3909 to transmit data from its two
*	A/D converters to the PIC32 via the SPI port
*****************************************************************************/
void CONFIG_3909()
{
	
	DBPRINTF("CONFIG_3909...\n");
	SpiChnEnable(1,0);
	SpiChnConfigure(1, SPI_CONFIG_MSTEN | SPI_CONFIG_MSSEN | SPI_CONFIG_SMP_END | SPI_CONFIG_MODE8 | SPI_CONFIG_ON);
	SpiChnEnable(1,1);
	// SPI mode code

	// set CS high
	mPORTESetBits(BIT_3);
	// set MCLR low 
	mPORTEClearBits(BIT_0);
	// delay
	DelayNs(10000);
	// set MCLR high
	mPORTESetBits(BIT_0);
	// set CS low
	mPORTEClearBits(BIT_3);

	//DelayNs(1);

	// feed SPI mode code to MCP3909
	SpiChnPutC(1, 0b10100100);
	
	mPORTESetBits(BIT_3);

	DelayMs(5000);




	mPORTEClearBits(BIT_3);

	int txdata = 0;
	SpiChnPutC(1, txdata);

	mPORTESetBits(BIT_3);



	SpiChnEnable(1,0);
	SpiChnConfigure(1, SPI_CONFIG_MSTEN | SPI_CONFIG_SSEN | SPI_CONFIG_SMP_END | SPI_CONFIG_FRMEN | SPI_CONFIG_FSP_IN | SPI_CONFIG_FSP_HIGH | SPI_CONFIG_MODE32 | SPI_CONFIG_FRM_CNT32 | SPI_CONFIG_SIDL | SPI_CONFIG_ON);
	SpiChnEnable(1,1);

	DBPRINTF("config successful \n");
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

