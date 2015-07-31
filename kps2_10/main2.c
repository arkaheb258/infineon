/* header of standard C - libraries */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* header of project specific types 
*  Enable hardware specific part for SET_BIT(), RESET_BIT() definition.
*/
#define DEF_HW_PART
#include <cal_conf.h>      /* !!! first file to include for all CANopen */
#ifdef CONFIG_CPU_FAMILY_LINUX
# include <unistd.h>
# include <time.h>
#endif

#include <co_acces.h>
#include <co_sdo.h>
#include <co_pdo.h>
#include <co_drv.h>
#include <co_lme.h>
#include <co_nmt.h>
#include <co_init.h>

/* examples.h is part of the CANopen driver 
 * and contains also the macros
 * PRINTF() for printf()
 * PUTCHAR() for putchar()
 * FFLUSH() for fflush()
 */
#include <examples.h>
#include "objects.h"            /* object dictionary */
#include <dave\io.h>
#include <kps_4_x.h>


/* constant definitions
---------------------------------------------------------------------------*/
//#define NODE_ID         12        /* ID of local node */


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

UNSIGNED16	bitRate = 125;   /* need for example initCan() */
UNSIGNED8	lNodeId = 32;   /* need for example getNodeId() */
/* list of local defined functions
---------------------------------------------------------------------------*/

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/
int		co_debug;


/* local defined variables
---------------------------------------------------------------------------*/

#ifdef CONFIG_RCS_IDENT
static char _rcsid[] = "$Id: main.c,v 1.33 2009/05/12 09:41:40 ro Exp $";
#endif




/* M A I N F U N K T I O N */

/* 
* Hint:
* For compilers, that need a 
* void main(void);
* prototype, change 'int' to 'void' 
* and remove the return value at the end
* of the main()-function.
*/

uword uwFIFO_SendData[4]; 
uword uwFIFO_RecData[2] = {0}; 
UNSIGNED8	  test_blink = 0;

INTEGER16 read_temp_iic(void)
{ 
	ulong time = 0;
//	UNSIGNED8 err = 0;
	INTEGER16 temp = 0;
	ubyte ubReadData = 0; 
	ubyte ubFIFOLevel = 2; 
	uword uwSlAdr = 0x48; // Address A0 - A2 = 0 
	uwSlAdr = U2C0_IIC_vSlaveAddrFormat(uwSlAdr);

	uwFIFO_SendData[0]= (((U2C0TDF_MStart << 8) & 0x0700) | ((uwSlAdr + U2C0IIC_READ)& 0x00FF)); // Read mode 
	uwFIFO_SendData[1]= ((U2C0TDF_MRxAck0 << 8) & 0x0700); // TDF_Ack0 
	uwFIFO_SendData[2]= ((U2C0TDF_MRxAck1 << 8) & 0x0700); // TDF_Ack1
	uwFIFO_SendData[3]= ((U2C0TDF_MStop << 8) & 0x0700); // TDF_Stop 

	while(!U2C0_IIC_ubIsTxFIFOempty()); // Wait for Tx FIFO Empty 
	while(U2C0_IIC_ubIsTxFIFObusy()); // Wait for TxFIFO Not Busy 
//    PRINTF("tfl1 0x%02x\n",(int)U2C0_IIC_ubGetTxFIFOFillingLevel());
	U2C0_IIC_vFlushTxFIFO(); // clear the transmit FIFO 
	U2C0_IIC_vFillTxFIFO(uwFIFO_SendData , 4); 
//    PRINTF("sr %d\n",U2C0_IIC_uwGetStatus());
//	U2C0_IIC_uwGetStatus();
	time = RTC_ulGetTime();
	while (U2C0_IIC_ubGetRxFIFOFillingLevel() != 2){ // Wait for Two Data's 
		if (RTC_ulGetTime() - time > 10){
			U2C0_IIC_vFlushTxFIFO(); // clear the transmit FIFO 
			return 0;
		}
	}
//    PRINTF("tfl3 0x%02x\n",(int)U2C0_IIC_ubGetTxFIFOFillingLevel());
	while (ubReadData != ubFIFOLevel) // Read Two Data's 
	 { 
	 while (U2C0_IIC_ubIsRxFIFObusy()); // Wait for RxFIFO Not busy 
	 uwFIFO_RecData[ubReadData] = U2C0_IIC_uwGetRxFIFOData(); // Get the Received Data 
	 ubReadData++; 
	 } 
	U2C0_IIC_vFlushRxFIFO(); 
	while(!U2C0_IIC_ubIsRxFIFOempty()); // Wait for Tx FIFO Empty 
	temp = (uwFIFO_RecData[0] & 0x00FF) << 1;
	temp |= ((uwFIFO_RecData[1] & 0x0080) >> 7);
	temp *= 5;
	return temp;
} 

