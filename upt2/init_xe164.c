/*
 * init_xe164 - Initialize the XE164 hardware
 *
 * Copyright (c) 2008 port GmbH Halle/Saale
 *------------------------------------------------------------------
 * $Header: /z2/cvsroot/library/co_lib/drivers/xe164/init_xe164.c,v 1.5 2008/07/24 14:07:24 boe Exp $
 *
 *--------------------------------------------------------------------------
 *
 *
 * modification history
 * --------------------
 * $Log: init_xe164.c,v $
 * Revision 1.5  2008/07/24 14:07:24  boe
 * add multiline and redundancy support
 * usable with BASIC and FULL-CAN mode
 *
 * Revision 1.4  2008/02/15 11:16:52  ro
 * printf implemented
 *
 * Revision 1.3  2008/01/17 13:40:41  ro
 * small changes for EVA
 *
 * Revision 1.2  2008/01/16 16:33:29  ro
 * Prototype for Init_CAN() in EVA mode added
 * Init for Uart added
 *
 * Revision 1.1  2008/01/14 09:29:10  ro
 * first beta XE164/UConnect
 *
 *
 *
 *
 *--------------------------------------------------------------------------
 */


/**
*  \file init_xe164.c
*  \author port GmbH Halle (Saale)
*  $Revision: 1.5 $
*  $Date: 2008/07/24 14:07:24 $
*
*++ This module contains the functions requiered to initialize the
*++ hardware components, i.e. processor with chip selects and timings
*++ or peripheral componets
*-- Dieses Modul enthält Funktionen zur Initialisierung der 
*-- Hardwarekomponenten, z.B. Prozessor mit Chip Select und Timings
*-- oder Peripherie
*
*/


/* header of standard C - libraries */
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include <ctype.h>
#include <stdlib.h>


#define DEF_HW_PART
#include <cal_conf.h>

#include <co_def.h>
#include <co_drv.h>
#include <co_drvif.h>

#ifdef CONFIG_EVA_VERSION
#else /* CONFIG_EVA_VERSION */
/* for Init_CAN() */
# include <can_multican.h>
#endif /* CONFIG_EVA_VERSION */

/* for setOptions() */
#include <environ.h>
#include <examples.h>

/* local defined data types
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/
#ifdef CONFIG_EVA_VERSION
/* Timer - cpu_xe166.c */
UNSIGNED16 CO_CONST coTimerPulse = CONFIG_TIMER_INC; /**< length of 1 tick */
#endif /* CONFIG_EVA_VERSION */

extern UNSIGNED8  lNodeId CO_LINE_PARA_ARRAY_DEF; /* local NodeId */
extern UNSIGNED16 bitRate CO_LINE_PARA_ARRAY_DEF; /* CAN default bitrate */

/* constant definitions
---------------------------------------------------------------------------*/
/* The values within the array are the addresses to the CAN Controller.*/
static UNSIGNED8 CO_MEM_CAN * CO_CONST str_can_addr CO_REDCY_PARA_ARRAY_DEF = CONFIG_CAN_ADDR;
 
/* list of external used functions, if not in headers
---------------------------------------------------------------------------*/
#ifdef CONFIG_EVA_VERSION
UNSIGNED8 Init_CAN(
    UNSIGNED8 CO_MEM_CAN * pBaseAddr, /**< pointer to address of CAN-Controller */
    UNSIGNED8 node, /**< CAN Node 0..8 */
    UNSIGNED16 wBaudRate       /**< bitrate (e.g. 50 = 50kBaud) */
    );

#endif	/* CONFIG_EVA_VERSION */

#if defined(CONFIG_NO_PRINTF) || defined(CONFIG_NO_PRINTF_INIT)
#else /* CONFIG_NO_PRINTF */
/*void ASC0_vInit(void);*/ /* asc0.c */

/* DAVE generated files - adapt for our needs */
/* void U0C0_ASC_vInit(void); */
extern void USIC0_vInit(void);
extern void U0C0_ASC_vSendData(UNSIGNED16 uwData);
#endif /* CONFIG_NO_PRINTF */

extern void DAVE_vInit(void);
extern void DAVE_vUnlockProtecReg(void);
extern void DAVE_vLockProtecReg(void);

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/

#ifdef CONFIG_RCS_IDENT
static char rcs_ident[] = "$Id: init_xe164.c,v 1.5 2008/07/24 14:07:24 boe Exp $";
#endif

/* GPT12E Kernel State Con. Register */
//#define GPT12E_KSCCFG          (*((unsigned short volatile *) 0xFE1C))


/***************************************************************************/
/**
*++ iniDevice - does all at boot up neccessarry initializations
*-- iniDevice - führt alle Bootup Initialisierungen durch.
*
*
* \returns 0
*++ if all was OK
*-- OK
* \returns -1
*++ if somthing coul not initialized
*-- Fehler, etwas konnte nicht initialisiert werden
*
* INTERNAL
*/

