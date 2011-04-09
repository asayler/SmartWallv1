/*  MeasurePower.c
 *
 *	Program to measure the power being used by the load on the outlet
 *	
 *	Most setup code copied from ports_basic.c
*/

 /*
 *
 * Oscillator Configuration Bit Settings:
 * 		- Oscillator Selection Bits = 	Primary Osc w/PLL (XT+HS+EC+PLL)
 *		- Primary Oscillator Config = 	HS osc mode
 *		- PLL Input Divider			=	2x Divider
 *		- PLL Multiplier			=	18x Multiplier
 *
 * Notes:
 *		- To clear a mis-match condition, always read the port first,
 *		  then clear CNIF interrupt flag.
 *
 *		- PIC32MX Starter Kit Led and Sw assignments:
 *		  LED1 is on PORTD.RD0
 *		  LED2 is on PORTD.RD1
 *		  LED3 is on PORTD.RD2
 *		  SW1 is on PORTD.RD6, Change Notice (CN15)
 *		  SW2 is on PORTD.RD7, Change Notice (CN16)
 *		  SW3 is on PORTD.RD13, Change Notice (CN19)
 *
 *		- No resistor pullups are provided for SW1..SW3.  PORTD internal
 *		  pullups should be enabled.
 *
 */


#include <plib.h>

// Configuration Bit settings
// SYSCLK = 80 MHz (8MHz Crystal/ FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 40 MHz
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care
#pragma config FNOSC    = PRIPLL        // Oscillator Selection
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (PIC32 Starter Kit: use divide by 2 only)
#pragma config FPLLMUL  = MUL_20        // PLL Multiplier
#pragma config FPLLODIV = DIV_1         // PLL Output Divider
#pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor
#pragma config FWDTEN   = OFF           // Watchdog Timer
#pragma config WDTPS    = PS1           // Watchdog Timer Postscale
#pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
#pragma config OSCIOFNC = OFF           // CLKO Enable
#pragma config POSCMOD  = XT            // Primary Oscillator
#pragma config IESO     = OFF           // Internal/External Switch-over
#pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable
#pragma config CP       = OFF           // Code Protect
#pragma config BWP      = OFF           // Boot Flash Write Protect
#pragma config PWP      = OFF           // Program Flash Write Protect
#pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
#pragma config DEBUG    = OFF           // Debugger Disabled for Starter Kit


#define CONFIG          (CN_ON | CN_IDLE_CON)
#define PINS            (CN15_ENABLE)
#define PULLUPS         (CN15_PULLUP_ENABLE)
#define INTERRUPT       (CHANGE_INT_ON | CHANGE_INT_PRI_2)


typedef struct
{
	unsigned long	nChars;		// number of characters in the packet
	unsigned long	data[1];	// data itself
}SpiPkt;	// a transferred SPI packet. assumes we're working in 32 bits mode

// application defines
#define SYS_FREQ		(80000000)

//
int num_samples = 1;
unsigned long cycle_sample_data[1];

// prototype
void DelayMs(unsigned int);

void SAMPLE();
void CONFIG_3909();
int MEASURE_POWER();


int main(void)
{
	DBINIT();
	DBPRINTF("MAIN.... \n");
   	unsigned int temp;
	int POWER;
	//bool RELAY;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //STEP 1. Configure cache, wait states and peripheral bus clock
	SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // STEP 2. configure the port registers
    mPORTDSetPinsDigitalOut(BIT_0 | BIT_1 | BIT_2 | BIT_9);
	mPORTESetPinsDigitalOut(BIT_0 | BIT_1 | BIT_2);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // STEP 3. initialize the port pin states = outputs low
    mPORTDClearBits(BIT_0 | BIT_1 | BIT_2);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // STEP 4. enable change notice, enable discrete pins and weak pullups
    mCNOpen(CONFIG, PINS, PULLUPS);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // STEP 5. read port(s) to clear mismatch on change notice pins
    temp = mPORTDRead();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // STEP 6. clear change notice interrupt flag
    ConfigIntCN(INTERRUPT);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // STEP 7. enable multi-vector interrupts
    INTEnableSystemMultiVectoredInt();

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// STEP 8. configure SPI port and MCP3909
	OpenSPI1(FRAME_ENABLE_ON | ENABLE_SDO_PIN | SPI_MODE32_ON | SPI_CKE_ON | SLAVE_ENABLE_OFF | CLK_POL_ACTIVE_LOW | MASTER_ENABLE_ON , SPI_ENABLE | SPI_FRZ_CONTINUE | SPI_IDLE_STOP | SPI_RX_OVFLOW_CLR);
	CONFIG_3909();

	DBPRINTF("CONFIGURED.... \n");


   while(1)
   {
		// Toggle LED's to signify code is looping
		mPORTDToggleBits(BIT_0);     // toggle LED1 (same as LATDINV = 0x0002)

		SAMPLE();

		POWER = MEASURE_POWER();

		DBPRINTF("power measured! \n");

		DelayMs(1000);

		/************************
		*ETHERNET COMMUNICATIONS*
		************************/

   };

}




	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // STEP 9. configure the CN interrupt handler
