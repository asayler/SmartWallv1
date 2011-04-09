/*********************************************************************
 *
 *	Hardware specific definitions
 *
 *********************************************************************
 * FileName:        HardwareProfile.h
 * Dependencies:    None
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.10 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.34 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2010 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder		09/01/2010	Converted to a stub
 ********************************************************************/

// These definitions are set in the MPLAB Project settings.  If you are starting
// a new project, you should start by modifying one of the pre-existing .mcp 
// files.  To modify the macro used, in MPLAB IDE, click on Project -> Build 
// Options... -> Project -> MPLAB XXX C Compiler -> Preprocessor Macros -> 
// Add.... Note that you may also have to add this macro to the assembler 
// (MPLAB XXX Assembler tab).
#if defined(YOUR_BOARD)
	#include "Alternative Configurations/HardwareProfile YOUR_BOARD.h"
#elif defined(INTERNET_RADIO)
	// DM183033 Internet Radio board, PIC18F67J60, internal Ethernet
	#include "Alternative Configurations/HardwareProfile INTERNET_RADIO.h"
#elif defined(PICDEMNET2_INTERNAL_ETHERNET)
	// DM163024 PICDEM.net 2 board, PIC18F97J60, internal Ethernet (J1 Ethernet jack)
	#include "Alternative Configurations/HardwareProfile PICDEMNET2_INTERNAL_ETHERNET.h"
#elif defined(PICDEMNET2_ONBOARD_ENC28J60)
	// DM163024 PICDEM.net 2 board, PIC18F97J60, onboard ENC28J60 (J2 Ethernet jack)
	#include "Alternative Configurations/HardwareProfile PICDEMNET2_ONBOARD_ENC28J60.h"
#elif defined(PICDEMNET2_ENC624J600)
	// DM163024 PICDEM.net 2 board, PIC18F97J60, AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600)
	#include "Alternative Configurations/HardwareProfile PICDEMNET2_ENC624J600.h"
#elif defined(PICDEMNET2_MRF24WB0M)
	// DM163024 PICDEM.net 2 board, PIC18F97J60, AC164136-4 MRF24WB0MA Wi-Fi PICtail/PICtail Plus
	#include "Alternative Configurations/HardwareProfile PICDEMNET2_MRF24WB0M.h"
#elif defined(PIC18_EXPLORER_ENC28J60)
	// DM183032 PIC18 Explorer board, PIC18F8722/PIC18F87J50/PIC18F87J10/PIC18F87J11/possibly others, AC164121 Ethernet PICtail (ENC28J60)
	#include "Alternative Configurations/HardwareProfile PIC18_EXPLORER_ENC28J60.h"
#elif defined(PIC18_EXPLORER_ENC624J600)
	// DM183032 PIC18 Explorer board, PIC18F8722/PIC18F87J50/PIC18F87J10/PIC18F87J11/possibly others, AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600)
	#include "Alternative Configurations/HardwareProfile PIC18_EXPLORER_ENC624J600.h"
#elif defined(PIC18_EXPLORER_MRF24WB0M)
	// DM183032 PIC18 Explorer board, PIC18F8722/PIC18F87J50/PIC18F87J10/PIC18F87J11/possibly others, AC164136-4 MRF24WB0MA Wi-Fi PICtail/PICtail Plus
	#include "Alternative Configurations/HardwareProfile PIC18_EXPLORER_MRF24WB0M.h"
#elif defined(PIC24FJ256DA210_DEV_BOARD_ENC28J60)
	// DM240312 PIC24FJ256DA210 Development Board, PIC24FJ256DA210, AC164123 Ethernet PICtail Plus (ENC28J60)
	#include "Alternative Configurations/HardwareProfile PIC24FJ256DA210_DEV_BOARD_ENC28J60.h"

