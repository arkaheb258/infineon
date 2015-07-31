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
*  Filename		:   eeprom.c
*  Controller	:	Infineon XC2000/XE166
*
*  Version		:	1.1
*  Release Date	:	8 September 2011
*  Version		:	1.2
*  Release Date	:	17 April 2011
*******************************************************************************/

/********************************************************************************************
	Project Includes
********************************************************************************************/
#include <stddef.h>
#include "eeprom.h"

/********************************************************************************************
	Variable Declaration
********************************************************************************************/														  
	static ubyte Address_Count;
	static ubyte Copy_Type;
	static ulong Erase_Address;
	ubyte EEPROM_JobType;
	ubyte Erase_Status;
	ubyte EEPROM_Status;
	_shuge char *Write_ptr;
	_shuge char *Erase_ptr;
	static _shuge char *CopyData_ptr;
	static _shuge char *NextActiveBlock;
	static _shuge char *PreviousBlock_ptr;
	static _shuge char *ActiveBlock_ptr;
	static _shuge int *Read_page;

/********************************************************************************************
	Local Function Prototypes
********************************************************************************************/

	static int EEPROM_intGetActiveBlock (void);
	static void EEPROM_vCheckPointer (void);
	static void EEPROM_vCopyData (void);
	static void EEPROM_vEraseInactiveBlock (void);
	static void EEPROM_vEraseReturn (void);
	static int  EEPROM_intCheckValidity (_shuge int *Checkptr);
	static int  EEPROM_intCheckAndFixed (void);
	static void EEPROM_vIdentifyPage (void);
	static void FLASH_Reset (void);
	static void EEPROM_vUserEraseReturn (void);
	static void EEPROM_vEraseBlock (unsigned long sector);
	static void FLASH_vErase (unsigned long sector);
	static void FLASH_vWrite (uword *dataPtr);
	static int  FLASH_intGetValidData (_shuge int *Start_Read, uword address);
	static void EEPROM_WriteRoutine (uword *DataPtr, uword address);

/******************************************************************************
	Function	  	int EEPROM_Init (void)
-------------------------------------------------------------------------------
	Description  	This function initializes the Emulated EEPROM driver.
					It will check for the EEPROM Emulation parameters and
					and recover previously saved data in the FLASH. All block
					will be erase except Active Block or no data found in the
					FLASH.

-------------------------------------------------------------------------------
	Returnvalue   	CONFIG_ERROR
					INIT_SUCCESS
					DATA_NOT_COMPLETE

-------------------------------------------------------------------------------
	Parameters    	None

*******************************************************************************/

int EEPROM_Init (void)
{
int RetVal;
int RetVal2;
ubyte GLOBAL_INT;

	if (EEPROM_Status == ALREADY_INIT)
	{
	  return INIT_SUCCESS;
	}

	/* Check if start address align with the 4k boundary */
#if((EEPROM_START_ADDRESS & 0x0FFF) != 0)
#error CONFIG_ERROR: EEPROM_START_ADDRESS not align with 4k boundary
#endif

	/* Check if emulation block size is align with the 4k boundary */
#if((EEPROM_BLOCK_SIZE & 0x0FFF) != 0)
#error CONFIG_ERROR: EEPROM_BLOCK_SIZE not align with 4k boundary
#endif

	/* Check if emulation block size is less than 1 sector*/
#if(EEPROM_BLOCK_SIZE < 0x1000)
#error CONFIG_ERROR: EEPROM_BLOCK_SIZE is less than 1 sector
#endif

	/* Check if emulation block size is less than emulation data set size*/
#if (EEPROM_BLOCK_SIZE <= (NO_OF_PAGE*0x080))
#error CONFIG_ERROR: EEPROM_BLOCK_SIZE is less than emulation data set size
#endif

	/*Check if the total emulation size is the multiple of emulation block size*/
#if ((TOTAL_EMULATION_SIZE % EEPROM_BLOCK_SIZE) != 0)
#error CONFIG_ERROR: TOTAL_EMULATION_SIZE is not the multiple of emulation block size
#endif

	/* Store the current interrupt status and disable the interrupt*/
	GLOBAL_INT = PSW_IEN;
	/* Disable global interrupt */
	PSW_IEN = 0;
	
	RetVal = EEPROM_intGetActiveBlock ();

	if (RetVal == INVALID)
	{
		return DATA_FULL;
	}
	else
	{
		RetVal2 = EEPROM_intCheckAndFixed();
	}

	NextActiveBlock = (_shuge char *)(((ulong)ActiveBlock_ptr) + EEPROM_BLOCK_SIZE);
	  
	EEPROM_Status = ALREADY_INIT;

	EEPROM_vCheckPointer();
	while (EEPROM_JobType != IDLE)
	{
	  while(FLASH_BUSY_STATUS != 0);
	  EEPROM_vIntProcess();
	}

	EEPROM_vEraseInactiveBlock();

	EEPROM_vIdentifyPage();

	/* Restore previous global interrupt status */
	PSW_IEN = GLOBAL_INT;

	if (RetVal == VALID)
	{
		if (RetVal2 == VALID)
		{
		  return INIT_SUCCESS;
		}
		else
		{
		  return DATA_INCOMPLETE;
		}
	}
	else
	{
      return DATA_EMPTY;
	}
	/* End of function */
}

