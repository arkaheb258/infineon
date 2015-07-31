/*
 * usr_301 - modul for user interfaces
 *
 * Copyright (c) 1997-2010 port GmbH Halle (Saale)
 *------------------------------------------------------------------
 * $Header: /z2/cvsroot/library/co_lib/examples/s1/usr_301.c,v 1.23 2010/06/18 14:04:13 ro Exp $
 *
 *------------------------------------------------------------------
 *
 *
 * modification history
 * --------------------
 * (not supported with examples)
 *
 *
 *
 *
 *------------------------------------------------------------------
 */


/**
*  \file usr_301.c
*  \author port GmbH Halle (Saale)
*  $Revision: 1.23 $
*  $Date: 2010/06/18 14:04:13 $
*
*++ This modul contains functions for the error handling
*++ (CAN controller errors, node-guarding errors, abort domain transfer)
*++ by the application.
*++ Furthermore interface routines for receiving SDOs and PDOs are defined here.
*++ The functions are called by the CANopen communication services.
*++ The user is responsible for the content of all these functions.
*-- Dieses Modul enthält Funktionen zur Fehlerbehandlung
*-- (CAN Controller Fehler, Node Guarding Fehler, Abort Domain Transfer)
*-- durch die Applikation.
*-- Weiterhin enthält es Schnittstellenfunktionen für den Empfang von
*-- SDOs und PDOs.
*
*/


#include <stdio.h>

#define DEF_HW_PART
#include <cal_conf.h>

#include <cal_conf.h>
#include <co_stru.h>
#include <co_acces.h>
#include <co_sdo.h>
#include <co_pdo.h>
#include <co_time.h>
#include <co_emcy.h>
#include <co_flag.h>
#include <co_usr.h>
#include <co_nmt.h>
#include <co_timer.h>
#include <co_drv.h>

#include <objects.h>

#include <examples.h>

/* constant definitions
---------------------------------------------------------------------------*/

/* external variables
---------------------------------------------------------------------------*/
extern UNSIGNED8 lNodeId; /* local Node-ID for our examples */

/* list of external used functions, if not in headers
---------------------------------------------------------------------------*/
void set_outputs(int port);

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/

/*******************************************************************/
/**
*
*++ \brief getNodeId - get the node ID of the device
*-- \brief getNodeId - ermitteln der Knotennummer des Gerätes
*
*++ This function has to be filled by the user.
*++ It returns the node ID of the device from e.g. a DIP switch
*++ or nonvolatile memory to the CANopen layer.
*-- Der Inhalt dieser Funktion ist durch den Anwender zu definieren.
*-- Sie ermittelt die Netzwerkknotennummer von z.B. einem Schalter
*-- oder nichtflüchtigen Speicher.
*
*++ It is called from the CANopen layer
*++ to initialize node ID dependent COB-IDs.
*-- Die Funktion wird von der CANopen Schicht aufgerufen,
*-- um die von der Knotennummer abhängigen Werte bestimmter COB-IDs
*-- zu bestimmen.
*
* \returns node-id
*++ node ID in the range of 1..127
*-- Netzwerkknotennummer im Bereich von 1..127
*/

UNSIGNED8 getNodeId(void)
{
    return(lNodeId);
}

#ifdef CONFIG_TIME_CONSUMER
/*******************************************************************/
/**
*++ \brief timeInd - indicate the occurence of a Time Stamp Object
*-- \brief timeInd - zeigt den Empfang eines Time Stamp Object an
*
*++ In this function the user has to define his application specific
*++ time stamp handling.
*++ The \c TIME_OF_DAY_T structure, referenced by \em address, contains
*++ the time in ms after midnight and the number of day since January 1, 1984.
*-- In diese Funktion kann der Anwender das Verhalten seiner Applikation
*-- beim Eintreffen eines Time Stamp Objects definieren.
*-- Die durch den Parameter \em address referenzierte Struktur
*-- \c TIME_OF_DAY_T enthält die Zeit seit Mitternacht in ms und die
*-- Anzahl Tage seit dem 1. Januar 1984.
*
* \return
*++ nothing
*-- nichts
*
*/