#elif defined(PIC24FJ256DA210_DEV_BOARD_ENC624J600)
	#if defined(PSP_5_INDIRECT_BITBANG)
		// DM240312 PIC24FJ256DA210 Development Board, PIC24FJ256DA210, AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in PSP Mode 5, Indirect addressing (using J1 side edge connector, PMA to AD = open, PMA to A = open, PSPCFG1&4 = GND, PSPCFG2 = GND, PSPCFG3 = 3V3)
		#include "Alternative Configurations/HardwareProfile PIC24FJ256DA210_DEV_BOARD_ENC624J600 PSP_5_INDIRECT_BITBANG.h"
	#else
		// DM240312 PIC24FJ256DA210 Development Board, PIC24FJ256DA210, AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in SPI mode (using J2 side edge connector)
		#include "Alternative Configurations/HardwareProfile PIC24FJ256DA210_DEV_BOARD_ENC624J600.h"
	#endif
#elif defined(PIC24FJ256DA210_DEV_BOARD_MRF24WB0M)
	// DM240312 PIC24FJ256DA210 Development Board, PIC24FJ256DA210, AC164136-4 MRF24WB0MA Wi-Fi PICtail/PICtail Plus
	#include "Alternative Configurations/HardwareProfile PIC24FJ256DA210_DEV_BOARD_MRF24WB0M.h"
#elif defined(EXPLORER_16_ENC28J60)
	// DM240001 Explorer 16 board, PIC24FJ128GA010/PIC24FJ256GB110/PIC24FJ256GA110/PIC24FJ256GB210/PIC24HJ256GP610/dsPIC33FJ256GP710/PIC32MX360F512L/PIC32MX460F512L/PIC32MX795F512L/possibly others, AC164123 Ethernet PICtail Plus (ENC28J60)
	#if defined(__C30__)
		#include "Alternative Configurations/HardwareProfile EXPLORER_16_ENC28J60 C30.h"
	#else
		#include "Alternative Configurations/HardwareProfile EXPLORER_16_ENC28J60 C32.h"
	#endif
#elif defined(EXPLORER_16_ENC624J600)
	// DM240001 Explorer 16 board, PIC24FJ128GA010/PIC24FJ256GB110/PIC24FJ256GA110/PIC24FJ256GB210/PIC24HJ256GP610/dsPIC33FJ256GP710/PIC32MX360F512L/PIC32MX460F512L/PIC32MX795F512L/possibly others, AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600)
	#if defined(__C30__)
		#if defined(PSP_5_INDIRECT)
			// AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in PSP Mode 5, Indirect addressing (using J1 side edge connector, PMA to AD = open, PMA to A = open, PSPCFG1&4 = GND, PSPCFG2 = GND, PSPCFG3 = 3V3)
			#include "Alternative Configurations/HardwareProfile EXPLORER_16_ENC624J600 PSP_5_INDIRECT C30.h"
		#elif defined(PSP_5_INDIRECT_BITBANG)
			// AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in PSP Mode 5, Indirect addressing (using J1 side edge connector, PMA to AD = open, PMA to A = open, PSPCFG1&4 = GND, PSPCFG2 = GND, PSPCFG3 = 3V3)
			#include "Alternative Configurations/HardwareProfile EXPLORER_16_ENC624J600 PSP_5_INDIRECT_BITBANG C30.h"
		#elif defined(PSP_9)
			// AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in PSP Mode 9 (using J1 side edge connector, PMA to AD = open, PMA to A = open, PSPCFG1&4 = GND, PSPCFG2 = 3V3, PSPCFG3 = 3V3)
			#include "Alternative Configurations/HardwareProfile EXPLORER_16_ENC624J600 PSP_9 C30.h"
		#else
			// AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in SPI mode (using J2 side edge connector)
			#include "Alternative Configurations/HardwareProfile EXPLORER_16_ENC624J600 C30.h"
		#endif
	#else
		#if defined(PSP_5_INDIRECT)
			// AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in PSP Mode 5, Indirect addressing (using J1 side edge connector, PMA to AD = open, PMA to A = open, PSPCFG1&4 = GND, PSPCFG2 = GND, PSPCFG3 = 3V3)
			#include "Alternative Configurations/HardwareProfile EXPLORER_16_ENC624J600 PSP_5_INDIRECT C32.h"
		#elif defined(PSP_9)
			// AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in PSP Mode 9 (using J1 side edge connector, PMA to AD = open, PMA to A = open, PSPCFG1&4 = GND, PSPCFG2 = 3V3, PSPCFG3 = 3V3)
			#include "Alternative Configurations/HardwareProfile EXPLORER_16_ENC624J600 PSP_9 C32.h"
		#else
			// AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in SPI mode (using J2 side edge connector)
			#include "Alternative Configurations/HardwareProfile EXPLORER_16_ENC624J600 C32.h"
		#endif
	#endif
