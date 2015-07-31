/*
* This is a part from MAIN.C
*
*/

//****************************************************************************
// @Module        Project Settings
// @Filename      MAIN.C
// @Project       stick_xe164.dav
//----------------------------------------------------------------------------
// @Controller    Infineon XE164F-96F66
//
// @Compiler      Tasking
//
// @Codegenerator 0.2
//
// @Description   This file contains the project initialization function.
//
//----------------------------------------------------------------------------
// @Date          20.12.2007 12:05:15
//
//****************************************************************************

// USER CODE BEGIN (MAIN_General,1)

// USER CODE END



//****************************************************************************
// @Project Includes
//****************************************************************************

//#include "SCS.H"
#include "my_SCS.H"
#include "dave/MAIN.H"

// USER CODE BEGIN (MAIN_General,2)
void DAVE_vUnlockProtecReg(void);
void DAVE_vLockProtecReg(void);
void DAVE_vChangeFreq(void);
// USER CODE END


//****************************************************************************
// @Macros
//****************************************************************************

// USER CODE BEGIN (MAIN_General,3)

// USER CODE END


//****************************************************************************
// @Defines
//****************************************************************************

// USER CODE BEGIN (MAIN_General,4)

// USER CODE END


//****************************************************************************
// @Typedefs
//****************************************************************************

// USER CODE BEGIN (MAIN_General,5)

// USER CODE END


//****************************************************************************
// @Imported Global Variables
//****************************************************************************

// USER CODE BEGIN (MAIN_General,6)

// USER CODE END


//****************************************************************************
// @Global Variables
//****************************************************************************

// USER CODE BEGIN (MAIN_General,7)

// USER CODE END


//****************************************************************************
// @External Prototypes
//****************************************************************************

// USER CODE BEGIN (MAIN_General,8)

// USER CODE END


//****************************************************************************
// @Prototypes Of Local Functions
//****************************************************************************

// USER CODE BEGIN (MAIN_General,9)

// USER CODE END


//****************************************************************************
// @Function      void MAIN_vInit(void) 
//
//----------------------------------------------------------------------------
// @Description   This function initializes the microcontroller.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          20.12.2007
//
//****************************************************************************

// USER CODE BEGIN (Init,1)

// USER CODE END

void DAVE_vInit(void)
{
  // USER CODE BEGIN (Init,2)

  // USER CODE END

  ///  -----------------------------------------------------------------------
  ///  Configuration of the System Clock:
  ///  -----------------------------------------------------------------------
  ///  - VCO clock used, input clock is connected
  ///  - input frequency is 8 MHz
  ///  - system clock is 66.00 MHz

  DAVE_vUnlockProtecReg();     // unlock write security

  DAVE_vChangeFreq();          // load PLL control register(Fsys = 80/66Mhz)
  
  DAVE_vLockProtecReg();

} //  End of function MAIN_vInit


//****************************************************************************
// @Function      void MAIN_vUnlockProtecReg(void) 
//
//----------------------------------------------------------------------------
// @Description   This function makes it possible to write one protected 
//                register.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          20.12.2007
//
//****************************************************************************

// USER CODE BEGIN (UnlockProtecReg,1)

// USER CODE END

void DAVE_vUnlockProtecReg(void)
{
  uword uwPASSWORD;

    SCU_SLC = 0xAAAA;                   // command 0
    SCU_SLC = 0x5554;                   // command 1

    uwPASSWORD = SCU_SLS & 0x00FF;
    uwPASSWORD = (~uwPASSWORD) & 0x00FF;

    SCU_SLC = 0x9600 | uwPASSWORD;      // command 2
    SCU_SLC = 0x0000;                   // command 3

} //  End of function MAIN_vUnlockProtecReg


//****************************************************************************
// @Function      void MAIN_vLockProtecReg(void) 
//
//----------------------------------------------------------------------------
// @Description   This function makes it possible to lock one protected 
//                register.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          20.12.2007
//
//****************************************************************************

// USER CODE BEGIN (LockProtecReg,1)

// USER CODE END

void DAVE_vLockProtecReg(void)
{
  uword uwPASSWORD;

    SCU_SLC = 0xAAAA;                   // command 0
    SCU_SLC = 0x5554;                   // command 1

    uwPASSWORD = SCU_SLS & 0x00FF;
    uwPASSWORD = (~uwPASSWORD) & 0x00FF;

    SCU_SLC = 0x9600 | uwPASSWORD;      // command 2
    SCU_SLC = 0x1800;                   // command 3; new PASSWOR is 0x00

    uwPASSWORD = SCU_SLS & 0x00FF;
    uwPASSWORD = (~uwPASSWORD) & 0x00FF;
    SCU_SLC = 0x8E00 | uwPASSWORD;      // command 4

} //  End of function MAIN_vLockProtecReg


//****************************************************************************
// @Function      void MAIN_vChangeFreq(void) 
//
//----------------------------------------------------------------------------
// @Description   This function is used to select the external crystal and
//                configure the system frequency to 80Mhz/66Mhz.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          20.12.2007
//
//****************************************************************************

// USER CODE BEGIN (ChangeFreq,1)

// USER CODE END

void DAVE_vChangeFreq(void)
{
  SCS_SwitchToHighPrecBandgap();
  SCS_StartXtalOsc(1);                             // Starts the crystal oscillator
  SCS_SwitchSystemClock(1);                        // System clock is increased to target speed (80/66 MHz)

} //  End of function MAIN_vChangeFreq