void timeInd(
	TIME_OF_DAY_T *address	/**< Time Stamp Object */
	CO_COMMA_LINE_PARA_DECL
    )
{
    printf("days %u time %lu",address->days,address->time);
}
#endif /* CONFIG_TIME_CONSUMER */


#if defined(CONFIG_HEARTBEAT_CONSUMER) || defined(CONFIG_NODE_GUARDING)
/********************************************************************/
/**
*++ \brief mGuardErrorInd - indicate the occurence of a nodeguarding/heartbeat error
*-- \brief mGuardErrorInd - zeigt einen Fehler beim Nodeguarding/Heartbeat an
*
*++ This function defines the reaction for node guading error
*++ or a heartbeat event (start heartbeat, bootup, ...)
*++ which will be indicated to the library.
*-- Diese Funktion wird beim Auslösen von Node-Guardingfehlern
*-- oder Heartbeat Ereignissen (wie Start Heartbeat, Bootup Messages...)
*-- aufgerufen.
*
*++ Meaning of the parameter:
*-- Bedeutung der Parameter:
*
* \li  CO_LOST_GUARDING_MSG
* \par
*++ Guarding time has elapsed or toggle bit has not altered.
*-- Guardingintervall ist überschritten oder das Toggle Bit
*-- hat sich nicht verändert.
*
* \li  CO_LOST_CONNECTION
* \par
*++ The lifetime (lifetime factor * guarding time) is elapsed.
*-- Die Lebenszeit (lifetime factor * guarding time) des Knotens ist abgelaufen.
*
* \li  CO_NODE_STATE
* \par
*++ The guarding node has not the expected state.
*-- Der überwachte Knoten hat nicht den erwarteten Zustand.
*
* \li  CO_BOOT_UP
* \par
*++ Bootup Message was received
*-- Bootup Message empfangen
*
* \li  CO_HB_STARTED
* \par
*++ First Heartbeat message was received
*-- Erste Heartbeatmeldung empfangen
*
* \li  CO_LOST_HEARTBEAT
* \par
*++ Heartbeat missing (heartbeat will be disabled)
*-- Heartbeat nicht eingetroffen (Heartbeat wird disabled)
*
* \return
*++ nothing
*-- nichts
*
*/

void mGuardErrorInd(
	UNSIGNED8 nodeId,   /**< node ID of error source */
	ERROR_SPEC_T kind   /**< kind of error */
	CO_COMMA_LINE_PARA_DECL
    )
{
    printf("Guarding Message Node %d: ", nodeId);
    switch(kind) {
	case CO_LOST_GUARDING_MSG:
	    printf("Lost Guarding\n");
	    break;
	case CO_LOST_CONNECTION:
	    printf("Lost Connection\n");
//		safe_state(0);	  
	    break;
	case CO_NODE_STATE:
	    printf("bad node state\n");
	    break;
	case CO_BOOT_UP:
	    printf("Boot Up\n");
	    break;
	case CO_HB_STARTED:
	    printf("Heartbeat Started\n");
		safe_state(0);
	    break;
	case CO_LOST_HEARTBEAT:
	    printf("Heartbeat Lost\n");
		safe_state(0);
	    break;
	default:
	    break;
    }
}
#endif /* defined(CONFIG_HEARTBEAT_CONSUMER) || defined(CONFIG_NODE_GUARDING) */


#ifdef CONFIG_PDO_CONSUMER
/*******************************************************************/
/**
*
*++ \brief pdoInd - indicate the occurence of a PDO
*-- \brief pdoInd - zeigt den Empfang einer PDO an
*
*++ In this function the user has to define his application specific
*++ handling for PDOs.
*-- In diese Funktion ist das Verhalten der Applikation bei Empfang
*-- bestimmter Nachrichten (PDOs) zu implementieren.
*
* \returns
*++ nothing
*-- nichts
*
*/