UNSIGNED8 iniDevice(void)
{
    DAVE_vInit();

    /* initialize interrupt functionality - all interrupts disabled */

    INIT_CPU_INTERRUPTS();
    
    /* init variables for examples */
#if defined(CONFIG_MULT_LINES) && !defined(CONFIG_REDUNDANCY_SUPPORT)
    {
    UNSIGNED8 canLine;

	for (canLine = 0; canLine < CONFIG_MULT_LINES; canLine++) {
	    /* initialize variables */
	    GL_ARRAY(bitRate) = 125;
	}
    }
#else
    /* initialize variables */
    GL_ARRAY(bitRate) = 125;
#endif /* CONFIG_MULT_LINES */


    /*
    
    -----------------------------------------------------------------------
    Configuration of Kernel State Configuration Register:
    -----------------------------------------------------------------------
    - Enable the CAN module(MODEN)
    - Enable Bit Protection for MODEN

    */
    
    DAVE_vUnlockProtecReg();
    
    CANSFR_KSCCFG =  0x0003; /* load Kernel State Configuration Register */
    GPT12E_KSCCFG  =  0x0003; /* Module Enable */

# if defined(CONFIG_NO_PRINTF) || defined(CONFIG_NO_PRINTF_INIT)
# else
    /* init ASC0 for printf() to 115200 Baud */
    USIC0_vInit();
# endif 

    NOP();  /* one cycle delay */

    NOP();  /* one cycle delay */

    DAVE_vLockProtecReg();

    return 0;
}



/***************************************************************************/
/*
* Callback for Init_CAN() from MultiCAN driver
*
* Currently this function implement only Node 1.
* Initialize CAN RX,TX Pin !
*/

void initCanPins(
	UNSIGNED8 node
    )
{
    if (node == 0) {
        /* CAN Node 1 Pins 2.5/2.6 -
         * for set CAN_NPCR1 the CAN must be im configuration mode
         */
        CAN_NPCR0 = 3;  // P2.6 Rx = receive input D

        //  Configuration of the used CAN Port Pins: 
        // I/O PIN Configuration for CAN 0
        
        P2_IOCR06  = 0x0020;   // P2.6 as input (pull up)
        P2_IOCR05  = 0x00A0;   // P2.5 as output (ALT1, push pull)
    }


    if(node == 1) {
	/* CAN Node 1 Pins 0.4/0.6 - 
	 * for set CAN_NPCR1 the CAN must be im configuration mode 
	 *
	 * -> UCONNECT Settings
	 */
	P0_IOCR04 = 0;
	P0_IOCR06 = 0x00a0;
	CAN_NPCR1 = 0x0001;
    }
}

/***************************************************************************/
/**
*++ initCan - initializes the CAN controller
*-- initCAN - initialisiert den CAN Kontroller
*
*++ This is a wrapper.
*++ It really calls the init function from the can driver source.
*-- Diese Funktion ruft die init Funktion vom CAN Treiber Source auf.
*
*/

UNSIGNED8 initCan(
	UNSIGNED16 baud /**< bitrate */
	CO_COMMA_REDCY_PARA_DECL
    )
{
UNSIGNED8 ret;

    INIT_CAN_INTERRUPTS( CO_REDCY_PARA );
    
#if defined(CONFIG_MULT_LINES) || defined(CONFIG_REDUNDANCY_SUPPORT)
    if (canLine == 0)  {
	ret = Init_CAN(str_can_addr CO_REDCY_PARA_ARRAY_INDEX, \
	    CONFIG_CAN_MULTICAN_NODE_LINE0 /*node*/,\
	    baud CO_COMMA_REDCY_PARA);
    } else {
	ret = Init_CAN(str_can_addr CO_REDCY_PARA_ARRAY_INDEX, \
	    CONFIG_CAN_MULTICAN_NODE_LINE1 /*node*/,\
	    baud CO_COMMA_REDCY_PARA);
    }
#else /* defined(CONFIG_MULT_LINES) || defined(CONFIG_REDUNDANCY_SUPPORT) */
    ret = Init_CAN(str_can_addr CO_REDCY_PARA_ARRAY_INDEX, \
    	CONFIG_CAN_MULTICAN_NODE /*node*/,\
	baud CO_COMMA_REDCY_PARA);
#endif /* defined(CONFIG_MULT_LINES) || defined(CONFIG_REDUNDANCY_SUPPORT) */

    return ret;
}


/***************************************************************************/
/**
*++ endLoop - Check 'end' of Application
*-- endLoop - prüft das Ende der Applikation
*
*
* \returns
* TRUE
*++ end criteria reached
*-- Ende Kriterium eingetreten
* \returns
* \FALSE
*++ end criteria Not reached
*-- Ende Kriterium nicht eingetreten.
*
* INTERNAL
*/

BOOL_T endLoop(void)
{
    return CO_FALSE;
}

/***************************************************************************/
/**
*++ setOptions - set options from cmd line
*-- setOptions - setzt Optionen der cmd Zeile
*
*++ Sets a output filepointer to a file or to stdout
*-- Setzt einen Ausgabezeiger auf eine Datei oder nach stdout.
*
* \returns
* TRUE
*++ success
*-- OK
* \returns
* FALSE
*++ failed
*-- Fehler
*
* INTERNAL
*/

BOOL_T setOptions(CO_EXAMPLE_ARGS_DECL)
{
    return(CO_TRUE);
}


/*______________________________________________________________________EOF_*/