#elif defined(EXPLORER_16_MRF24WB0M)
	// DM240001 Explorer 16 board, PIC24FJ128GA010/PIC24FJ256GB110/PIC24FJ256GA110/PIC24FJ256GB210/PIC24HJ256GP610/dsPIC33FJ256GP710/PIC32MX360F512L/PIC32MX460F512L/PIC32MX795F512L/possibly others, AC164136-4 MRF24WB0MA Wi-Fi PICtail/PICtail Plus
	#if defined(__C30__)
		#include "Alternative Configurations/HardwareProfile EXPLORER_16_MRF24WB0M C30.h"
	#else
		#include "Alternative Configurations/HardwareProfile EXPLORER_16_MRF24WB0M C32.h"
	#endif
#elif defined(PIC32_ENET_SK_DM320004_INTERNAL_ETHERNET)
	// DM320004 PIC32 Ethernet Starter Kit, PIC32MX795F512L, internal Ethernet (National DP83848 PHY)
	#include "Alternative Configurations/HardwareProfile PIC32_ENET_SK_DM320004_INTERNAL_ETHERNET.h"
#elif defined(PIC32_GP_SK_DM320001_ENC28J60)
	// DM320001 PIC32 Starter Kit (General Purpose), PIC32MX360F512L, DM320002 PIC32 I/O Expansion Board, AC164123 Ethernet PICtail Plus (ENC28J60)
	#include "Alternative Configurations/HardwareProfile PIC32_GP_SK_DM320001_ENC28J60.h"
#elif defined(PIC32_GP_SK_DM320001_ENC624J600)
	// DM320001 PIC32 Starter Kit (General Purpose), PIC32MX360F512L, DM320002 PIC32 I/O Expansion Board, AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600)
	#if defined(PSP_5_INDIRECT)
		// AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in PSP Mode 5, Indirect addressing (using J1 side edge connector, PMA to AD = open, PMA to A = open, PSPCFG1&4 = GND, PSPCFG2 = GND, PSPCFG3 = 3V3)
		#include "Alternative Configurations/HardwareProfile PIC32_GP_SK_DM320001_ENC624J600 PSP_5_INDIRECT.h"
	#elif defined(PSP_9)
		// AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in PSP Mode 9 (using J1 side edge connector, PMA to AD = open, PMA to A = open, PSPCFG1&4 = GND, PSPCFG2 = 3V3, PSPCFG3 = 3V3)
		#include "Alternative Configurations/HardwareProfile PIC32_GP_SK_DM320001_ENC624J600 PSP_9.h"
	#else
		// AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in SPI mode (using J2 side edge connector)
		#include "Alternative Configurations/HardwareProfile PIC32_GP_SK_DM320001_ENC624J600.h"
	#endif
#elif defined(PIC32_GP_SK_DM320001_MRF24WB0M)
	// DM320001 PIC32 Starter Kit (General Purpose), PIC32MX360F512L, DM320002 PIC32 I/O Expansion Board, AC164136-4 MRF24WB0MA Wi-Fi PICtail/PICtail Plus
	#include "Alternative Configurations/HardwareProfile PIC32_GP_SK_DM320001_MRF24WB0M.h"
#elif defined(PIC32_USB_DM320003_1_ENC28J60)
	// DM320003-1 PIC32 USB Starter Board (Kit), PIC32MX460F512L, DM320002 PIC32 I/O Expansion Board, AC164123 Ethernet PICtail Plus (ENC28J60) (installed in SPI 2 slot)
	#include "Alternative Configurations/HardwareProfile PIC32_USB_DM320003_1_ENC28J60.h"