void pdoInd(
     UNSIGNED16 pdoNr    /**< nr of PDO */
     )
{
   /* Indicate the PDO indication */

# ifndef CONFIG_TIME_TEST
#  ifdef NO_PRINTF_S
    PUTCHAR('*');
#  else /* NO_PRINTF_S */
    PRINTF("PDO received %u \n",(int)pdoNr);
#  endif /* NO_PRINTF_S */
# endif /* CONFIG_TIME_TEST */

    switch(pdoNr) {
	case 1:
	    /* we have a fixed PDO mapping.
	     * Therefore we know that the two sub indicies of 0x6200
	     * are mapped. The content now in the object directory
	     * has to be transferred to the two hardware ports 1 and 2.
	     */
//	    set_outputs(1);
//	    set_outputs(2);
	    break;
    
    }
	PRINTF("Otrzymano PDO 0x%02x\n",pdoNr);
}

#endif /* CONFIG_PDO_CONSUMER */

#if defined(CONFIG_PDO_CONSUMER) && defined(CONFIG_PDO_EVENTTIMER)
/*******************************************************************/
/**
*++ \brief pdoTimerInd - indicate the occurence of a PDO timer event
*-- \brief pdoTimerInd - zeigt den Empfang eines PDO Timer Events an
*
*++ In this function the user has to define his application specific
*++ handling for receive PDO timer events.
*-- In diese Funktion ist das Verhalten der Applikation bei Auftreten
*-- des Timer Ereignis für Empfangs PDOs zu implementieren.
*++ For PDO remote requests (called with the function readPdoReq())
*++ the optional timer event time is used to watch the
*++ occurence of the requested PDO.
*++ If the PDO doesn't arrive in this period,
*++ this function will be called.
*-- Bei der Anforderung von PDOs über RTR Requests (readPdoReq())
*-- wird die optionale Event-Timer Zeit benutzt,
*-- um das Eintreffen des angeforderten PDOs zu überwachen.
*-- Wenn das PDO nicht innerhalb dieser Zeit eintrifft,
*-- wird diese Funktion ebenfalls aufgerufen.
*
* \return
*++ nothing
*-- nichts
*
*/

void pdoTimerInd(
	UNSIGNED16 pdoNr	/**< nr of PDO */
	CO_COMMA_LINE_PARA_DECL
    )
{
    printf("pdoTimerInd: PDO %d\n", pdoNr);
}
#endif /* defined(CONFIG_PDO_CONSUMER) && defined(CONFIG_PDO_EVENTTIMER) */


#ifdef CONFIG_SDO_SERVER
/********************************************************************/
/**
*
*++ \brief sdoWrInd - indicate the occurence of a SDO write access
*-- \brief sdoWrInd - zeigt einen Schreibzugriff über eine SDO an
*
*++ This function is called if an SDO write request reaches the CANopen
*++ SDO server.
*++ Parameters of the function are the Index and Sub Index
*++ of the entry in the local object dictionary where the data
*++ should be written to.
*
*-- Diese Funktion wird beim Eintreffen eines Schreib-Requestes auf einem
*-- CANopen SDO Server gerufen.
*-- Parameter sind der Index und Subindex eines Eintrages im lokalen
*-- Objektverzeichnis, in den der Schreibzugriff erfolgen soll.
*
*++ If numerical data with size up to 4 byte should be written,
*++ the library stores the prevoius value in in temporary buffer.
*++ The new value is put into the local object dictionary.
*-- Bei numerischen Werten (Objektgröße bis zu 4 Byte)
*-- speichert die CANopen Library den alten Wert
*-- in einem Zwischenpuffer und legt den im Schreibrequest enthaltenen Wert
*-- im Objektverzeichnis ab.
*
*++ If the application does not accept this new value,
*++ i.e. the function returns with a value > 0,
*++ the old value is restored from the temporary buffer to the
*++ object dictionary and the SDO write request will be answered
*++ with a "\b Abort \b Domain \b Transfer"
*++ by the library.
*-- Verwirft die Applikation diesen Wert, d.h. Funktionsrückgabewert > 0,
*-- so wird der alte Wert aus dem Zwischenpuffer
*-- nach dem Verlassen dieser Funktion wieder hergestellt und ein
*-- "\b Abort \b Domain \b Transfer"
*-- wird gestartet.
*-- Den Abbruchkode kann man durch Angabe des
*-- \b return -Wertes
*-- festlegen.
*++ The abort code can be specified by the \b return -value.
*
* \returns
*++ The return value, which has to be specified by the application,
*++ selects the possible protocol answer of the write request
*++ to the SDO server.
*-- Der Rückgabewert, welcher applikationsspezifisch ausgefüllt werden muß,
*-- legt die Antwort des SDO-Servers auf die Schreibanforderung fest.
*-- Mögliche Werte sind:
* \retval CO_OK
*++ success
*-- Erfolg
* \retval RET_T
*-- Einer der gültigen, SDO bezogenen Werte kann übergeben werden.
*-- Dieser Wert wird im Fehlerfall an \em abortSdoTransf_Req() übergeben
*++ One of the valid, SDO related, values can be returned.
*++ This value is transferred  to  \em abortSdoTransf_Req() .
*-- Möglich sind:
*++ Possible are:
* \li \c CO_E_NONEXIST_OBJECT
* \li \c CO_E_NONEXIST_SUBINDEX
* \li \c CO_E_NO_READ_PERM
* \li \c CO_E_NO_WRITE_PERM
* \li \c CO_E_MAP
* \li \c CO_E_DATA_LENGTH
* \li \c CO_E_TRANS_TYPE
* \li \c CO_E_VALUE_TO_HIGH
* \li \c CO_E_VALUE_TO_LOW:
* \li \c CO_E_WRONG_SIZE
* \li \c CO_E_PARA_INCOMP
* \li \c CO_E_HARDWARE_FAULT
* \li \c CO_E_SRD_NO_RESSOURCE
* \li \c CO_E_SDO_CMD_SPEC_INVALID
* \li \c CO_E_MEM
* \li \c CO_E_SDO_INVALID_BLKSIZE
* \li \c CO_E_SDO_INVALID_BLKCRC
* \li \c CO_E_SDO_TIMEOUT
* \li \c CO_E_INVALID_TRANSMODE
* \li \c CO_E_SDO_OTHER
* \li \c CO_E_DEVICE_STATE
*
*++ all other return values are defaulting to E_SDO_OTHER. 
*-- alle anderen Werte führen zu einem Default Wert E_SDO_OTHER. 
*
*/