/******************************************************************************
	Function	  	FLASH_vErase (unsigned long sector)
-------------------------------------------------------------------------------
	Description  	This function shall erase the sector of the provided
					address	in the flash memory.

-------------------------------------------------------------------------------
	Returnvalue   	None

-------------------------------------------------------------------------------
	Parameters    	sector:
					- Address of the sector to be erase

*******************************************************************************/

static void FLASH_vErase (unsigned long sector)
{	
	_shuge unsigned int *Flash_Command_1 = (_shuge unsigned int *) 0x0c000AA;
	_shuge unsigned int *Flash_Command_2 = (_shuge unsigned int *) 0x0c00054;
	_shuge unsigned int *Flash_Command_3 = (_shuge unsigned int *) sector;
	
	while(FLASH_BUSY_STATUS != 0);

	/* erase sector command sequence */
	*Flash_Command_1 = 0x0080;
	*Flash_Command_2 = 0x00AA;
	*Flash_Command_3 = 0x0033;

	/* End of function */
}

/******************************************************************************
	Function	  	int EEPROM_intWrite (uword *DataPtr, uword address)
-------------------------------------------------------------------------------
	Description  	This function will initiated the function to write data 
					into Flash memory with the page address.

-------------------------------------------------------------------------------
	Returnvalue   	SUCCESS
					FAILED
					CANCELLED
-------------------------------------------------------------------------------
	Parameters    	DataPtr:
					- Pointer to data buffer
					address
					- EEPROM logical address

*******************************************************************************/

int EEPROM_intWrite (uword *DataPtr, uword address)
{
	/*Check for correct input parameters*/
	if (address > TOTAL_PAGE)
	{
	  return CONFIG_ERROR;
	}

	/* Check for EEPROM initialize status */
 	if (EEPROM_Status != ALREADY_INIT)
	{
	  return NOT_INIT;
	}

	/* Check for EEPROM is copying data */
	if (EEPROM_JobType == EEPROM_COPYDATA)
	{
	  return EEPROM_BUSY;
	}

	/* Check for EEPROM is writing data to EEPROM */
	if (EEPROM_JobType == EEPROM_WRITE)
	{
	  return EEPROM_BUSY;
	}

	EEPROM_WriteRoutine(DataPtr, address);


#if (ADVANCE_MODE == OFF)
	while(EEPROM_JobType != IDLE)
	{
	  while(FLASH_BUSY_STATUS != 0);
	  EEPROM_vIntProcess();
	}

	if (Erase_Status == ERASE)
    {
	  while (Erase_Status!= CLEAR)
	  {
	    EEPROM_vEraseDataBlock();
	    while(FLASH_BUSY_STATUS != 0);
	  }
    }
#endif

	return COMPLETE;
}

