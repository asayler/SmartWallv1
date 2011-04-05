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
int CHANNEL1 = 0;
int CHANNEL2 = 0;

void SET_CHANNEL_STATE(int, int);
int GET_CHANNEL_STATE(int);


// prototype
void DelayMs(unsigned int);
void SAMPLE();
int MEASURE_POWER();
void CONFIG_3909();



int main(void)
{
	DBINIT();
	
	DBPRINTF("MAIN... \n");
   	unsigned int temp;

	int POWER;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //STEP 1. Configure cache, wait states and peripheral bus clock
	SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // STEP 2. configure the port registers
    mPORTDSetPinsDigitalOut(BIT_0 | BIT_1 | BIT_2);
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
    INTSetVectorPriority(INT_CORE_SOFTWARE_1_VECTOR, INT_PRIORITY_LEVEL_7);
    INTSetVectorSubPriority(INT_CORE_SOFTWARE_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTClearFlag(INT_CS1);
    INTEnable(INT_CS1, INT_ENABLED);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// STEP 8. configure SPI port and MCP3909
	//OpenSPI1(FRAME_ENABLE_OFF | ENABLE_SDO_PIN | SPI_MODE32_ON | SPI_SMP_ON | SPI_CKE_OFF | SLAVE_ENABLE_OFF | CLK_POL_ACTIVE_LOW | MASTER_ENABLE_ON , SPI_ENABLE | SPI_FRZ_CONTINUE | SPI_IDLE_STOP | SPI_RX_OVFLOW_CLR | FRAME_POL_ACTIVE_LOW | FRAME_SYNC_EDGE_COINCIDE);
	//DBPRINTF("openspi1... \n");
	//ConfigIntSPI1(SPI_FAULT_INT_EN | SPI_TX_INT_DIS | SPI_RX_INT_EN | SPI_INT_PRI_3);
	//DBPRINTF("configintspi1... \n");
	//CONFIG_3909();
	
	//DBPRINTF("Begin Relay Toggling.... \n");


   while(1)
   {
		// Toggle LED's to signify code is looping
		DelayMs(1000);
		mPORTDToggleBits(BIT_2);     // toggle LED2 (same as LATDINV = 0x0004)
		DBPRINTF("GREEN BLINK... \n");

		CHANNEL1 = 1;
		CoreSetSoftwareInterrupt0();
		DelayMs(5000);

		CHANNEL2 = 1;
		CoreSetSoftwareInterrupt0();
		DelayMs(5000);

		CHANNEL1 = 0;
		CoreSetSoftwareInterrupt0();
		DelayMs(5000);

		CHANNEL2 = 0;
		CoreSetSoftwareInterrupt0();
		DelayMs(5000);

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
		CHANNEL1 = 1;
		mPORTDSetBits(BIT_0);
		mPORTESetBits(BIT_1);
		DBPRINTF("CHANNEL 1 ON \n");
	}
	else
	{
		CHANNEL1 = 0;
		mPORTDClearBits(BIT_0);
		mPORTEClearBits(BIT_1);
		DBPRINTF("CHANNEL 1 OFF \n");
	}

	if (temp2 == 0)
	{
		CHANNEL2 = 1;
		mPORTDSetBits(BIT_1);
		mPORTESetBits(BIT_2);
		DBPRINTF("CHANNEL 2 ON \n");
	}
	else
	{
		CHANNEL2 = 0;
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
		
    tWait=(SYS_FREQ/2000)*msec;
    tStart=ReadCoreTimer();
    while((ReadCoreTimer()-tStart)<tWait);		// wait for the time to pass

}




/******************************************************************************
*	SET_CHANNEL_STATE()
*
*	This function sets the state of a specified channel
******************************************************************************/
void SET_CHANNEL_STATE(int channel, int state)
{
	if (channel == 1)
		{
			if (state == 0)
				{	
					CHANNEL1 = 0;
					mPORTEClearBits(BIT_1);
				}
			else
				{	
					CHANNEL1 = 1;	
					mPORTESetBits(BIT_1);
				}
		}
	if (channel == 2)
		{
			if (state == 0)
				{	
					CHANNEL2 = 0;
					mPORTEClearBits(BIT_2);
				}
			else
				{	
					CHANNEL2 = 1;	
					mPORTESetBits(BIT_2);
				}
		}
	if ((channel != 1) && (channel != 2))
		{
			DBPRINTF("IMPROPER CHANNEL SELECTION ERROR.  PLEASE TRY AGAIN. \n");
		}
}





/******************************************************************************
*	GET_CHANNEL_STATE()
*
*	This function returns the current state of a specified channel
******************************************************************************/
int GET_CHANNEL_STATE(int channel)
{
	if (channel == 1)
		{
			return CHANNEL1;
		}
	if (channel == 2)
		{
			return CHANNEL2;
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
*****************************************************************************
int MEASURE_POWER()
{
	// local variable declarations
	int MASK = 0x0000FFFF;
	int Csamples[num_samples];
	int Vsamples[num_samples];
	int Power = 0;
	int i = 0;

	// get array of samples from the MCP3909
	SAMPLE();
	
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

		return Power;
}





/*****************************************************************************
*	SAMPLE()
*
*	This function reads the digitally sampled voltage and current readings
*	from the MCP3909 via the SPI port
*****************************************************************************
void SAMPLE()
{
	DBPRINTF("sample....\n");

	// get data
	SpiChnGetS(1, (unsigned long*)cycle_sample_data, num_samples);
}





/*****************************************************************************
*	CONFIG_3909()
*
*	This function configures the MCP3909 to transmit data from its two
*	A/D converters to the PIC32 via the SPI port
*****************************************************************************
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

	SpiChnPutC(1, 0x00000000);
	
	int data;
	data = getcSPI1();
}



*/




/*********************************************************************
 * Function:       void _CoreSoftwareInt0Handler(void)
 *
 * PreCondition:    none
 *
 * Input:           none
 *
 * Output:          none
 *
 * Side Effects:    none
 *
 * Overview:        The interrupt handler function for the core software
 *                  interrupt
 *
 * Note:           The handler is placed directly into the vector location
 ********************************************************************/
void __ISR(_CORE_SOFTWARE_0_VECTOR, ipl3) _CoreSoftwareInt0Handler(void)
{
    INTClearFlag(INT_CS0);                      // clear the interrupt flag
    CoreClearSoftwareInterrupt0();              // clear the core int flag
    
	if (CHANNEL1 == 1)
	{
		mPORTDSetBits(BIT_0);
		mPORTESetBits(BIT_1);
		DBPRINTF("CHANNEL 1 ON \n");
	}
	if (CHANNEL1 == 0)
	{
		mPORTDClearBits(BIT_0);
		mPORTEClearBits(BIT_1);
		DBPRINTF("CHANNEL 1 OFF \n");
	}
	if (CHANNEL2 == 1)
	{
		mPORTDSetBits(BIT_1);
		mPORTESetBits(BIT_2);
		DBPRINTF("CHANNEL 2 ON \n");
	}
	if (CHANNEL2 == 0)
	{
		mPORTDClearBits(BIT_1);
		mPORTEClearBits(BIT_2);
		DBPRINTF("CHANNEL 2 OFF \n");
	}

}




/*********************************************************************
 * Function:       void _CoreSoftwareInt1Handler(void)
 *
 * PreCondition:    none
 *
 * Input:           none
 *
 * Output:          none
 *
 * Side Effects:    none
 *
 * Overview:        The interrupt handler fucntion for the core software
 *                  interrupt
 *
 * Note:            The jump to this handler will be placed in the vactor
 *                  location
 ********************************************************************
void __ISR(_CORE_SOFTWARE_1_VECTOR, ipl4) _CoreSoftwareInt1Handler(void)
{
    INTClearFlag(INT_CS1);                      // clear the interrupt flag
    CoreClearSoftwareInterrupt1();                // clear the core int flag
    PORTToggleBits(LED_PORT, CORE_SW_1_LED);
}

*/
