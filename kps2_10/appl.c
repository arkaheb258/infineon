/*
 * appl - CANopen example application 'demo' code
 *
 * Copyright (c) 2005 port GmbH Halle
 *------------------------------------------------------------------
 * $Header: /z2/cvsroot/library/co_lib/examples/s1/appl.c,v 1.12 2006/05/05 15:04:22 ro Exp $
 *
 *------------------------------------------------------------------
 *
 * modification history
 * --------------------
 * (not supported with examples)
 *
 *
 *------------------------------------------------------------------
 */

/**
*  \file appl.c
*  \author port Gmbh, Halle (Saale)
*  $Revision: 1.12 $
*  $Date: 2006/05/05 15:04:22 $
*
* This file contains the handling of the "digital output" ports.
* The two indicies 0x6200 and 0x6202 are handled with two sub-indicies
* each. The changed values are printed to stdout (using PRINTF()).
* Try to chnge this to control real hardware. Have Fun.
*
*/

/* header of standard C - libraries */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* header of project specific types 
*  Enable hardware specific part for SET_BIT(), RESET_BIT() definition.
*/
#define DEF_HW_PART
#include <cal_conf.h>      /* !!! first file to include for all CANopen */

#include <co_acces.h>
#include <co_sdo.h>
#include <co_pdo.h>

#include <examples.h>      /* enable/disable PRINTF */

#include "objects.h"            /* object dictionary */

/* constant definitions
---------------------------------------------------------------------------*/


/* For the example exists two variants of a macro to print
 * out return values of CANopen system functions.
 * First for small embedded systems, which only puts a '0' (error free)
 * or '1'.
 * Second for systems with a large console capable of printing text.
 */
#ifdef NO_PRINTF_S
# define PRINTRET(s, e) do {						\
			    if ((e) == 0) { PUTCHAR('0'); }		\
			    else { PUTCHAR('1'); err = CO_TRUE; }	\
			  } while(0);
#else
# define PRINTRET(s, e) do {						\
			    PRINTF(s, e);				\
			    if((e) != 0) err = CO_TRUE;			\
			  } while(0);
#endif

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
---------------------------------------------------------------------------*/

/* list of global defined functions
---------------------------------------------------------------------------*/

void set_outputs(int port);

/* list of local defined functions
---------------------------------------------------------------------------*/

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/


/* local defined variables
---------------------------------------------------------------------------*/

#ifdef CONFIG_RCS_IDENT
static char _rcsid[] = "$Id: appl.c,v 1.12 2006/05/05 15:04:22 ro Exp $";
#endif

/********************************************************************/
/**
*
* \brief set_outputs - indicate the occurence of a SDO write access
*
* This function is called if an SDO write request
* for the digital outputs at 0x6200 reaches the CANopen SDO server,
* or an PDO with these objects mapped is received.
*
* If no hardware is available, th port value is only printed to stdou
* using PRINTF().
*
* The parameter port is enough. It is related to the written Subindex
* of 0x6200. The values are taken from the object directory.
*
* \returns
* Nothing
*
*/


void set_outputs(int port)
{
UNSIGNED8	val = 0;

//	val = p401_write_state_8[port] ^ p401_polarity_write_8[port];
	/* p401_polarity_write_8 */
	PRINTF("==> Write %3d/%02x to port %d\n", (int)val, (int)val, port);
}
