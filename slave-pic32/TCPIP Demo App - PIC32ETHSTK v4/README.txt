PIC32 Ethernet Starter Kit
--------------------------

This file contains the following sections:
1. Code Example Description
2. Folder Contents
3. Required Development Resources

NOTE:
THIS DEMO WORKS ONLY WITH THE PIC32 ETHERNET STARTER KIT.
THE PRE-COMPILED LIBRARY USED IN THIS DEMO WAS SOURCED FROM THE MICROCHIP APPLICATIONS LIBRARY (MAL). 
IF YOU ARE INTERESTED IN DEVELOPING WITH THE STACK RATHER THAN USING THIS PRECOMPILED LIBRARY, 
PLEASE USE THE LATEST SOURCE FILES PROVIDED IN THE MICROCHIP APPLICATIONS LIBRARY FOR TCPIP STACK AND CODE EXAMPLES.


1. Code Example Description:
----------------------------
This demo code will run on the PIC32 Ethernet Starter Kit. It hosts a HTTP server when connected to a network. To view the webpage hosted by the demo application open a web browser, type http://mchpboard in the address bar and hit the enter key.

After downloading the Microchip TCPIP demo and installing the development tools, please use the following procedure to run the included demo code:

1. Load the Microchip TCPIP demo code into MPLAB by double clicking the *.mcp project file.
2. Connect the mini-B debugger port on-board the PIC32 Ethernet Starter board to an USB port on the development computer using the USB cable provided in the kit.
3. Connect the RJ-45 ethernet port on the PIC32 Ethernet Starter board to a network hub or an ethernet port on the development computer using the ethernet patch cord provided in the kit.
4. Choose the PIC32 Starter Kit debugger tool in MPLAB IDE by selecting Debugger>Select Tool and then click on PIC32 Starter Kit.
5. Build the project by selecting Project>Build All.
6. Download your code into the evaluation board microcontroller by selecting Debugger>Programming>Program All Memories.
7. Run the demo code included in the Microchip TCPIP stack previously downloaded by selecting Debugger>Run.
8. A HTTP server is hosted by the demo application. Open a web browser, type http://mchpboard in the address bar and hit the enter key. 
   The demo application features following:
	a.)Real-time hardware control and Dynamic Variables - On the Overview page the the LEDs can be clicked to toggle LEDs on the Ethernet Starter Board. The buttons on Ethernet Starter Board can be pressed	to see the Buttons on the webpage toggle. The dynamic variables can be updated in real time on the HTTP server.
	b.)Form Processing - Input can be handled from the client by using GET and POST methods. 
	c.)Authentication - Shows an example to restricted access feature commonly used.
	d.)File Uploads - Shows an example of file upload using POST method. The HTTP server can accept a user defined MPFS/MPFS2 image file for webpages. 
	e.)Board Configuration - MAC address, host name and IP address of the PIC32 Ethernet starter board can be viewed in the Network Configuration page and some configuration can be updated.


2. Folder Contents:
-------------------
This project folder contains the following:

a. Application source files

b. TCPIP Library PIC32MX795F512L.a
This archive file is the pre-compiled TCPIP stack.

c. Include
This folder contains include and header files for the code example.


3. Required Development Resources:
-----------------------------------
a. PIC32 Ethernet Starter Kit.
b. MPLAB IDE.
c. C Compiler.