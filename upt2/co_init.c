/*
 * co_init.c - Initialization functions for the CANopen Library
 *
 * Copyright (C) 1998-2013  by port GmbH  Halle(Saale)/Germany
 *
 *------------------------------------------------------------
 * This file was generated by the CANopen Design Tool V2.2.53.0
 * on 2015/01/21 at 10:42:00.
 */

/**
 * \file co_init.c
 * \author port GmbH
 *
 * This module contains the initialization and deinitialization
 * of the CANopen Library for the CANopen device.
 */


/* header of standard C libraries */ 
#include <stdio.h>
#include <stdlib.h>

/* header of the CANopen Library */
#include <cal_conf.h>
#include <co_init.h>
#include <co_type.h>
#include <co_acces.h>
#include <co_nmt.h>
#include <co_lme.h>
#include <co_drv.h>
#include <co_sdo.h>
#include <co_pdo.h>
#include <co_sync.h>
#include <co_time.h>
#include <co_hb.h>
/* Redefine the following expressions with your specific
 output functions.*/
#ifndef PRINTF
# define PRINTF(s, e)                                                                                            
#endif
#ifndef PUTCHAR
# define PUTCHAR(e)    
#endif


#ifndef CO_GLOBVARS_PARA_DECL
# define CO_GLOBVARS_PARA_DECL	void
#endif


/* Redefine INIT_USER_SETTINGS with your specific function. */
#ifndef INIT_USER_SETTINGS
# define INIT_USER_SETTINGS() 
#endif


/* Macro to return at errors. */ 
#ifndef CO_INIT_PRINTRET
# ifdef NO_PRINTF_S
#  define CO_INIT_PRINTRET(s) do {						\
		    if ( commonRet == CO_OK ) { PUTCHAR('0'); }		\
		    else { PUTCHAR('1'); return(commonRet); }		\
		  } while(0)
# else 
#  define CO_INIT_PRINTRET(s) do {						\
		    PRINTF( s": %d\n", (int) commonRet);         	\
		    if( commonRet != CO_OK ) return(commonRet);		\
		  } while(0)
# endif
#endif

#ifdef CONFIG_RCS_IDENT
static CO_CONST char _rcsid[] = "$Id: generateInitc.tcl,v 1.66 2013/12/09 12:35:44 se Exp $";
#endif /* CONFIG_RCS_IDENT*/

 
/* Definition of init_Library */ 
/********************************************************************/
/**
*++\brief init_Library - initialization routine for the CANopen library
*--\brief init_Library - Initialisierungsroutine f�r die CANopen Bibliothek
*
*++ This function initializes the CANopen library
*++ and defines the CANopen services.
*-- Diese Funktion initialisiert die Bibliothek
*-- und definiert die CANopen-Dienste.
*
*\retval
*++ CO_OK in case of success
*++ or specific library error code of the failed function.
*-- CO_OK bei Erfolg
*-- oder spezifischer R�ckgabewert der fehlgeschlagenen Funktion.
*
*/
RET_T init_Library(CO_GLOBVARS_PARA_DECL) { 
   RET_T 	commonRet;      /* return value for CANopen functions */ 
   UNSIGNED16	pdoNum =0;      /* pdo loop variable */ 


   /*--- inititializing of CANopen -----------------------------------*/ 
   /* defines also the Network control Object -- NMT 
    * reset communication and goes to the 
    * state preoperational + Initialization of CANopen */ 
    commonRet = initCANopen( CO_LINE_PARA );
    CO_INIT_PRINTRET("CANopen initialization");

   /* Call user defined function */

    INIT_USER_SETTINGS();

    /* Definition of CANopen Objects */
    /* ============================= */
    commonRet = defineSync(CONSUMER CO_COMMA_LINE_PARA);
    CO_INIT_PRINTRET("Line 0: Define Sync Consumer");
    commonRet = defineTime(CONSUMER CO_COMMA_LINE_PARA);
    CO_INIT_PRINTRET("Line 0: Define Time Consumer");

    /* definition of SDOs Line 0 
     * 1st parameter: SDO number 
     * 2nd parameter: SDO type: CLIENT �SERVER
     */
    commonRet = defineSdo(1, SERVER  CO_COMMA_LINE_PARA);
    CO_INIT_PRINTRET("Line 0: Define 1st Server-SDO");

    /* definition of PDOs 
     * 1st parameter: type of PDO: RECEIVE | TRANSMIT
     * 2nd parameter: PDO number
     * 3rd parameter: permission for dynamically mapping
     */
    for (pdoNum = 1; pdoNum <= 2; pdoNum++) {
        commonRet = definePdo(TRANSMIT_PDO, pdoNum, CO_FALSE  CO_COMMA_LINE_PARA);
        CO_INIT_PRINTRET("Line 0: Define TRANSMIT_PDO");
    }

    /* Definition of Heartbeat Consumer */
    commonRet = defineHeartbeatConsumer(CO_LINE_PARA);
    CO_INIT_PRINTRET("defineHeartbeatConsumer");

    /* definition of the local node */
                     /* Node Guarding, Heartbeat, Master */
    commonRet = createNodeReq(CO_FALSE,CO_TRUE CO_COMMA_LINE_PARA);
    CO_INIT_PRINTRET("NMT Node created");
    return(commonRet);
}

/* Definition of deinit_Library */ 
/********************************************************************/
/**
*++\brief deinit_Library - deinitialization routine for the CANopen library
*--\brief deinit_Library - Deinitialisierungsroutine f�r die CANopen Bibliothek
*
*++ This function deinitializes the CANopen library.
*-- Diese Funktion deinitialisiert die Bibliothek
*
*\retval
*++ CO_OK
*-- CO_OK
*
*/
RET_T deinit_Library(CO_GLOBVARS_PARA_DECL) { 

   RET_T 	commonRet;      /* return value for CANopen functions */ 
    /* delete node object */
    commonRet = deleteNodeReq(CO_LINE_PARA);
    CO_INIT_PRINTRET("NMT Node removed");

    /* leaves CANopen    */
    leaveCANopen(CO_LINE_PARA);
    return(CO_OK);
}