static void EEPROM_WriteRoutine (uword *DataPtr, uword address)
{
  uword *tempPtr;	

	tempPtr = DataPtr;
	tempPtr = tempPtr + 63;

	/* Insert Address*/
	address <<= VALIDITY_BIT;
	address |= VALIDITY_VALUE;
	*tempPtr &= (MASKING_VALUE ^ 0xFFFF);
	*tempPtr |=  address;
	
	if (EEPROM_JobType != EEPROM_COPYDATA)
	{
	  EEPROM_JobType = EEPROM_WRITE;
	}
	else
	{
	  while(FLASH_BUSY_STATUS != 0);
	}
		  
	/* Write to Flash Memory*/
	while(FLASH_BUSY_STATUS != 0);
	FLASH_vWrite(DataPtr);
	
	/* End of function */
}

/******************************************************************************
	Function	  	void FLASH_vWrite (uword *dataPtr)
-------------------------------------------------------------------------------
	Description  	This function contain the command cycles to write data
					from the data buffer to Flash memory.

-------------------------------------------------------------------------------
	Returnvalue   	None

-------------------------------------------------------------------------------
	Parameters    	DataPtr:
					- Pointer to data buffer

*******************************************************************************/

static void FLASH_vWrite (uword *dataPtr)
{
	_shuge unsigned int *Flash = (_shuge unsigned int  *) 0x0c000AA;
	_shuge unsigned int *Load_Page = (_shuge unsigned int  *) 0x0c000F2;
	_shuge unsigned int *Write_Page = (_shuge unsigned int *) 0x0c0005A;
	_shuge unsigned int *Page_destination = (_shuge unsigned int *) Write_ptr;
	unsigned int count;

	/* Enter Page mode */
	*Flash = 0x0050;
	*Page_destination = 0x00AA;

	/* Wait for busy flag to clear after enter page mode */
	while(FLASH_BUSY_STATUS != 0);

	/* Load Page*/
	for (count=0; count < 64; count++)
	{
		*Load_Page = *dataPtr;
		dataPtr++;
	}

	/* Program page */
	*Flash = 0x00A0;
	*Write_Page = 0x00AA;

	/* End of function */
}

/******************************************************************************
	Function	  	int EEPROM_intRead (uword *buffer, uword address)
-------------------------------------------------------------------------------
	Description  	This function shall load the latest data of the provided
					page address in the Flash memory to the read buffer.

-------------------------------------------------------------------------------
	Returnvalue   	CONFIG_ERROR
					FAILED
					COMPLETE

-------------------------------------------------------------------------------
	Parameters		buffer
					address

*******************************************************************************/

int EEPROM_intRead (uword *buffer, uword address)
{
	uword c;
	uword RetVal;
	

	/* Check for correct input parameters */
	if (address > TOTAL_PAGE)
	{
	  return CONFIG_ERROR;
	}	
	
	/* Check for EEPROM initialise status */
	if (EEPROM_Status != ALREADY_INIT)
	{
	  return NOT_INIT;
	}

	while(FLASH_BUSY_STATUS != 0);

	/* Reset to read */
	FLASH_Reset();
	Read_page = 0;

	if (EEPROM_JobType == EEPROM_COPYDATA)
	{
	  RetVal = FLASH_intGetValidData((_shuge int *)CopyData_ptr, address);
	}
	else
	{
	  RetVal = FLASH_intGetValidData((_shuge int *)Write_ptr, address);
	}

	if (RetVal == FAILED)
	{
	 return FAILED;
	}

	/* Load data from Flash to buffer */
	for(c=0; c < 64; c++)
	{
	  *buffer = *Read_page;
	   buffer++;
	   Read_page++;														
	}

	return COMPLETE;
	  /* End of function */
}

/******************************************************************************
	Function	  	Flash_intGetValidData
-------------------------------------------------------------------------------
	Description 	The function will located Flash physical address of the
					EEPROM logical address provided to the pointer Read_page.

-------------------------------------------------------------------------------
	Returnvalue   	COMPLETE
					FAILED

-------------------------------------------------------------------------------
	Parameters    	Start_Read:
					- The starting address by the pointer to located the EEPROM
					  logical address
					address:
					- EEPROM logical address value

*******************************************************************************/