void __ISR(_CHANGE_NOTICE_VECTOR, ipl2) ChangeNotice_Handler(void)
{
	unsigned int temp;

    // clear the mismatch condition
    temp = PORTReadBits(IOPORT_D, BIT_6);

    // .. things to do .. toggle the led
    mPORTDToggleBits(BIT_1);
	mPORTEToggleBits(BIT_2);

	// clear the interrupt flag
    mCNClearIntFlag();
}




/******************************************************************************
*	DelayMs()
*
*	This functions provides a software millisecond delay
******************************************************************************/
void DelayMs(unsigned int msec)
{
	unsigned int tWait, tStart;
		
    tWait=(SYS_FREQ/2000)*msec;
    tStart=ReadCoreTimer();
    while((ReadCoreTimer()-tStart)<tWait);		// wait for the time to pass

}



/*****************************************************************************
*	MEARSURE_POWER()
*
*	This function measures how much power is being used by the outlet's load
*	at a given instant
*****************************************************************************/
int MEASURE_POWER()
{
	DBPRINTF("MEASURE POWER.... \n");
	// local variable declarations
	int MASK = 0x0000FFFF;
	int Csamples[num_samples];
	int Vsamples[num_samples];
	int Power = 0;
	int i = 0;

	// get array of samples from the MCP3909
	SAMPLE();
	
/*
	// sample the current and voltage enough to calculate avg active power
	for (i=0 ; i<num_samples ; i++)
	{
		Vsamples[i] = cycle_sample_data[i] & MASK;
		Csamples[i] = (cycle_sample_data[i] >> 16) & MASK;
		DBPRINTF("Vsamples \n");
		DBPRINTF("Csamples \n");
	}

	/* process sample data received from MCP3909
	********************************************

		// Pinst = Vinst * Iinst
		for (i=0 ; i<num_samples ; i++)
		{
			Power += Csamples[i] * Vsamples[i];
		}

		Power = Power/num_samples;
*/

		return Power;
}





/*****************************************************************************
*	SAMPLE()
*
*	This function reads the digitally sampled voltage and current readings
*	from the MCP3909 via the SPI port
*****************************************************************************/
void SAMPLE()
{
	DBPRINTF("sample....\n");
	/*	
	int		txferSize = 5;
	SpiPkt*	pTxPkt;
	SpiPkt*	pRxPkt;

	pTxPkt=(SpiPkt*)malloc(txferSize*sizeof(long)+2);
	pRxPkt=(SpiPkt*)malloc(txferSize*sizeof(long)+2);

	SpiChnChangeMode(1, 1, 1, 1);		// set slave mode
	SpiChnGetS(1, (unsigned int*)pRxPkt, 1);
	*/

	// get data
	int data;
	data = getcSPI1();
	
	// SpiChnGetS(1, (unsigned int*)cycle_sample_data, num_samples);
	DBPRINTF("SUCCESS!!!! \n");
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

	// SPI mode code
	int code = 0xA4;
	
	// set CS high
	mPORTDSetBits(BIT_9);
	// set MCLR low 
	mPORTEClearBits(BIT_0);
	// delay
	DelayMs(5);
	// set MCLR high
	mPORTESetBits(BIT_0);
	// set CS low
	mPORTDClearBits(BIT_9);

	// feed SPI mode code to MCP3909
	SpiChnPutC(1, code);

	DelayMs(1000);

	SpiChnPutC(1, 0x12345678);
	
	int data;
	data = getcSPI1();
	char a = (char)data;
	char * c;
	c = &a;
	DBPUTC(c);

	data = getcSPI1();
	a = (char)data;
	c = &a;
	DBPUTC(c);
}