RET_T sdoWrInd(
	  UNSIGNED16 index,	/**< index to object */
	  UNSIGNED8  subIndex   /**< subindex to object */
	  )
{
    /* printf("sdoWrInd: %x:%d\n", index, subIndex); */

    if (index == 0x6200) { 
	/* write request to the digital output 8-bit ports */
	set_outputs(subIndex);
    }

//	PRINTF("Zapis SDO 0x%02x 0x%02x\n",index,subIndex);
    return CO_OK;
}


/********************************************************************/
/**
*++ \brief sdoRdInd - indicate the occurence of a SDO read access
*-- \brief sdoRdInd - zeigt einen Lesezugriff über eine SDO an
*
*++ This function determines a reaction to the device for
*++ an index - subindex read access via SDO.
*++ With this function the object dictionary has to made up to date
*++ e.g. by reading of digital inputs.
*++ If an error occurs an \b abort \b domain \b transfer will be started.
*++ This function is called
*++ only for objects with an index greater 0x1FFF.
*-- Diese Funktion definiert eine Reaktion der Applikation auf einen
*-- Lesezugriff zum Objektverzeichnis.
*-- Durch diese Funktion ist der angegebene Wert im Objektverzeichnis
*-- zu aktualisieren z.B Einlesen von digitalen Eingängen.
*-- Tritt ein Fehler auf, wird ein \b Abort \b Domain \b Transfer gestartet.
*-- Diese Funktion wird nur für Objekte mit einem Index größer 0x1FFF gerufen.
*
* \retval CO_OK
*++ success
*-- Erfolg
* \retval RET_T
*-- Einer der gültigen, SDO bezogenen Werte kann übergeben werden.
*-- Dieser Wert wird im Fehlerfall an \em abortSdoTransf_Req() übergeben
*++ One of the valid, SDO related, values can be returned.
*++ This value is transferred  to  \em abortSdoTransf_Req() .
*-- Möglich sind:
*++ Possible are:
* \li \c CO_E_NONEXIST_OBJECT
* \li \c CO_E_NONEXIST_SUBINDEX
* \li \c CO_E_NO_READ_PERM
* \li \c CO_E_NO_WRITE_PERM
* \li \c CO_E_MAP
* \li \c CO_E_DATA_LENGTH
* \li \c CO_E_TRANS_TYPE
* \li \c CO_E_VALUE_TO_HIGH
* \li \c CO_E_VALUE_TO_LOW:
* \li \c CO_E_WRONG_SIZE
* \li \c CO_E_PARA_INCOMP
* \li \c CO_E_HARDWARE_FAULT
* \li \c CO_E_SRD_NO_RESSOURCE
* \li \c CO_E_SDO_CMD_SPEC_INVALID
* \li \c CO_E_MEM
* \li \c CO_E_SDO_INVALID_BLKSIZE
* \li \c CO_E_SDO_INVALID_BLKCRC
* \li \c CO_E_SDO_TIMEOUT
* \li \c CO_E_INVALID_TRANSMODE
* \li \c CO_E_SDO_OTHER
* \li \c CO_E_DEVICE_STATE
*
*++ all other return values are defaulting to E_SDO_OTHER. 
*-- alle anderen Werte führen zu einem Default Wert E_SDO_OTHER. 
*
*/

