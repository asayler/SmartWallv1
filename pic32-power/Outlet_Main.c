/* Outlet_Main.c
 *
 *	Main embedded code for outlet
 *	
 *
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
#pragma config DEBUG    = ON         	// Debugger Disabled for Starter Kit


#define CONFIG          (CN_ON)
#define PINS            (CN15_ENABLE | CN16_ENABLE)
#define PULLUPS         (CN15_PULLUP_ENABLE | CN16_PULLUP_ENABLE)
#define INTERRUPT       (CHANGE_INT_ON | CHANGE_INT_PRI_2)



// application defines
#define SYS_FREQ		(80000000)




// define channel states
int CHANNEL_1_STATE = 0;
int CHANNEL_2_STATE = 0;

int SET_CHANNEL_STATE(int, int);
int GET_CHANNEL_STATE(int);




// define power measuring
int CHANNEL_1_POWER;
int CHANNEL_2_POWER;
void MEASURE_POWER();
int MEASURE(int);


void DelayMs(unsigned int);
void CONFIG_3909();



int main(void)
{
	DBINIT();
	
	DBPRINTF("MAIN... \n");
   	unsigned int temp;

	int state;

	int POWER;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //STEP 1. Configure cache, wait states and peripheral bus clock
	SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // STEP 2. configure the port registers
    mPORTDSetPinsDigitalOut(BIT_0 | BIT_1 | BIT_2 | BIT_9);
	mPORTESetPinsDigitalOut(BIT_0 | BIT_1 | BIT_2);

    //PORTSetPinsDigitalIn(IOPORT_D, BIT_6);
	

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // STEP 3. initialize the port pin states = outputs low
    mPORTDClearBits(BIT_0 | BIT_1 | BIT_2);
	mPORTEClearBits(BIT_0);

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

	// set up the core software interrupt with a prioirty of 3 and zero sub-priority
    INTSetVectorPriority(INT_CORE_SOFTWARE_0_VECTOR, INT_PRIORITY_LEVEL_3);
    INTSetVectorSubPriority(INT_CORE_SOFTWARE_0_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTClearFlag(INT_CS0);
    INTEnable(INT_CS0, INT_ENABLED);

    // set up the core software interrupt with a prioirty of 4 and zero sub-priority
    INTSetVectorPriority(INT_CORE_SOFTWARE_1_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_CORE_SOFTWARE_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTClearFlag(INT_CS1);
    INTEnable(INT_CS1, INT_ENABLED);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// STEP 8. configure SPI port and MCP3909
	SpiChnOpen(2, SPI_OPEN_MSTEN | SPI_OPEN_MSSEN | SPI_OPEN_SMP_END | SPI_OPEN_FRMEN | SPI_OPEN_FSP_IN | SPI_OPEN_FSP_HIGH | SPI_OPEN_MODE32 | SPI_OPEN_SIDL, 4);
	SpiChnEnable(2,1);
	DBPRINTF("openspi1... \n");
	//ConfigIntSPI1(SPI_TX_INT_DIS | SPI_RX_INT_DIS | SPI_INT_PRI_2);
	DBPRINTF("configintspi1... \n");
	CONFIG_3909();
	
	DBPRINTF("Begin Relay Toggling.... \n");


   while(1)
   {
		// Toggle LED's to signify code is looping
		mPORTDToggleBits(BIT_2);     // toggle LED2 (same as LATDINV = 0x0004)
		DBPRINTF("GREEN BLINK... \n");

		CHANNEL_1_STATE = 1;
		CHANNEL_2_STATE = 0;

		SET_CHANNEL_STATE(CHANNEL_1_STATE, CHANNEL_2_STATE);
		DelayMs(500000);

		CHANNEL_1_STATE = 1;
		CHANNEL_2_STATE = 1;

		SET_CHANNEL_STATE(CHANNEL_1_STATE, CHANNEL_2_STATE);
		DelayMs(500000);

		CHANNEL_1_STATE = 0;
		CHANNEL_2_STATE = 1;

		SET_CHANNEL_STATE(CHANNEL_1_STATE, CHANNEL_2_STATE);
		DelayMs(500000);

		CHANNEL_1_STATE = 0;
		CHANNEL_2_STATE = 0;

		SET_CHANNEL_STATE(CHANNEL_1_STATE, CHANNEL_2_STATE);
		DelayMs(500000);



/*
		POWER = 0;

		POWER = MEASURE_POWER(2);
		
		if (POWER > 0)
		{
			DBPRINTF("positive power reading \n");
		}
		if (POWER < 0)
		{
			DBPRINTF("negative power reading \n");
		}
		if (POWER == 0)
		{
			DBPRINTF("power reading failed \n");
		}

*/

   };

}




	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // STEP 9. configure the CN interrupt handler