#elif defined(PIC32_USB_DM320003_1_ENC624J600)
	// DM320003-1 PIC32 USB Starter Board (Kit), PIC32MX460F512L, DM320002 PIC32 I/O Expansion Board, AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600)
	#if defined(PSP_5_INDIRECT)
		// AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in PSP Mode 5, Indirect addressing (using J1 side edge connector, PMA to AD = open, PMA to A = open, PSPCFG1&4 = GND, PSPCFG2 = GND, PSPCFG3 = 3V3)
		#include "Alternative Configurations/HardwareProfile PIC32_USB_DM320003_1_ENC624J600 PSP_5_INDIRECT.h"
	#elif defined(PSP_9)
		// AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in PSP Mode 9 (using J1 side edge connector, PMA to AD = open, PMA to A = open, PSPCFG1&4 = GND, PSPCFG2 = 3V3, PSPCFG3 = 3V3)
		#include "Alternative Configurations/HardwareProfile PIC32_USB_DM320003_1_ENC624J600 PSP_9.h"
	#else
		// AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in SPI mode (using J2 side edge connector), installed in SPI 2 slot
		#include "Alternative Configurations/HardwareProfile PIC32_USB_DM320003_1_ENC624J600.h"
	#endif
#elif defined(PIC32_USB_DM320003_1_MRF24WB0M)
	// DM320003-1 PIC32 USB Starter Board (Kit), PIC32MX460F512L, DM320002 PIC32 I/O Expansion Board, AC164136-4 MRF24WB0MA Wi-Fi PICtail/PICtail Plus (installed in SPI 2 slot)
	#include "Alternative Configurations/HardwareProfile PIC32_USB_DM320003_1_MRF24WB0M.h"
#elif defined(PIC32_USB_SK_DM320003_2_ENC28J60)
	// DM320003-1 PIC32 USB Starter Kit II, PIC32MX795F512L, DM320002 PIC32 I/O Expansion Board, AC164123 Ethernet PICtail Plus (ENC28J60) (installed in SPI 2 slot)
	#include "Alternative Configurations/HardwareProfile PIC32_USB_SK_DM320003_2_ENC28J60.h"
#elif defined(PIC32_USB_SK_DM320003_2_ENC624J600)
	// DM320003-1 PIC32 USB Starter Kit II, PIC32MX795F512L, DM320002 PIC32 I/O Expansion Board, AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) (installed in SPI 2 slot)
	#if defined(PSP_5_INDIRECT)
		// AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in PSP Mode 5, Indirect addressing (using J1 side edge connector, PMA to AD = open, PMA to A = open, PSPCFG1&4 = GND, PSPCFG2 = GND, PSPCFG3 = 3V3)
		#include "Alternative Configurations/HardwareProfile PIC32_USB_SK_DM320003_2_ENC624J600 PSP_5_INDIRECT.h"
	#elif defined(PSP_9)
		// AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in PSP Mode 9 (using J1 side edge connector, PMA to AD = open, PMA to A = open, PSPCFG1&4 = GND, PSPCFG2 = 3V3, PSPCFG3 = 3V3)
		#include "Alternative Configurations/HardwareProfile PIC32_USB_SK_DM320003_2_ENC624J600 PSP_9.h"
	#else
		// AC164132 Fast 100Mbps Ethernet PICtail/PICtail Plus (ENC624J600) in SPI mode (using J2 side edge connector), installed in SPI 2 slot
		#include "Alternative Configurations/HardwareProfile PIC32_USB_SK_DM320003_2_ENC624J600.h"
	#endif
#elif defined(PIC32_USB_SK_DM320003_2_MRF24WB0M)
	// DM320003-1 PIC32 USB Starter Kit II, PIC32MX795F512L, DM320002 PIC32 I/O Expansion Board, AC164136-4 MRF24WB0MA Wi-Fi PICtail/PICtail Plus (installed in SPI 2 slot)
	#include "Alternative Configurations/HardwareProfile PIC32_USB_SK_DM320003_2_MRF24WB0M.h"
#else
	#error "No extended HardwareProfile .h included.  Add the appropriate compiler macro to the MPLAB project."
#endif