RET_T sdoRdInd(
	  UNSIGNED16 index,	/**< index to object */
	  UNSIGNED8  subIndex   /**< subindex to object */
	  )
{
    switch (index) {
	case 0x1000:
	    if (subIndex > 0) {
		subIndex = 0;
	    }
	    break;
    }
//	PRINTF("Odczyt SDO 0x%02x 0x%02x\n",index,subIndex);
    return CO_OK;
}


#endif /* CONFIG_SDO_SERVER */


#ifdef CONFIG_NON_VOLATILE_MEM
/********************************************************************/
/**
*
*++ \brief saveParameterInd - indicate a store to non volatile memory command
*-- \brief saveParameterInd - zeigt das Kommando Store to non volatile memory an
*
*++ This function indicates a "store parameters to non volatile memory" 
*++ command via SDO.
*++ This command is a SDO write access to object 0x1010 with the signature
*++ "save".
*++ In this function the user has to implement his target specific
*++ save functions.
*++ If this function returns an error an \b abort \b domain \b transfer will
*++ be initiate with the error code "hardware fault".
*-- Diese Funktion zeigt ein "store parameters to non volatile memory" an.
*-- Diese Kommando ist ein SDO Schreibzugriff auf das Objekt 0x1010 
*-- mit der Signatur "save".
*-- In diese Funktion ist die plattformspezifische Speicherroutine
*-- zu implementieren.
*-- Gibt diese Funktion einen Fehler zurück, so wird ein \b Abort \b Domain \b Tranfer
*-- mit dem Fehlerkode "hardware fault" ausgelöst. 
*++ The parameter segment corresponds by subindex the subindex from 1010.
*-- Der Parameter segment entspricht dem subindex vom Objekt 1010.
*
* \retval CO_TRUE
*++ success
*--Erfolg
* \retval CO_FALSE
*++ error
*-- Fehler
*
*/

BOOL_T saveParameterInd(
	UNSIGNED8 segment   /**< subindex which specifies the memory segment */
	CO_COMMA_LINE_PARA_DECL
    )
{ 
    switch(segment)
    {
        /* all parameters */
        case MEM_SEG_ALL_PARAMETERS:
            break;
        /* communication parameter */    
        case MEM_SEG_COM_PARAMETERS:
            break;
        /* application parameter */    
	case MEM_SEG_APPL_PARAMETERS:
            break;
        /* segment 4 - 255 manufacturer specific */     
        default:     
            break;
    }
    return CO_TRUE;
}