void __ISR(_CHANGE_NOTICE_VECTOR, ipl2) ChangeNotice_Handler(void)
{
	DBPRINTF("INTERRUPT!!! \n");
	unsigned int temp1;
	unsigned int temp2;

    // clear the mismatch condition
    temp1 = PORTReadBits(IOPORT_D, BIT_6);
	temp2 = PORTReadBits(IOPORT_D, BIT_7);
	
	// clear the interrupt flag
    mCNClearIntFlag();
	
	if (temp1 == 0)
	{
		CHANNEL_1_STATE = 1;
		mPORTDSetBits(BIT_0);
		mPORTESetBits(BIT_1);
		DBPRINTF("CHANNEL 1 ON \n");
	}
	else
	{
		CHANNEL_1_STATE = 0;
		mPORTDClearBits(BIT_0);
		mPORTEClearBits(BIT_1);
		DBPRINTF("CHANNEL 1 OFF \n");
	}

	if (temp2 == 0)
	{
		CHANNEL_2_STATE = 1;
		mPORTDSetBits(BIT_1);
		mPORTESetBits(BIT_2);
		DBPRINTF("CHANNEL 2 ON \n");
	}
	else
	{
		CHANNEL_2_STATE = 0;
		mPORTDClearBits(BIT_1);
		mPORTEClearBits(BIT_2);
		DBPRINTF("CHANNEL 2 OFF \n");
	}

}




/******************************************************************************
*	DelayMs()
*
*	This functions provides a software millisecond delay
******************************************************************************/
void DelayMs(unsigned int msec)
{
	unsigned int tWait, tStart;
		
    tWait=(SYS_FREQ/20000000)*msec;
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
		mPORTDSetBits(BIT_0);
		mPORTESetBits(BIT_1);
		DBPRINTF("CHANNEL 1 ON \n");
	}
	if (state_1 == 0)
	{
		mPORTDClearBits(BIT_0);
		mPORTEClearBits(BIT_1);
		DBPRINTF("CHANNEL 1 OFF \n");
	}

	// set state of channel 2
	// **********************
	if (state_2 == 1)
	{
		mPORTDSetBits(BIT_1);
		mPORTESetBits(BIT_2);
		DBPRINTF("CHANNEL 2 ON \n");
	}
	if (state_2 == 0)
	{
		mPORTDClearBits(BIT_1);
		mPORTEClearBits(BIT_2);
		DBPRINTF("CHANNEL 2 OFF \n");
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
	if (channel == 2)
		{
			return CHANNEL_2_STATE;
		}
	if ((channel != 1) && (channel != 2))
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
		//CHANNEL_1_POWER = MEASURE(1);
		CHANNEL_1_POWER = 5;
	}

	// CHANNEL 2
	// *********
	if (CHANNEL_2_STATE == 0)
	{
		CHANNEL_2_POWER == 0;
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
	// local variable declarations
	int MASK = 0x0000FFFF;
	int num_samples = 128;
	int Csamples[num_samples];
	int Vsamples[num_samples];
	int Power = 0;
	int i = 0;

	DBPRINTF("sample....\n");
	int data;
	int samples[num_samples];

	// get data
	for (i=0 ; i<num_samples ; i++)
	{
		data = SpiChnGetC(channel);
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

	// SPI mode code
	int code = 0b10100100;
	
	// set CS high
	mPORTDSetBits(BIT_9);
	// set MCLR low 
	mPORTEClearBits(BIT_0);
	// delay
	DelayMs(10000);
	// set MCLR high
	mPORTESetBits(BIT_0);
	// set CS low
	mPORTDClearBits(BIT_9);

	DelayMs(3);

	// feed SPI mode code to MCP3909
	SpiChnPutC(1, 0b10100100);
	
	mPORTDSetBits(BIT_9);

	DelayMs(5000);

	int txdata = 0;
	SpiChnPutC(1, txdata);
}






/*****************************************************************************
*	SAMPLE(int channel)
*
*	This function reads the digitally sampled voltage and current readings
*	from the MCP3909 via the SPI port
*****************************************************************************
int SAMPLE(int channel)
{
	DBPRINTF("sample....\n");
	int data;
	int samples[num_samples];

	// get data
	for (int i = 0 ; i < num_samples ; i++)
	{
		data = SpiChnGetC(channel);
		samples[i] = data;
	}

	/*
	char * c;
	char a = (char)data;
	c = &a;
	DBPUTC(c);
	DBPRINTF("\n");
	data = data & 0x7FFFFFFF;


	DBPRINTF("sample....\n");
	data = SpiChnGetC(channel);
	a = (char)data;
	c = &a;
	DBPUTC(c);
	DBPRINTF("\n");
	*

	return samples;
}
*/



