/*
 * environ.h - CPU/compiler settings depending on the used environment
 *
 * Copyright (c) 2007-2008 port GmbH Halle/Saale
 *------------------------------------------------------------------
 *
 * $Header: /z2/cvsroot/library/co_lib/drivers/xe164/environ.h,v 1.4 2008/07/24 14:08:46 boe Exp $
 *
 *------------------------------------------------------------------
 * $Log: environ.h,v $
 * Revision 1.4  2008/07/24 14:08:46  boe
 * release keil compiler usage
 *
 * Revision 1.3  2008/02/15 11:13:27  ro
 * small changes (Tasking Classic C166)
 *
 * Revision 1.2  2008/01/17 13:40:41  ro
 * small changes for EVA
 *
 * Revision 1.1  2008/01/14 09:29:09  ro
 * first beta XE164/UConnect
 *
 * Revision 1.1  2007/10/30 07:29:59  se
 * adaptation at Tasking VX-toolset for C166:
 * 1. file added for hardware-, compiler- and application-specific
 *    definitions
 *
 *
 *
 *------------------------------------------------------------------
 */

/**
* \file environ.h
* \author port GmbH
* $Revision: 1.4 $
* $Date: 2008/07/24 14:08:46 $
*
*/

#ifndef ENVIRON_H
# define ENVIRON_H 1

# ifdef CONFIG_COMPILER_TASKING_C166
/*---------------------------------------------------------*/
/* Tasking VX-toolset C166 */
/*---------------------------------------------------------*/
#  ifdef __VX__
#   define NOP()		__nop()

#  else
/*---------------------------------------------------------*/
/* Tasking Classic C166 */
/*---------------------------------------------------------*/
#  endif /* __VX__ */

# endif /* CONFIG_COMPILER_TASKING_C166 */

/*---------------------------------------------------------*/
/* Keil */
/*---------------------------------------------------------*/
# ifdef CONFIG_COMPILER_KEIL_C166
#  include <intrins.h>
#  include <xe16x.h>

#  define NOP()		_nop_()
#  define IEN		PSW_IEN
# endif

#endif