/********************************************************************/
/**
*
*++ \brief clearParameterInd - indicate a clear non volatile memory command
*-- \brief clearParameterInd - zeigt das Kommandos clear non volatile memory an
*
*++ This function indicates a "clear to non volatile memory"
*++ command via SDO.
*++ This command is a SDO write access to object 0x1010 with the signature
*++ "\b kill".
*++ This signature is an extension of the library by \em port.
*++ In this function the user has to implement his target specific
*++ clear functions.
*++ If this function returns an error an \b abort \b domain \b transfer will
*++ be initiate with the error code "hardware fault".
*-- Diese Funktion zeigt ein "clear non volatile memory" an.
*-- Diese Kommando ist ein SDO Schreibzugriff auf das Objekt 0x1010
*-- mit der Signatur "\b kill".
*-- Diese Signatur ist eine Erweiterung der Bibliothek von \em port.
*-- In diese Funktion ist die plattformspezifische Löschroutine
*-- zu implementieren.
*-- Gibt diese Funktion einen Fehler zurück, so wird ein \b Abort \b Domain \b Tranfer
*-- mit dem Fehlerkode "hardware fault" ausgelöst.
*
* \retval
* CO_TRUE
*++ success
*--Erfolg
* \retval
* CO_FALSE
*++ error
*-- Fehler
*
*/

BOOL_T clearParameterInd(
	UNSIGNED8 segment   /**< subindex which specifies the memory segment */
	CO_COMMA_LINE_PARA_DECL
    )
{
    switch(segment)
    {
        /* all parameters */
        case MEM_SEG_ALL_PARAMETERS:
            break;
        /* communication parameter */    
	case MEM_SEG_COM_PARAMETERS:
            break;
        /* application parameter */    
        case MEM_SEG_APPL_PARAMETERS:
            break;
        /* segment 4 - 255 manufacturer specific */     
        default:     
            break;
    }

    return CO_TRUE;
}

/********************************************************************/
/**
*
*++ \brief loadParameterInd - indicate a restore from non volatile memory command
*-- \brief loadParameterInd - zeigt das Kommandos restore from non volatile memory an
*
*++ This function indicates a "restore parameters from non volatile memory".
*++ It its called from the library to load the objects from Flash or EEPROM.
*-- Diese Funktion zeigt ein "load parameters from non volatile memory" an.
*-- Sie wird von der Library zum Laden der mit saveParameterInd() 
*-- gesicherten Objekte aufgerufen.
*
* \retval
* CO_TRUE
*++ success
*--Erfolg
* \retval
* CO_FALSE
*++ error
*-- Fehler
*
*/

BOOL_T loadParameterInd(
	UNSIGNED8 segment   /**< subindex which specifies the memory segment */
	CO_COMMA_LINE_PARA_DECL
    )
{ 
    switch(segment)
    {
        /* all parameters */
        case MEM_SEG_ALL_PARAMETERS:
            break;
        /* communication parameter */    
        case MEM_SEG_COM_PARAMETERS:
            break;
        /* application parameter */    
        case MEM_SEG_APPL_PARAMETERS:
            break;
        /* segment 4 - 255 manufacturer specific */     
        default:     
            break;
    }

    return CO_TRUE;
}


/********************************************************************/
/**
*
*++ \brief loadDefaultParameterInd - indicate a restore to default data
*-- \brief loadDefaultParameterInd - zeigt das Kommandos restore default Para
*
*++ This function indicates a "restore default parameters"
*++ command via SDO.
*++ This command is a SDO write access to object 0x1011 with the signature
*++ "load".
*++ In this function the user has to implement his target specific
*++ restore functions.
*++ If this function returns an error an \b abort \b domain \b transfer will
*++ be initiate with the error code "hardware fault".
*-- Diese Funktion zeigt ein "load default parameters" an.
*-- Diese Kommando ist ein SDO Schreibzugriff auf das Objekt 0x1010
*-- mit der Signatur "load".
*-- Diese Signatur ist eine Erweiterung der Bibliothek von \em port.
*-- In diese Funktion ist die plattformspezifische Leseroutine
*-- zu implementieren.
*-- Gibt diese Funktion einen Fehler zurück,
*-- so wird ein \b Abort \b Domain \b Tranfer
*-- mit dem Fehlerkode "hardware fault" ausgelöst.
*
*++ Please Note:
*++ the default Parameter are only valid after a reset communication !
*-- Achtung:
*-- Die Default Parameter werden erst nach einem Reset Communication gültig !
*
*
* \retval
* CO_TRUE
*++ success
*--Erfolg
* \retval
* CO_FALSE
*++ error
*-- Fehler
*
*/