static int FLASH_intGetValidData (_shuge int *Start_Read, uword address)
{
	_shuge int *Address_ptr;
	_shuge int *End_ptr;
	ulong tempAdd;

	Address_ptr = (_shuge int *)(((ulong)Start_Read) - 0x02);

	address <<= VALIDITY_BIT;
	address |= VALIDITY_VALUE;

	End_ptr = (_shuge int *)(((ulong)ActiveBlock_ptr) - 0x02);

	while (Address_ptr != End_ptr)
	{
	  tempAdd = *Address_ptr;
	  tempAdd &= MASKING_VALUE;

	  if(tempAdd == address)
	  {
		Read_page = (_shuge int *)(((ulong)Address_ptr) - 0x7E);
		return COMPLETE;
	  }
	  Address_ptr = (_shuge int *)(((ulong) Address_ptr) - 0x80);
	}
	return FAILED;
  
}

/******************************************************************************
	Function	  	Eeprom_vGetActiveBlock (void)
-------------------------------------------------------------------------------
	Description  	This function retrieve the latest active data block and
					set the address for Write Pointer during the EEPROM
					initialisation.

-------------------------------------------------------------------------------
	Returnvalue   	None

-------------------------------------------------------------------------------
	Parameters    	None

*******************************************************************************/

static int EEPROM_intGetActiveBlock (void)
{
	_shuge int *Address_ptr;
	_shuge int *Block_ptr;

	int checkAdd;
	char ValidBlock;
	char End_Block_Address;
	char Write_Address;

	/* Set the pointer to last data block */
	ActiveBlock_ptr = (_shuge char *)(END_ADDRESS - EEPROM_BLOCK_SIZE);

	/* Set Block_ptr to point at the header bytes of the EEPROM page */
	Block_ptr = (_shuge int *) (ActiveBlock_ptr + 0x7E);
	Address_ptr = NULL;

	End_Block_Address = FALSE;
	ValidBlock = FALSE;
	Write_Address =  FALSE;

	while(End_Block_Address != TRUE)
	{
	  checkAdd = *Block_ptr;
	  if(checkAdd != NULL)
	  {
		/* Set Address_ptr to the last EEPROM page of the data block*/
		Address_ptr = (_shuge int *)(((ulong)Block_ptr + EEPROM_BLOCK_SIZE) - 0x80);

		checkAdd = *Address_ptr;
		if(checkAdd != NULL)
		{
		  NextActiveBlock = ((_shuge char *)((ulong)ActiveBlock_ptr + EEPROM_BLOCK_SIZE));
		  if ((ulong)NextActiveBlock == END_ADDRESS)
		  {
			NextActiveBlock = (_shuge char *)EEPROM_START_ADDRESS;
		  }
		  /* Check if next data block is empty*/
		  Block_ptr = (_shuge int *) (NextActiveBlock + 0x7E);
		  checkAdd = *Block_ptr;
		  if(checkAdd == NULL)
		  {
		    End_Block_Address = TRUE;
		    ValidBlock = TRUE;
		  }
		  else if ((ulong)ActiveBlock_ptr == EEPROM_START_ADDRESS)
	 	  {
	        End_Block_Address = TRUE;
		    ValidBlock = FALSE;
	  	  }
		  else
		  { /* Update pointer to next data block*/
		    ActiveBlock_ptr = (_shuge char *) (((ulong)ActiveBlock_ptr) - EEPROM_BLOCK_SIZE);
			Block_ptr = (_shuge int *) (ActiveBlock_ptr + 0x7E);
		  }
		}
		else
		{ /* if last EEPROM page of the data block is empty, active block found */
		  End_Block_Address = TRUE;
		  ValidBlock = TRUE;
		}
	  }
	  else if ((ulong)ActiveBlock_ptr == EEPROM_START_ADDRESS)
	  { /* if pointer reach first data block, no active data block found */
	    End_Block_Address = TRUE;
		ValidBlock = FALSE;
	  }
	  else
	  { /* Update pointer to next data block */
 		ActiveBlock_ptr = (_shuge char *) (((ulong)ActiveBlock_ptr) - EEPROM_BLOCK_SIZE);
		Block_ptr = (_shuge int *) (ActiveBlock_ptr + 0x7E);
	  }
	} 

	/* find empty EEPROM page for next write if active block found */
	if (ValidBlock == TRUE)
	{
	  Block_ptr = (_shuge int *) (ActiveBlock_ptr + 0x7E);
	  
	  while ((Write_Address != TRUE) && (Address_ptr != Block_ptr))
	  {				
		checkAdd = *Address_ptr;
	    if(checkAdd != NULL)
	    {
		  Write_ptr = (_shuge char *)(((ulong)Address_ptr) + 0x02);
		  Write_Address = TRUE;
	    }
		else
		{
		  Address_ptr = (_shuge int *)(((ulong)Address_ptr) - 0x80);
		}
	  }
	  if (Address_ptr == Block_ptr)
	  {
		Write_ptr = (_shuge char *)(((ulong)Address_ptr) + 0x02);
	  }
	}
	else
	{
	  ActiveBlock_ptr = ((_shuge char *)EEPROM_START_ADDRESS);
	  Block_ptr = (_shuge int *) (ActiveBlock_ptr + 0x7E);
	  checkAdd = *Block_ptr;
	  if(checkAdd == NULL)
	  {
		  Write_ptr = ((_shuge char *)EEPROM_START_ADDRESS);
		  return DATA_EMPTY;
	  }
	  else
	  {
		  return INVALID;
	  }
	}
	return VALID;
}

