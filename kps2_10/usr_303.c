/*
 * usr_303.c - led interface to CANopen Library
 *
 * Copyright (c) 2005 port GmbH Halle(Saale)
 *------------------------------------------------------------------
 * $Header: /z2/cvsroot/library/co_lib/examples/template/usr_303.c,v 1.4 2007/10/09 14:03:02 boe Exp $
 *
 *------------------------------------------------------------------
 *
 * modification history
 * --------------------
 * $Log: usr_303.c,v $
 * Revision 1.4  2007/10/09 14:03:02  boe
 * adapt for redundancy usage
 *
 * Revision 1.3  2006/08/23 07:21:26  ro
 * ledInd() updated
 *
 * Revision 1.2  2006/02/08 16:14:36  boe
 * Library V4.4
 *
 * Revision 1.1  2004/05/19 09:18:15  boe
 * template files
 *
 *
 *------------------------------------------------------------------
 */


/**
* \file usr_303.c
*++ Callback functions for DS303-3 (LED indicator) services
*-- Anwenderfunktionen für DS303-3 (LED indicator) Dienste
* \author port GmbH, Halle
+ $Revision: 1.4 $
* $Date: 2007/10/09 14:03:02 $
*
*-- Diese Datei enthält Funktionen für die Dienste,
*-- die im Standard DS303 definiert wurden.
*++ This file contains special indication functions for services 
*++ described in the standard DS303.
*/

#include <stdio.h>

#include <cal_conf.h>
#include <co_led.h>
#include <dave\IO.H>

#ifdef CONFIG_CO_LED
/********************************************************************/
/**
*
*-- \brief ledInd - schaltet die CANopen LED an bzw aus
*++ \brief ledInd - switch CANopen indicator on or off
*
*-- Diese Funktion dient zum Setzen (Ein/Ausschalten) der CANopen LEDs.
*-- Sie wird von der Bibliothek gemäss dem
*-- NMT-Zustand bzw. Fehlerzustand aufgerufen.
*-- Der Parameter led zeigt an, welche der beiden LEDs
*--\code
*-- 	CO_ERR_LED	Fehler LED
*-- 	CO_RUN_LED	Status LED
 --\endcode
*-- betätigt werden soll.
*
*-- Der einzustellende Zustand für diese LED
*--\code
*-- 	CO_LED_ON	LED einschalten
*-- 	CO_LED_OFF	LED ausschalten
 --\endcode
*-- wird über den Parameter action übergeben.
*
*
*++ This function switches the CANopen LEDs on or off.
*++ It is called from the library according to the
*++ NMT-state and error state, respectively.
*++ The parameter led determines which of the two LEDs to act on.
*++\code
*++ 	CO_ERR_LED	Error LED
*++ 	CO_RUN_LED	NMT LED
*++\endcode
*
*++ The state for this LED is handed over
*++\code
*++ 	CO_LED_ON	turn LED on
*++ 	CO_LED_OFF	turn LED off
*++\endcode
*++ within the parameter action.

* \return 
*-- nichts
*++ nothing
*/

void ledInd (
	UNSIGNED8 led,		/**< which CANopen LED */
	UNSIGNED8 action	/**< turn LED on or off */
	CO_COMMA_REDCY_PARA_DECL
    )
{
    if (led == CO_ERR_LED) {
	if (action == CO_LED_ON) {
	    /* switch Error LED on */
		IO_vResetPin(IO_P0_0_LED_ERR);
	} else {
	    /* switch Error LED off */
		IO_vSetPin(IO_P0_0_LED_ERR);
	}
    }
    if (led == CO_RUN_LED) {
	if (action == CO_LED_ON) {
	    /* switch Status LED on */
		IO_vResetPin(IO_P0_1_LED_RUN);
	} else {
	    /* switch Status LED off */
		IO_vSetPin(IO_P0_1_LED_RUN);
	}
    }
}
#endif /* CONFIG_CO_LED */