BOOL_T loadDefaultParameterInd(
	UNSIGNED8 segment   /**< subindex which specifies the memory segment */
	CO_COMMA_LINE_PARA_DECL
    )
{
    switch(segment)
    {
	/* application parameter */
	case MEM_SEG_APPL_PARAMETERS:
	    break;
	/* segment 4 - 127 manufacturer specific */
	default:
	    return CO_FALSE;
    }

    return CO_TRUE;
}

#endif /* CONFIG_NON_VOLATILE_MEM */

#ifdef CONFIG_CAN_ERROR_HANDLING
/********************************************************************/
/**
*++ \brief canErrorInd - indicate the occurence of errors on the CAN driver
*-- \brief canErrorInd - zeigt das Aufreten von CAN Treiberfehlern an
*
*++ This function indicates the following errors:
*-- Diese Funktion zeigt das Auftreten der folgenden Fehler an:
*
* - \c CANFLAG_ACTIVE -
*   CAN Error Active
*
* - \c CANFLAG_BUSOFF -
*++ CAN-controller error CAN Busoff
*-- Fehler vom CAN Controller - CAN Busoff
*
* - \c CANFLAG_PASSIVE -
*++ CAN-controller error
*-- Fehler vom CAN Controller
*
* - \c CANFLAG_OVERFLOW -
*++ CAN-controller overrun error
*-- Overrun Fehler vom CAN Controller
*
* - \c CANFLAG_TXBUFFER_OVERFLOW -
*++ transmit buffer overflow
*-- Sendepuffer übergelaufen
*
* - \c CANFLAG_RXBUFFER_OVERFLOW -
*++ receive buffer overflow
*-- Empfangspuffer übergelaufen
* 
*
*++ All occured status changes since the last canErrorInd() call are indicated.
*++ The current state can be read with getCanDriverState().
*-- Es werden alle aufgetretenen Statusänderungen
*-- seit dem letzten canErrorInd() angezeigt.
*-- Der aktuelle Zustand kann mit getCanDriverState() ausgelesen werden.
*
* \retval
* CO_TRUE
*++ CAN controller has to stay in the current state
*-- CAN Controller soll im aktuellen Zustand bleiben
* \retval
* CO_FALSE
*++ CAN controller has to go to BUS ON again
*-- CAN Controller soll wieder nach BUS ON gehen
*
*/

BOOL_T canErrorInd(
	UNSIGNED8 errorFlags	/**< CAN error flags */
	CO_COMMA_LINE_PARA_DECL
    )
{
UNSIGNED8	state;
BOOL_T		ret = CO_TRUE;


    PRINTF("canErrorInd status changes: ");
    if ((errorFlags & CANFLAG_ACTIVE) != 0)  {
	PRINTF("ERROR_ACTIVE\n");
    }
    if ((errorFlags & CANFLAG_PASSIVE) != 0)  {
	PRINTF("ERROR_PASSIVE\n");
    }
    if ((errorFlags & CANFLAG_BUSOFF) != 0)  {
	PRINTF("BUS_OFF\n");
	ret = CO_FALSE; /* Auto Bus-On */
    }
    if ((errorFlags & CANFLAG_OVERFLOW) != 0)  {
	PRINTF("OVERRUN\n");
    }
    if ((errorFlags & CANFLAG_RXBUFFER_OVERFLOW) != 0)  {
	PRINTF("ERROR_RXBUFFER_OVERFLOW\n");
    }
    if ((errorFlags & CANFLAG_TXBUFFER_OVERFLOW) != 0)  {
	PRINTF("ERROR_TXBUFFER_OVERFLOW\n");
    }

    state = getCanDriverState();
    PRINTF("The actual state is: (%x): ", state);

    if ((state & CANFLAG_INIT) != 0)  {
	PRINTF("INIT\n");
    }
    if ((state & CANFLAG_ACTIVE) != 0)  {
	PRINTF("Active\n");
    }
    if ((state & CANFLAG_BUSOFF) != 0)  {
	PRINTF("BUS_OFF\n");
    }
    if ((state & CANFLAG_PASSIVE) != 0)  {
	PRINTF("ERROR_PASSIVE\n");
    }

    return(ret);
}
#endif /* CONFIG_CAN_ERROR_HANDLING */

/*______________________________________________________________________EOF_*/