/******************************************************************************
	Function	  	EEPROM_intCheckAndFixed (void)
-------------------------------------------------------------------------------
	Description		This function will check if the current active block have
					all the EEPROM logical address. If no, it will search on the
					previous data block. If previous data block have all EEPROM
					logical address, the current active block will be deleted
					and the previous block become the active block.
-------------------------------------------------------------------------------
	Returnvalue   	None

-------------------------------------------------------------------------------
	Parameters    	None

*******************************************************************************/

static int EEPROM_intCheckAndFixed (void)
{
int EEPROMData;
int LastBlockData;
ulong tempACT;
ulong tempWRT;

  /* Check if all EEPROM logical address is available inside active block */
  EEPROMData = EEPROM_intCheckValidity ((_shuge int *)Write_ptr);
  if (EEPROMData == INVALID)
  {
	 tempACT = (ulong)ActiveBlock_ptr;
	 tempWRT = (ulong)Write_ptr;

	 /* Get the previous data block address */
	 if (tempACT == EEPROM_START_ADDRESS)
	 {
	   ActiveBlock_ptr = (_shuge char *)(END_ADDRESS - EEPROM_BLOCK_SIZE);
	   Write_ptr = (_shuge char *)END_ADDRESS;
	 }
	 else
	 {
	   ActiveBlock_ptr = (_shuge char *)(((ulong)ActiveBlock_ptr) - EEPROM_BLOCK_SIZE);
	   Write_ptr = (_shuge char *) tempACT;
	 }

	 /* Check if previous data block have all the EEPROM logical address available */
	 LastBlockData = EEPROM_intCheckValidity ((_shuge int *)Write_ptr);

	 if (LastBlockData == VALID)
	 { /* Erase current active block if all EEPROM logical address
		            is available in previous block */
	   EEPROM_vEraseBlock (tempACT);
	   while (EEPROM_JobType != IDLE)
	   {
	    while(FLASH_BUSY_STATUS != 0);
		EEPROM_vEraseReturn();
	   }
	   return VALID;
	 }
	 else
	 {
	   Write_ptr = (_shuge char *)tempWRT;
	   ActiveBlock_ptr = (_shuge char *)tempACT;
	   return INVALID;
	 }
  }
  else
  {
    return VALID;
  }

}

/******************************************************************************
	Function	  	EEPROM_vCheckPointer (void)
-------------------------------------------------------------------------------
	Description  	This function shall identify whether the Write Pointer has
					reach the last block of the sector or the end of the
					emulation sector.
					If Write Pointer reach the last EEPROM page of the data block,
					the latest emulation data set shall be copy to the next data
					block.
					If the Write Pointer reach the end of the emulation sector,
					it wrap around and start to copy the latest emulation data set
					to the start address data block.

-------------------------------------------------------------------------------
	Returnvalue   	None

-------------------------------------------------------------------------------
	Parameters    	None

*******************************************************************************/

