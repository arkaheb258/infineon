/****************************************************************************
*
*  Copyright (C) Infineon Technologies (2011)
*
*  All rights reserved.
*
*  This document contains proprietary information belonging to Infineon
*  Technologies. Passing on and copying of this document, and communication
*  of its contents is not permitted without prior written authorization.
*
*                          No Warranty
*  Because the program is licensed free of charge, there is no warranty for
*  the program, to the extent permitted by applicable law. Except when
*  otherwise stated in writing the copyright holders and/or other parties
*  provide the program "as is" without warranty of any kind, either expressed
*  or implied, including, but not limited to, the implied warranties of
*  merchantability and fitness for a particular purpose. The entire risk as
*  to the quality and performance of the program is with you. Should the
*  program prove defective, you assume the cost of all necessary servicing,
*  repair or correction.
*----------------------------------------------------------------------------
*  Module		:   Emulated EEPROM Driver
*  Filename		:   eeprom.h
*  Controller	:	Infineon XC2000/XE166
*
*  Version		:	1.1
*  Release Date	:	8 September 2011
*******************************************************************************/

/****************************************************************************
	Project Includes
****************************************************************************/
#include  "dave/MAIN.H"

/****************************************************************************
	User-Configurable Parameters
****************************************************************************/

#define EEPROM_START_ADDRESS		(0x0c40000)

#define NO_OF_PAGE					(3)

#define EEPROM_BLOCK_SIZE			(0x1000)

#define TOTAL_EMULATION_SIZE		(0x3000)
			  
#define FLASH_MODULE				(1)

#define MASKING_VALUE				(0x00FF)

#define VALIDITY_BIT				(4)

#define ADVANCE_MODE				(OFF)

/****************************************************************************
	Non-configurable Defines
****************************************************************************/

	/*General define*/
#define ON							(1)

#define OFF							(0)

#define TRUE						(1)

#define FALSE						(0)

	/*Configuration define*/
#define TOTAL_PAGE					(NO_OF_PAGE - 1)

#define TOTAL_BLOCK					(EEPROM_BLOCK_SIZE / 0x1000)

#define EEPROM_ADDRESS_LOC			(END_ADDRESS - 2)

#define BUSY_FLAG					(0x0001U << FLASH_MODULE)

#define FLASH_BUSY_STATUS			(IMB_FSR_BUSY & BUSY_FLAG)

#define END_OF_BLOCK				(EEPROM_START_ADDRESS + 126)

#define END_ADDRESS					(EEPROM_START_ADDRESS + TOTAL_EMULATION_SIZE)

#define BIT_MASKING					~(0xFF << VALIDITY_BIT)

#define VALIDITY_VALUE				(0X05 & BIT_MASKING)

	/*EEPROM Job Type*/
#define IDLE						(0)

#define EEPROM_WRITE				(1)

#define EEPROM_ERASE				(2)

#define EEPROM_COPYDATA				(4)

#define STATIC_EEPROM_ERASE			(5)

	/*Copy Type*/
#define COPY_END_ADDRESS			(4)

#define COPY_NEXT_BLOCK				(5)

	/*EEPROM STATUS*/
#define NOT_INIT					(0)

#define ALREADY_INIT				(1)

#define EEPROM_BUSY					(4) //change

	/*EEPROM INIT RETURN*/
#define INIT_SUCCESS				(1)

#define DATA_INCOMPLETE				(2)

#define DATA_EMPTY					(4)

#define DATA_FULL					(8)

	/* OTHER RETURN STATUS */
#define COMPLETE					(1)

#define CONFIG_ERROR				(2)

#define FAILED						(4) //change

#define INVALID_1					(1)

#define INVALID						(2)

#define VALID						(3)

	/* Erase_Status */
#define ERASE						(2)

#define START					    (1)

#define CLEAR						(0)


/****************************************************************************
	Global Function declaration
****************************************************************************/

	void EEPROM_vIntProcess (void);
	void EEPROM_vEraseDataBlock (void);
 	int EEPROM_Init (void);
	int EEPROM_intWrite (uword *DataPtr, uword address);
	int EEPROM_intRead (uword *buffer, uword address);
	int EEPROM_intCheckBusy (void);
	char EEPROM_intReadInteger (uword address, ubyte ByteAdd);

/****************************************************************************/