INTEGER16 write_test(UNSIGNED8 a, UNSIGNED8 b)
{ 
	INTEGER16 temp = 0;
	ubyte ubReadData = 0; 
	ubyte ubFIFOLevel = 2; 
	uword uwSlAdr = 0x17;
	uwSlAdr = U2C0_IIC_vSlaveAddrFormat(uwSlAdr);

	U2C0_IIC_vFlushTxFIFO(); // clear the transmit FIFO 
	U2C0_IIC_vWriteData(U2C0TDF_MStart, (uwSlAdr | U2C0IIC_WRITE));
	while (U2C0_IIC_ubGetRxFIFOFillingLevel() != 2){ // Wait for Two Data's 
	    PRINTF("rfl 0x%02x\n",(int)U2C0_IIC_ubGetRxFIFOFillingLevel());
	    PRINTF("tfl2 0x%02x\n",(int)U2C0_IIC_ubGetTxFIFOFillingLevel());
	    PRINTF("s 0x%02x\n",(int)U2C0_IIC_uwGetStatus());
		U2C0_IIC_vFlushTxFIFO(); // clear the transmit FIFO 
		if (1)
			break;
	}
	U2C0_IIC_vWriteData(U2C0TDF_MRxAck0, 0);
	while (U2C0_IIC_ubGetRxFIFOFillingLevel() != 2){ // Wait for Two Data's 
	    PRINTF("rfl 0x%02x\n",(int)U2C0_IIC_ubGetRxFIFOFillingLevel());
	    PRINTF("tfl2 0x%02x\n",(int)U2C0_IIC_ubGetTxFIFOFillingLevel());
	    PRINTF("s 0x%02x\n",(int)U2C0_IIC_uwGetStatus());
		U2C0_IIC_vFlushTxFIFO(); // clear the transmit FIFO 
		if (1)
			break;
	}
	U2C0_IIC_vWriteData(U2C0TDF_MStop, 0);

	uwFIFO_RecData[ubReadData] = U2C0_IIC_uwReadData();
    PRINTF("rfl 0x%02x\n",(int)uwFIFO_RecData[ubReadData]);
	ubReadData++;

	temp = (uwFIFO_RecData[0] & 0x00FF) << 1;
	temp |= ((uwFIFO_RecData[1] & 0x0080) >> 7);
	temp *= 5;
	return temp;
} 

void safe_state(UNSIGNED8 blink){
	UNSIGNED8 i; 
	for (i=1;i<4;i++)
		p401_write_output_16_bit[i] = 0;
	for (i=1;i<31;i++)
		p401_write_analogue_output_16_bit[i] = 0;
	test_blink = blink; 
}

UNSIGNED8 blink(UNSIGNED16 time, UNSIGNED8 hz){
//	long width = 50;
	long temp1;
	long temp2;
	temp1 = 500/hz;
	temp2 = 1000/hz;
	if ((time%(temp2)) > temp1)
		return 0;
	else
		return 1;
}