static void EEPROM_vCheckPointer (void)
{ 

    /* Check if the write pointer pointed to the End Address */	
	if (((ulong)Write_ptr) == END_ADDRESS)
	{
	  while (Erase_Status != CLEAR)
	  {
	  	EEPROM_vEraseDataBlock();
		while (FLASH_BUSY_STATUS != 0);		 
	  }
	  
	  Copy_Type = COPY_END_ADDRESS;
	  CopyData_ptr = Write_ptr;
	  Write_ptr = (_shuge char *)EEPROM_START_ADDRESS;

	  while (FLASH_BUSY_STATUS != 0);

	  EEPROM_JobType = EEPROM_COPYDATA;

	  Address_Count = 0;
	  EEPROM_vCopyData();
	}	
	
	/* Check if the write pointer pointed to the next emulation block */
	else if (Write_ptr == NextActiveBlock)
	{
	  Copy_Type = COPY_NEXT_BLOCK;
	  CopyData_ptr = Write_ptr;

	  while (FLASH_BUSY_STATUS != 0);

	  EEPROM_JobType = EEPROM_COPYDATA;

	  Address_Count = 0;
	  EEPROM_vCopyData(); 
	}
	 /* End of function */
}

/******************************************************************************
	Function	  	Eeprom_vCopyData (void)
-------------------------------------------------------------------------------
	Description  	This function will located the latest emulation data set
					and write to the next data block. Upon finish copying the
					whole emulation data set, the function will update the
					active block address.

-------------------------------------------------------------------------------
	Returnvalue   	None

-------------------------------------------------------------------------------
	Parameters    	None

*******************************************************************************/

static void EEPROM_vCopyData (void)
{
  uword CopyBuffer[64];
  uword *Buffer_ptr;
  int RetVal;

  Buffer_ptr = &CopyBuffer[0];
  /* Located the latest emulation data set and write to the next data block */
  if(Address_Count <= TOTAL_PAGE)
  {
    RetVal = EEPROM_intRead(Buffer_ptr, Address_Count);
    if (RetVal == COMPLETE)
    {
	  EEPROM_WriteRoutine(Buffer_ptr, Address_Count);
    }
	else
	{
	  Write_ptr += 0x80;
	  EEPROM_vIntProcess();
	}
  }

  else
  /* Update the ActiveBlock_ptr to the next data block address */
  {
   while(FLASH_BUSY_STATUS != 0);
   PreviousBlock_ptr = ActiveBlock_ptr;

   if (Copy_Type == COPY_END_ADDRESS)
   { 
	 ActiveBlock_ptr = (_shuge char *)EEPROM_START_ADDRESS;
	 NextActiveBlock = (_shuge char *)(EEPROM_START_ADDRESS + EEPROM_BLOCK_SIZE);

   }
   else if(Copy_Type == COPY_NEXT_BLOCK)
   {
	  ActiveBlock_ptr = NextActiveBlock;
	  NextActiveBlock =	 (_shuge char *)(((ulong)ActiveBlock_ptr) + EEPROM_BLOCK_SIZE);
   }
   EEPROM_JobType = IDLE; 
  }
}

/******************************************************************************
	Function	  	Eeprom_vIntProcess (void)
-------------------------------------------------------------------------------
	Description  	This function update the necessary variable status and
					pointer address after write operation or copy data operation.

-------------------------------------------------------------------------------
	Returnvalue   	None

-------------------------------------------------------------------------------
	Parameters    	None

*******************************************************************************/

void EEPROM_vIntProcess (void)
{
  if (EEPROM_JobType == EEPROM_COPYDATA)
  {
	/* Update Write pointer address to the next write address*/
	Write_ptr = (_shuge char *)(((ulong) Write_ptr) + 0x80);
	Address_Count++;
	EEPROM_vCopyData();
	/* Update Erase_Status variable when finish data copying process */
	if ((EEPROM_JobType == IDLE)&& (Erase_Status == CLEAR))
	{
		Erase_Status = ERASE;
		Erase_ptr = PreviousBlock_ptr;
	}
  }

  else if (EEPROM_JobType == EEPROM_WRITE)
  {
	/* Update Write pointer address to the next write address */
	Write_ptr = (_shuge char *)(((ulong) Write_ptr) + 0x80);
	EEPROM_vCheckPointer();
	if (EEPROM_JobType == EEPROM_WRITE)
	{
	  EEPROM_JobType = IDLE;
	}																			 
  }
  
}

