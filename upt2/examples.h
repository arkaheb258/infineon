/*
 *++ examples - definitions for a XE164
 *-- examples - Definitionen für ein XE164 Zielsystem
 *
 * Copyright (c) 2008 port GmbH Halle (Saale)
 *------------------------------------------------------------------
 *
 * $Header: /z2/cvsroot/library/co_lib/drivers/xe164/examples.h,v 1.3 2008/02/15 11:14:32 ro Exp $
 *
 *------------------------------------------------------------------
 * $Log: examples.h,v $
 * Revision 1.3  2008/02/15 11:14:32  ro
 * small changes (Tasking Classic C166)
 *
 * Revision 1.2  2008/01/17 13:40:41  ro
 * small changes for EVA
 *
 * Revision 1.1  2008/01/14 09:29:09  ro
 * first beta XE164/UConnect
 *
 *
 *
 *
 *
 *------------------------------------------------------------------
 */

/*
 *++ Header file for XE164 target.
 *-- Header für ein XE164 Zielsystem. 
 */

#ifndef EXAMPLES_H
# define EXAMPLES_H 1

/*
   configure example's main() and setOptions() as void or with arguments

   # define CO_EXAMPLE_ARGS_DECL  int argc, char **argv
   # define CO_EXAMPLE_ARGS       argc, argv
   or
   # define CO_EXAMPLE_ARGS_DECL  void
   # define CO_EXAMPLE_ARGS
 */
# define CO_EXAMPLE_ARGS_DECL  void
# define CO_EXAMPLE_ARGS

/* object dictionary version and target strings for examples */
# define CO_MANUF_DEV_NAME	"CANopen - XE164"
# define CO_HW_VER		"1.1"
# define CO_SW_VER		"4.4"


/*
 * define fix bit timing or loadable bit timing from file
 *
 * CAN_START_BIT_RATE = bitRate : use bitRate variable of example
 */
 
# define CAN_START_BIT_RATE	bitRate

/******************************************************************/

/* only overwritten for Keil-c, not for Tasking */
# ifdef CONFIG_COMPILER_KEIL_C166
#  define stdout
# endif

# define fflush()

# ifndef FFLUSH
#  define FFLUSH(dummy)
# endif

/* remove definition from co_debug.h for examples */
# undef PRINTF

/* examples with or without printf() */
#ifdef CONFIG_NO_PRINTF
/* #  define PRINTF (void) */
	/* typical warning: parameter count mismatch */
//#  define PRINTF(a,b)
#  define PRINTF //
#  define PUTCHAR(n)
#else /* CONFIG_NO_PRINTF */
# ifndef PRINTF
#  define PRINTF printf
# endif
# ifndef PUTCHAR
#  define PUTCHAR(n) putchar(n)
# endif
#endif /* CONFIG_NO_PRINTF */

#ifdef CONFIG_TIME_TEST
/* # define SET_BIT(n) do {if(n==1) P9_P4 = 0;}while(0) */
/* # define RESET_BIT(n) do {if(n==1) P9_P4 = 1;}while(0) */
/* # define PULSE_BIT(n) do {if(n==1) { P9_P4 = ~P9_P4; P9_P4 = ~P9_P4;}} while(0) */
#endif

# ifdef MOVE_PRINTF_TO_EMPTY_FUNCTION
/* default: this part will not be used */
#  include <stdarg.h>
#  define PRINTF no_printf
static int no_printf(char* fmt, ...) {return 0;}
# endif /* MOVE_PRINTF_TO_EMPTY_FUNCTION */

/* common prototypes from init_xc164.c 
----------------------------------------------------------------------*/
UNSIGNED8 	iniDevice(void);
BOOL_T    	endLoop(void);
BOOL_T 		setOptions(CO_EXAMPLE_ARGS_DECL);

#endif /* EXAMPLES_H */
