/*
 * cpu.c - cpu driver for XC164 device
 *
 * Copyright (c) 2007-2008 port GmbH Halle/Saale
 *------------------------------------------------------------------
 * $Header: /z2/cvsroot/library/co_lib/drivers/xe164/cpu.c,v 1.4 2008/02/15 11:11:49 ro Exp $
 *
 *------------------------------------------------------------------
 *
 * modification history
 * --------------------
 * 
 * $Log: cpu.c,v $
 * Revision 1.4  2008/02/15 11:11:49  ro
 * small changes
 *
 * Revision 1.3  2008/01/17 13:40:40  ro
 * small changes for EVA
 *
 * Revision 1.2  2008/01/16 16:32:35  ro
 * co_stru.h added
 *
 * Revision 1.1  2008/01/14 09:29:09  ro
 * first beta XE164/UConnect
 *
 *
 *
 *
 *
 *
 *------------------------------------------------------------------
 */

/**
*  \file cpu.c
*  \author port GmbH Halle (Saale)
*  $Revision: 1.4 $
*  $Date: 2008/02/15 11:11:49 $
*
*
*/

#ifdef CONFIG_RCS_IDENT
static char rcsid[] = "$Id: cpu.c,v 1.4 2008/02/15 11:11:49 ro Exp $";
#endif

/* includes  common c libraries */
#include <stdio.h>

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define DEF_HW_PART
#include <cal_conf.h>

#include <co_def.h>
#include <co_stru.h>
#include <co_flag.h>

#ifdef CONFIG_EVA_VERSION
# define GL_DRV_ARRAY(var) var
#else /* CONFIG_EVA_VERSION */
# include <cdriver.h>
# include <can_multican.h>
#endif

/* includes hardware libraries 
---------------------------------------------------------------------------*/

/* externals 
---------------------------------------------------------------------------*/

/* globals 
---------------------------------------------------------------------------*/

/** counter of disable/restore interrupt calls */
UNSIGNED8 cocnt_canInt CO_REDCY_PARA_ARRAY_DEF;

	/* CAN Node 1 Interrupt Pointer Register */
#define CAN_0IC 	(*((unsigned short volatile *) 0xF140))

/*******************************************************************/
/**
*++ \brief Init_CAN_Interrupts - initialize the CAN interrupt handling
*-- \brief Init_CAN_Interrupts - Initialisiert das CAN Interrupt Handling
*
*++ This function disables the CAN interrupts and resets the counter
*++ \em cocnt_canInt to 1.
*-- Diese Funktion deaktiviert alle CAN Interrupte 
*-- und setzt den Zähler \em cocnt_canInt auf 1.
*
* \retval
*++ nothing
*-- nichts
*/

void Init_CAN_Interrupts(
    CO_REDCY_PARA_DECL
     )
{
    CAN_0IC = 0x0004u; /* Level */
    /*Disable_CAN_Interrupts(CO_REDCY_PARA);*/
    GL_DRV_ARRAY(cocnt_canInt) = 1;
}

/*******************************************************************/
/**
*
*++ \brief Enable_CAN_Interrupts - enables the CAN-controller interrupt
*-- \brief Enable_CAN_Interrupts - gibt CAN-Controller interrupt frei
*
*++ Enables the interrupt for the CAN-controller.
*++ The disable/enable counter is reset.
*-- Schaltet den Interrupt für den CAN-Controller aktiv.
*-- Ausserdem wird der Zähler für die Disable-Aufrufe gelöscht.
*
* \returns
*++ nothing
*-- nichts
*
*/
void Enable_CAN_Interrupts(
    CO_REDCY_PARA_DECL
     ) 
{

    GL_DRV_ARRAY(cocnt_canInt) = 0;

    CAN_0IC |= 0x0040u;

}


/*******************************************************************/
/**
*
*++ \brief Disable_CAN_Interrupts - disables the CAN-controller interrupt
*-- \brief Disable_CAN_Interrupts - sperrt CAN-Controller interrupt
*
*++ Disables the interrupt for the CAN-controller.
*++ Additionally the counter for disabling calls is incremented.
*-- Schaltet den Autovector-Interrupt für den CAN-Controller deaktiv.
*-- Ausserdem wird der Zähler für die Disable-Aufrufe erhöht.
*
* \returns
*++ nothing
*-- nichts
*
*/

void Disable_CAN_Interrupts(
    CO_REDCY_PARA_DECL
     )
{

    CAN_0IC &= ~0x0040u;

    GL_DRV_ARRAY(cocnt_canInt) ++;
}


/*******************************************************************/
/**
*++ \brief Restore_CAN_Interrupts - restore the CAN-Controller interrupt
*-- \brief Restore_CAN_Interrupts - restauriert den CAN-Controller interrupt
*
*-- Dekrementiert den Zähler für die Disable-Aufrufe.
*-- Wird dieser 0, wird der CAN-Interrupt freigegeben.
*++ Decrements the disable/enable counter.
*++ If this counter is 0 the CAN interrupt is enabled.
*
* \returns
*++ nothing
*-- nichts
*
*/

void Restore_CAN_Interrupts(
    CO_REDCY_PARA_DECL
     )
{
    if( GL_DRV_ARRAY(cocnt_canInt) > 1 ) {
	GL_DRV_ARRAY(cocnt_canInt) --;
    } else {	
	Enable_CAN_Interrupts( CO_REDCY_PARA );
    }	
}


/*******************************************************************/
/**
*
*++ SetIntMask - installs the ISR for the CAN controller
*-- SetIntMask - installiert die ISR für den CAN-Controller
*
*++ Commonly this function installs the ISR for the CAN controller.
*++ For this target this is done by the compiler.
*-- Standardmäßig installiert diese Funktion die ISR des CAN-Controllers.
*-- Für dieses Zielsystem übernimmt der Compiler diesen Teil.
*
* \returns
*++ nothing
*-- nichts
*
*/

void SetIntMask(
	void
	)
{

}

/*******************************************************************/
/**
*
*++ ResetIntMask - deinstalls the ISR for the CAN controller
*-- ResetIntMask - deinstalliert die ISR für den CAN-Controller
*
* \returns
*++ nothing
*-- nichts
*
*/

void ResetIntMask(
	void
	)
{

}

/*******************************************************************/
/*
* illegal word access
*/
#if defined(_C166) 
void _interrupt (0x0A) illegal_word_function(void)
#elif (defined(__TASKING__) && defined (__VX__))
void __interrupt (0x0A) illegal_word_function(void)
#else
void illegal_word_function(void) interrupt 0x0A using trap_reg
#endif
{

#pragma asm
	sbrk
#pragma endasm

    while(1);
}