/******************************************************************************
	Function	  	EEPROM_intCheckValidity
-------------------------------------------------------------------------------
	Description  	Check the current active block whether have a complete
					emulation data set

-------------------------------------------------------------------------------
	Returnvalue   	VALID
					INVALID

-------------------------------------------------------------------------------
	Parameters    	None

*******************************************************************************/

static int EEPROM_intCheckValidity (int _shuge *Checkptr)
{
  int RetVal;
  uword Address;

  for (Address = 0; Address <= TOTAL_PAGE; Address++)
  { 
    RetVal = FLASH_intGetValidData (Checkptr, Address);
	if(RetVal == FAILED)
	{
	  return INVALID;
	}
  }
  return VALID;  
}

/******************************************************************************
	Function	  	EEPROM_vIdentifyPage (void)
-------------------------------------------------------------------------------
	Description  	This function check if the next write address is empty.
					If the next write address is not empty, Write_ptr will be
					update to next EEPROM page.

-------------------------------------------------------------------------------
	Returnvalue   	None

-------------------------------------------------------------------------------
	Parameters    	None

*******************************************************************************/

static void EEPROM_vIdentifyPage (void)
{
  _shuge int *check_ptr;
  uword checksum;
  ubyte Status;
  ubyte	c;
  int checkBuffer[64];

  FLASH_Reset();

  	Status = FALSE;
	  
	while (Status == FALSE)
	{
	    check_ptr = (_shuge int *) Write_ptr;
 	    checksum = 0;
	
	    /*  Load data on the Flash pointed by Write Pointer
		    and check whether is empty or not. 				*/
	    for(c=0; c < 64; c++)
		{
		  checkBuffer[c] = *check_ptr;			 
		  checksum = checksum + checkBuffer[c];
		  check_ptr++;
		}

	    if (checksum == 0)															  
		{
		  Status = TRUE;
		}

	    else 
		{
		  Write_ptr = (_shuge char *)(((ulong) Write_ptr) + 0x80);
		  EEPROM_vCheckPointer();
		}
	}
	/* End of function */ 

}

/******************************************************************************
	Function	  	EEPROM_vEraseInactiveBlock (void)
-------------------------------------------------------------------------------
	Description  	This function erase all inactive data blocks

-------------------------------------------------------------------------------
	Returnvalue   	None

-------------------------------------------------------------------------------
	Parameters    	None

*******************************************************************************/	

static void EEPROM_vEraseInactiveBlock (void)
{
  ulong InactiveBlock_Address;
  ulong Activeblock_Address;

  InactiveBlock_Address = (ulong) EEPROM_START_ADDRESS;
  Activeblock_Address = (ulong)ActiveBlock_ptr;

  if ((ulong)Write_ptr == EEPROM_START_ADDRESS)
  {
    EEPROM_vEraseBlock (EEPROM_START_ADDRESS); 
  }

  while (InactiveBlock_Address != END_ADDRESS)
  {
   
   if ((InactiveBlock_Address != Activeblock_Address))
   {
     EEPROM_vEraseBlock (InactiveBlock_Address);
	 while (EEPROM_JobType != IDLE)
	 {
	   while(FLASH_BUSY_STATUS != 0);
	   EEPROM_vEraseReturn();
	 }
   }

   InactiveBlock_Address = InactiveBlock_Address + EEPROM_BLOCK_SIZE;
  }

}

/******************************************************************************
	Function	  	EEPROM_vEraseDataBlock (void)
-------------------------------------------------------------------------------
	Description  	This function is called by user to erase filled data blocks 
					when Erase_Status == ERASE

-------------------------------------------------------------------------------
	Returnvalue   	None

-------------------------------------------------------------------------------
	Parameters    	None

*******************************************************************************/