int main(CO_EXAMPLE_ARGS_DECL)
/* void main(CO_EXAMPLE_ARGS_DECL) */
{
	RET_T         commonRet;      /* return value for CANopen functions */
	UNSIGNED8     ret;            /* return value for common purpose */
	BOOL_T        err = CO_FALSE; /* error flag */
	//UNSIGNED8	  bitRate_temp = 3;
	UNSIGNED8	  temp_uc = 0;
	UNSIGNED16	  main_type_out = 0;
	//UNSIGNED8	  temp_type = 0;
	UNSIGNED8	  diagn_mode = 0;
	UNSIGNED8	  switch_BR = 0;
	UNSIGNED8	  switch_ID1 = 0;
	UNSIGNED8	  switch_ID2 = 0;
	UNSIGNED8	  temp = 0;
	UNSIGNED8	  i = 0;
	UNSIGNED8	  type_U0C1 = 0;
	UNSIGNED8	  type_U1C0 = 0;
	UNSIGNED8	  type_U1C1 = 0;
	UNSIGNED8	  type_U2C1 = 0;
	UNSIGNED8	  type_U3C0 = 0;
	UNSIGNED8	  type_U3C1 = 0;
	INTEGER16	i16_temp = 0;
	ulong time = 0;
	ulong last_temp_read = 0;
	UNSIGNED8	  yel_led = 0;
	UNSIGNED8	  conf_err = 0;

    /*--- hardware initialization e.g SIO, Chip-Selects, ...----------*/
    ret = iniDevice();
    PRINTRET("iniDevice: 0x%02x\n", (int)ret);


  //   initializes the Parallel Ports
	IO_vInit();
  //   initializes the USICx Modules
	USIC0_vInit();
	USIC1_vInit();
	USIC2_vInit();
	USIC3_vInit();

    /* special options for current target
     * if available use command line arguments to set bit rate
     * or CANopen node ID
     */

//RTC init
    DAVE_vUnlockProtecReg();
	RTC_vInit();
    NOP();  /* one cycle delay */
    NOP();  /* one cycle delay */
    DAVE_vLockProtecReg();

//IIC enable Power
	IO_vResetPin(IO_P10_5_IIC_ENP);
	IO_vSetPin(IO_P10_4_IIC_EN);
    
//    lNodeId = NODE_ID;
//	main_type_out = (IO_uwReadPort(P5) & 0x2000) == 0x2000;
//	if (main_type_out)
//	    PRINTF("wyjscia\n");
//	else
//	    PRINTF("wejscia\n");

	lNodeId = (~IO_uwReadPort(P15))&0x7F;
	switch_ID1 = (~IO_uwReadPort(P15))&0x0F;
	switch_ID2 = (~IO_uwReadPort(P15))&0xF0;
	if ((~IO_uwReadPort(P15))&0x80)
		lNodeId = 127;
    setOptions(CO_EXAMPLE_ARGS);

    /*--- initialize CAN controller -----------------------------------*/
    /*   address, rate, osc.frequ., ... , OutputControl */
    /*
     * CAN_START_BIT_RATE = 0       : read bit timing from init file
     *                          (if possible - not supported from all drivers)
     * CAN_START_BIT_RATE = bitRate : use local bitRate variable
     */
//	bitRate_temp = ;
	switch_BR = (~IO_uwReadPort(P10))&0x0F;
	switch(switch_BR){
		case 0: bitRate = 10; break;
		case 1: bitRate = 20; break; 
		case 2: bitRate = 50; break; 
		case 3: bitRate = 125; break; 
		case 4: bitRate = 250; break; 
		case 5: bitRate = 500; break; 
//		case 6: bitRate = 800; break; 
//		case 7: bitRate = 1000; break;
		case 0x9:
		case 0xF:
			bitRate = 125;
			if (!switch_ID1 && !switch_ID2){
				diagn_mode = 1;
				lNodeId = 127;
			}
		break;
//		default: bitRate = 1000; break;
		default: bitRate = 500; break;
	}

    ret = initCan(bitRate);
//    PRINTRET("initCan: 0x%02x\n", (int)ret);

    /*--- inititializing of CANopen -----------------------------------*/
    /* defines also the Network control Object -- NMT
     * reset communication and goes to the				   
     * state preoperational + Initialization of CANopen
     */
    commonRet = init_Library();
//    PRINTF("bitRate  %d\n", (int)bitRate);
//    PRINTF("Node ID  %d\n", (int)lNodeId);

    /*
     * timer is needed for inhibit time, heartbeat and
     * host lifeguarding (if life guarding is supported)
     */
    initTimer();

    Start_CAN();
    ENABLE_CPU_INTERRUPTS();

	type_U0C1 = KPS_check(U0C1) & 0xFF;
	type_U3C1 = KPS_check(U3C1) & 0xFF;
	type_U1C1 = KPS_check(U1C1) & 0xFF;
	type_U2C1 = KPS_check(U2C1) & 0xFF;
	type_U3C0 = KPS_check(U3C0) & 0xFF;
	type_U1C0 = KPS_check(U1C0) & 0xFF;

	if ((type_U3C1 == 0x41) && (type_U2C1 == 0x41)){
		if ((type_U1C0 == 0x42)) 
			main_type_out = 0x0112;
		else if (type_U1C0 == 0x40 && diagn_mode)
			main_type_out = 0x0112;
	} else if((type_U0C1 == 0x51)
		&& (type_U1C1 == AD779X_AD7794)
		&& (type_U2C1 == AD779X_AD7795)
		&& (type_U3C0 == AD779X_AD7794)
		&& (type_U1C0 == AD779X_AD7795)){
			main_type_out = 0x0344;
	}

	p401_fault_mode_8[1] = type_U0C1;
	p401_fault_mode_8[2] = type_U3C1;
	p401_fault_mode_8[3] = type_U1C1;
	p401_fault_mode_8[4] = type_U2C1;
	p401_fault_mode_8[5] = type_U3C0;
	p401_fault_mode_8[6] = type_U1C0;
	p401_fault_mode_8[7] = main_type_out >> 8;
	p401_fault_mode_8[8] = main_type_out;

	if (main_type_out == 0x0112){
		KPS_4_1_init(U3C1);
		KPS_4_1_init(U2C1);
		KPS_4_2_init(U1C0);
	} else if (main_type_out == 0x0344){
		KPS_5_1_init(U0C1);
		KPS_5_2_init(U1C1);
		KPS_5_2_init(U2C1);
		KPS_5_2_init(U3C0);
		KPS_5_2_init(U1C0);
	} else
		conf_err = 1;

	RTC_vSetTime(0,0);
//    PRINTF("CANopen (compiled " __DATE__ ") is ready now\n"
//    "Going into application loop\n");
//    PRINTF("loop\n");
    while (err == CO_FALSE) {
		FlushMbox();		         /* Do the CANopen job */
		switch_ID1 = (~IO_uwReadPort(P15))&0x0F;
		switch_ID2 = ((~IO_uwReadPort(P15))&0xF0) >> 4;
		if (diagn_mode){
			if (switch_ID1){
				p401_write_output_16_bit[1] = 
				p401_write_output_16_bit[2] = 
				p401_write_output_16_bit[3] = 
					1 << (switch_ID1-1);
			} else {
				p401_write_output_16_bit[1] = 
				p401_write_output_16_bit[2] = 
				p401_write_output_16_bit[3] = 
					0;
			}
			
			i16_temp = (switch_ID2%8) << 12;
			if ((switch_ID2%8) == 7) i16_temp = 0x7fff;
			if (switch_ID2 > 8) i16_temp = -i16_temp;
			if (switch_ID1 && switch_ID1 <= 10) {
				p401_write_analogue_output_16_bit[switch_ID1] = 
				p401_write_analogue_output_16_bit[10+switch_ID1] = 
				p401_write_analogue_output_16_bit[20+switch_ID1] = 
			   		i16_temp;
			}
		}

		if (type_U0C1 == 0x51){
			KPS_5_1_loop(U0C1, p401_read_input_16_bit, 1);
			KPS_5_2_namur(U0C1, p401_read_state_bl1_1, 13);
		}
		if (type_U1C1 == AD779X_AD7794){
			KPS_5_2_pt100_loop(U1C1, p401_read_analogue_input_16_bit, 11, 2, p401_fault_mode_8[9]);
			KPS_5_2_namur(U1C1, p401_read_state_bl1_1, 5);
		}
		if (type_U2C1 == AD779X_AD7795){
			KPS_5_2_mA_loop(U2C1, p401_read_analogue_input_16_bit, 15, 6);
			KPS_5_2_namur(U2C1, p401_read_state_bl1_1, 7);
		}
		if (type_U3C0 == AD779X_AD7794){
			KPS_5_2_pt100_loop(U3C0, p401_read_analogue_input_16_bit, 21, 2, p401_fault_mode_8[9]);
			KPS_5_2_namur(U3C0, p401_read_state_bl1_1, 9);
		}
		if (type_U1C0 == AD779X_AD7795){
			KPS_5_2_mA_loop(U1C0, p401_read_analogue_input_16_bit, 25, 6);
			KPS_5_2_namur(U1C0, p401_read_state_bl1_1, 11);
		}
		if (type_U3C1 == 0x41){
			KPS_4_1_loop(U3C1, p401_write_output_16_bit, 1, p401_read_input_16_bit, 4);
		}
		if (type_U2C1 == 0x41){
			KPS_4_1_loop(U2C1, p401_write_output_16_bit, 2, p401_read_input_16_bit, 5);
		}
		if (type_U1C0 == 0x42){
			KPS_4_2_loop(U1C0, p401_write_analogue_output_16_bit, 21);
		}

		p401_read_input_16_bit[10] = 0;
		for (i=0;i<12;i++)
			if (p401_read_state_bl1_1[i+1])
				p401_read_input_16_bit[10] |= (1 << i);
			
		
//dla sprawdzenia karty bez nakladki
		if (type_U1C0 == 0x40 && diagn_mode){				
			if (switch_ID1) {
				p401_write_analogue_output_16_bit[5+switch_ID1] = p401_write_analogue_output_16_bit[0+switch_ID1];
				p401_write_analogue_output_16_bit[15+switch_ID1] = p401_write_analogue_output_16_bit[10+switch_ID1];
				p401_write_analogue_output_16_bit[25+switch_ID1] = p401_write_analogue_output_16_bit[20+switch_ID1];
			}
			KPS_4_2_loop(U1C0, p401_write_analogue_output_16_bit, 21);
		}

		if (RTC_ulGetTime() - last_temp_read > 1000){
			p401_read_anin_manuf_specific[1] = read_temp_iic();
			last_temp_read = RTC_ulGetTime();
		}

		time = RTC_ulGetTime();
		if (diagn_mode)
			yel_led = blink(time & 0xFFFF, 5);
		else if (conf_err)
			yel_led = blink(time & 0xFFFF, 1);
		else if (test_blink > 0)
			yel_led = blink(time & 0xFFFF, test_blink);
		else
			yel_led = 0;
		if (yel_led) IO_vResetPin(IO_P4_7_LED_ST); 
		else IO_vSetPin(IO_P4_7_LED_ST);
//		test_send(U3C0, (time >> 8) & 0xff, time & 0xff);

//		p401_read_anin_manuf_specific[2] = time & 0xFFFF;

		/* give a chance to finish the loop */
		err = endLoop();
    }

    PRINTF("\nSTOP\n");

    /*
     * Application ends really
     *
     * release all resources.
     */
    Stop_CAN();
    DISABLE_CPU_INTERRUPTS();
    releaseTimer();
    ResetIntMask();

    deinit_Library();

    PUTCHAR('X');

    return 0; 
}
/*______________________________________________________________________EOF_*/