void EEPROM_vEraseDataBlock (void)
{
 if (EEPROM_JobType == IDLE)
 {
	if (Erase_Status == ERASE)
    {
	EEPROM_JobType = EEPROM_ERASE;
    Erase_Status = START;
	Erase_Address = (ulong)Erase_ptr;
	EEPROM_vUserEraseReturn();
    }
  
    else if (Erase_Status == START)
    {
    EEPROM_JobType = EEPROM_ERASE;
    Erase_Address = Erase_Address + 0x1000;
	Erase_ptr = (_shuge char *) Erase_Address;
	EEPROM_vUserEraseReturn();
    }
 }
}
 

/******************************************************************************
	Function	  	EEPROM_vEraseBlock (unsigned long sector)
-------------------------------------------------------------------------------
	Description  	Erase function is used on the initialisation by the driver.

-------------------------------------------------------------------------------
	Returnvalue   	None

-------------------------------------------------------------------------------
	Parameters    	None

*******************************************************************************/

static void EEPROM_vEraseBlock (unsigned long sector)
{
  while(FLASH_BUSY_STATUS != 0);

  Erase_Address = sector;
  EEPROM_JobType = STATIC_EEPROM_ERASE;
  Erase_Status = START;
  Address_Count = 1;

  FLASH_Reset();
  EEPROM_vEraseReturn();
}

/******************************************************************************
	Function	  	EEPROM_vUserEraseReturn (void)
-------------------------------------------------------------------------------
	Description  	This function will erase the data blocks until Erase_Address
					reach Active Block address

-------------------------------------------------------------------------------
	Returnvalue   	None

-------------------------------------------------------------------------------
	Parameters    	None

*******************************************************************************/

static void EEPROM_vUserEraseReturn (void)
{
  ulong End_of_Erase;

  End_of_Erase = (ulong)ActiveBlock_ptr;

  if(Erase_Address == END_ADDRESS)
  {
    Erase_Address = EEPROM_START_ADDRESS;
  }
  if(Erase_Address != End_of_Erase)
  {
	  /* Start Flash erase process */
	  FLASH_vErase (Erase_Address);
	  EEPROM_JobType = IDLE;
  }

  else /* When all filled data blocks is erased */
  {
    EEPROM_JobType = IDLE;
	Erase_Status = CLEAR;
	Erase_ptr = NULL;
  }
 
}

/******************************************************************************
	Function	  	EEPROM_vEraseReturn (void)
-------------------------------------------------------------------------------
	Description  	This function is used to update Erase_address for erasing
					data blocks during initialisation

-------------------------------------------------------------------------------
	Returnvalue   	None

-------------------------------------------------------------------------------
	Parameters    	None

*******************************************************************************/

static void EEPROM_vEraseReturn (void)
{
  if (Address_Count != 1)
  {
    Erase_Address = Erase_Address + 0x1000;
  }

  if(Address_Count <= TOTAL_BLOCK)
  {
	FLASH_vErase (Erase_Address);
  }
  else
  {
    EEPROM_JobType = IDLE;
	Erase_Status = CLEAR;
  }
  Address_Count++;
}


/******************************************************************************
	Function	  	FLASH_Reset (void)
-------------------------------------------------------------------------------
	Description  	This function contains the "Reset to Read" command cycles.

-------------------------------------------------------------------------------
	Returnvalue   	None

-------------------------------------------------------------------------------
	Parameters    	None

*******************************************************************************/

static void FLASH_Reset (void)
{
	_shuge int *Flash = (_shuge int *) 0x0c000AA;

	/* Reset to read */
	while(FLASH_BUSY_STATUS != 0);
	*Flash = 0x00F0;

	/* End of function */
}

/******************************************************************************
	Function	  	int EEPROM_intCheckBusy (void)
-------------------------------------------------------------------------------
	Description  	This function return EEPROM_JobType status. This can be
					used to indicated whether the EEPROM driver is BUSY or IDLE
	 
-------------------------------------------------------------------------------
	Returnvalue   	IDLE
					EEPROM_WRITE
					EEPROM_ERASE
					EEPROM_COPYDATA
-------------------------------------------------------------------------------
	Parameters    	None

*******************************************************************************/

int EEPROM_intCheckBusy (void)
{
  return (EEPROM_JobType);
/* End of function */
}

