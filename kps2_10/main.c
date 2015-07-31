#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define DEF_HW_PART
#include <cal_conf.h>      /* !!! first file to include for all CANopen */
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
#include <kps_4_x.h>
#include <iic.h>
#include <dave\rtc.h>

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

UNSIGNED16	bitRate = 125;   /* need for example initCan() */
UNSIGNED8	lNodeId = 32;   /* need for example getNodeId() */
UNSIGNED8	test_blink = 0;
UNSIGNED16	main_type_out = 0;
int		co_debug;

#ifdef CONFIG_RCS_IDENT
static char _rcsid[] = "$Id: main.c,v 1.33 2009/05/12 09:41:40 ro Exp $";
#endif

void safe_state(UNSIGNED8 blink){
	UNSIGNED8 i; 
	for (i=1;i<4;i++)
		p401_write_output_16_bit[i] = 0;
	for (i=1;i<31;i++)
		p401_write_analogue_output_16_bit[i] = 0;
	test_blink = blink; 
}

int main(CO_EXAMPLE_ARGS_DECL)
/* void main(CO_EXAMPLE_ARGS_DECL) */
{
	RET_T         commonRet;      /* return value for CANopen functions */
	UNSIGNED8     ret;            /* return value for common purpose */
	BOOL_T        err = CO_FALSE; /* error flag */
	//UNSIGNED8	  bitRate_temp = 3;
	UNSIGNED8	  temp_uc = 0;
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
//	ulong time = 0;
	uword time_lo = 0;
	uword time_hi = 0;
//	ulong last_temp_read = 0;
	uword last_temp_read_lo = 0;
	uword last_temp_read_hi = 0;
	UNSIGNED8	  yel_led = 0;
	UNSIGNED8	  conf_err = 0;

    /*--- hardware initialization e.g SIO, Chip-Selects, ...----------*/
    ret = iniDevice();
//    PRINTRET("iniDevice: 0x%02x\n", (int)ret);

	IO_vInit();
	USIC0_vInit();
	USIC1_vInit();
	USIC2_vInit();
	USIC3_vInit();

//RTC init
    DAVE_vUnlockProtecReg();
	RTC_vInit();
    NOP();  /* one cycle delay */
    NOP();  /* one cycle delay */
    DAVE_vLockProtecReg();

//IIC enable Power
	IO_vResetPin(IO_P10_5_IIC_ENP);
	IO_vSetPin(IO_P10_4_IIC_EN);
    
	lNodeId = (~IO_uwReadPort(P15))&0x7F;
	switch_ID1 = (~IO_uwReadPort(P15))&0x0F;
	switch_ID2 = (~IO_uwReadPort(P15))&0xF0;
	if ((~IO_uwReadPort(P15))&0x80)
		lNodeId = 127;
    setOptions(CO_EXAMPLE_ARGS);

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
		case 0xE:
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
    commonRet = init_Library();
//    PRINTF("bitRate  %d\n", (int)bitRate);
//    PRINTF("Node ID  %d\n", (int)lNodeId);

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
		else if ((type_U1C0 == 0x40) && diagn_mode)
			main_type_out = 0x0112;
	} else if((type_U0C1 == 0x51)
		&& (type_U1C1 == AD779X_AD7794)
		&& (type_U2C1 == AD779X_AD7795)
		&& (type_U3C0 == AD779X_AD7794)
		&& (type_U1C0 == AD779X_AD7795)){
			main_type_out = 0x0344;
	} else {
		conf_err = 1;
	}

	p401_fault_mode_8[1] = type_U0C1;
	p401_fault_mode_8[2] = type_U3C1;
	p401_fault_mode_8[3] = type_U1C1;
	p401_fault_mode_8[4] = type_U2C1;
	p401_fault_mode_8[5] = type_U3C0;
	p401_fault_mode_8[6] = type_U1C0;
	p401_fault_mode_8[7] = main_type_out >> 8;
	p401_fault_mode_8[8] = main_type_out;

	switch (type_U0C1) {
	case 0x51:
		KPS_5_1_init(U0C1);
		break;
	case AD779X_AD7794:
		KPS_5_2_init(U0C1);
		break;
	default:
		break;
	}

	switch (type_U3C1) {
	case 0x41:
		KPS_4_1_init(U3C1);
		break;
	case 0x47:
		KPS_4_1_init_old(U3C1);
		break;
	case 0x42:
		KPS_4_2_init(U3C1);
		break;
	case AD779X_AD7795:
		KPS_5_2_init(U3C1);
		break;
	default:
		break;
	}

	switch (type_U1C1) {
	case 0x51:
		KPS_5_1_init(U1C1);
		break;
	case AD779X_AD7794:
		KPS_5_2_init(U1C1);
		break;
	default:
		break;
	}

	switch (type_U2C1) {
	case 0x41:
		KPS_4_1_init(U2C1);
		break;
	case 0x47:
		KPS_4_1_init_old(U2C1);
		break;
	case 0x42:
		KPS_4_2_init(U2C1);
		break;
	case AD779X_AD7795:
		KPS_5_2_init(U2C1);
		break;
	default:
		break;
	}

	switch (type_U3C0) {
	case 0x51:
		KPS_5_1_init(U3C0);
		break;
	case AD779X_AD7794:
		KPS_5_2_init(U3C0);
		break;
	default:
		break;
	}

	switch (type_U1C0) {
	case 0x41:
		KPS_4_1_init(U1C0);
		break;
	case 0x47:
		KPS_4_1_init_old(U1C0);
		break;
	case 0x42:
		KPS_4_2_init(U1C0);
		break;
	case AD779X_AD7795:
		KPS_5_2_init(U1C0);
		break;
	default:
		break;
	}

	RTC_vSetTime(0,0);
//    PRINTF("loop\n");
    while (err == CO_FALSE) {
		myRTC_elapsed2(1);
//		time = RTC_ulGetTime();
		myRTC_ulGetTime(&time_lo, &time_hi);
		FlushMbox();		         /* Do the CANopen job */
//p401_fault_mode_16[1] = RTC_ulGetTime() - time;
p401_fault_mode_16[2] =	myRTC_sub(time_lo, time_hi) / RTC_MUL;
		iic_led_loop();
		switch_ID1 = (~IO_uwReadPort(P15))&0x0F;
		switch_ID2 = ((~IO_uwReadPort(P15))&0xF0) >> 4;
		if (diagn_mode){
			safe_state(test_blink);
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
			iic_led_setd(0, 0x51, p401_read_input_16_bit[1]);
//??			KPS_5_2_namur(U0C1, p401_read_state_bl1_1, 13);
		}
//p401_fault_mode_16[3] = RTC_ulGetTime() - time;
p401_fault_mode_16[3] = myRTC_sub(time_lo, time_hi) / RTC_MUL;
		if (type_U1C1 == AD779X_AD7794){
			KPS_5_2_pt100_loop(U1C1, p401_read_analogue_input_16_bit, 11, 2, p401_fault_mode_8[9]);
			iic_led_seta(1, 0x520, p401_read_analogue_input_16_bit, 11, 2);
			KPS_5_2_namur(U1C1, p401_read_state_bl1_1, 5);
			iic_led_setd(1, 0x52D, p401_read_input_16_bit[10]);
		}
p401_fault_mode_16[4] = myRTC_sub(time_lo, time_hi) / RTC_MUL;
//p401_fault_mode_16[4] = RTC_ulGetTime() - time;
		if (type_U2C1 == AD779X_AD7795){
			KPS_5_2_mA_loop(U2C1, p401_read_analogue_input_16_bit, 15, 6);
			iic_led_seta(1, 0x52A, p401_read_analogue_input_16_bit, 15, 6);
			KPS_5_2_namur(U2C1, p401_read_state_bl1_1, 7);
		}
p401_fault_mode_16[5] = myRTC_sub(time_lo, time_hi) / RTC_MUL;
//p401_fault_mode_16[5] = RTC_ulGetTime() - time;
		if (type_U3C0 == AD779X_AD7794){
			KPS_5_2_pt100_loop(U3C0, p401_read_analogue_input_16_bit, 21, 2, p401_fault_mode_8[9]);
			iic_led_seta(2, 0x520, p401_read_analogue_input_16_bit, 21, 2);
			KPS_5_2_namur(U3C0, p401_read_state_bl1_1, 9);
			iic_led_setd(2, 0x52D, p401_read_input_16_bit[10]);
		}
p401_fault_mode_16[6] = myRTC_sub(time_lo, time_hi) / RTC_MUL;
//p401_fault_mode_16[6] = RTC_ulGetTime() - time;
		if (type_U1C0 == AD779X_AD7795){
			KPS_5_2_mA_loop(U1C0, p401_read_analogue_input_16_bit, 25, 6);
			iic_led_seta(2, 0x52A, p401_read_analogue_input_16_bit, 25, 6);
			KPS_5_2_namur(U1C0, p401_read_state_bl1_1, 11);
		}
p401_fault_mode_16[7] = myRTC_sub(time_lo, time_hi) / RTC_MUL;
//p401_fault_mode_16[7] = RTC_ulGetTime() - time;
		if (type_U3C1 == 0x41){
			KPS_4_1_loop(U3C1, p401_write_output_16_bit, 1, p401_read_input_16_bit, 4);
			iic_led_setd(0, 0x41, p401_write_output_16_bit[1]);
		}
		if (type_U2C1 == 0x41){
			KPS_4_1_loop(U2C1, p401_write_output_16_bit, 2, p401_read_input_16_bit, 5);
			iic_led_setd(1, 0x41, p401_write_output_16_bit[2]);
		}
		iic_led_loop();
p401_fault_mode_16[8] = myRTC_sub(time_lo, time_hi) / RTC_MUL;
//p401_fault_mode_16[8] = RTC_ulGetTime() - time;
		if (type_U1C0 == 0x42){
			KPS_4_2_loop(U1C0, p401_write_analogue_output_16_bit, 21);
			iic_led_seta(2, 0x42, p401_write_analogue_output_16_bit, 21, 10);
		}
p401_fault_mode_16[9] = myRTC_sub(time_lo, time_hi) / RTC_MUL;
//p401_fault_mode_16[9] = RTC_ulGetTime() - time;
		p401_read_input_16_bit[10] = 0;
		for (i=0;i<12;i++)
			if (p401_read_state_bl1_1[i+1])
				p401_read_input_16_bit[10] |= (1 << i);
p401_fault_mode_16[10] = myRTC_sub(time_lo, time_hi) / RTC_MUL;
//p401_fault_mode_16[10] = RTC_ulGetTime() - time;
			
		
//dla sprawdzenia karty bez nakladki
		if ((type_U1C0 == 0x40) && diagn_mode){				
			if (switch_ID1) {
				p401_write_analogue_output_16_bit[5+switch_ID1] = p401_write_analogue_output_16_bit[0+switch_ID1];
				p401_write_analogue_output_16_bit[15+switch_ID1] = p401_write_analogue_output_16_bit[10+switch_ID1];
				p401_write_analogue_output_16_bit[25+switch_ID1] = p401_write_analogue_output_16_bit[20+switch_ID1];
			}
			KPS_4_2_loop(U1C0, p401_write_analogue_output_16_bit, 21);
		}
p401_fault_mode_16[11] = myRTC_sub(time_lo, time_hi) / RTC_MUL;
//p401_fault_mode_16[11] = RTC_ulGetTime() - time;
//		if (RTC_ulGetTime() - last_temp_read > 10000){
//		if (myRTC_sub(last_temp_read_lo, last_temp_read_hi) > 10000 * RTC_MUL){	//sprawdzenie temperatury co 10 sekund
		if (last_temp_read_hi != time_hi) {
			p401_read_anin_manuf_specific[1] = read_temp_iic();
			last_temp_read_hi = time_hi;
//			last_temp_read = RTC_ulGetTime();
//			myRTC_ulGetTime(&last_temp_read_lo, &last_temp_read_hi);
		}

		iic_led_loop();
		if (diagn_mode)
//			yel_led = blink(time_lo, 1);
			yel_led = blink(time_lo, 5);
		else if (conf_err)
			yel_led = blink(time_lo, 1);
		else if (test_blink > 0)
			yel_led = blink(time_lo, test_blink);
		else
			yel_led = 0;
		if (yel_led) IO_vResetPin(IO_P4_7_LED_ST); 
		else IO_vSetPin(IO_P4_7_LED_ST);
		/* give a chance to finish the loop */
		err = endLoop();
//test_send(U3C0, (time >> 8) & 0xff, time & 0xff);

//p401_read_anin_manuf_specific[2] = time & 0xFFFF;

//p401_fault_mode_16[1] = RTC_ulGetTime() - time;
//p401_fault_mode_16[2] =	myRTC_sub(time_lo, time_hi);
p401_fault_mode_16[12] = myRTC_elapsed2(0) / RTC_MUL;
p401_fault_mode_16[13] = time_lo;
p401_fault_mode_16[14] = time_hi;
p401_fault_mode_16[15] = myRTC_sub(time_lo, time_hi) / RTC_MUL;
//p401_fault_mode_16[15] = RTC_ulGetTime() - time;
//p401_fault_mode_16[16] = time & 0xFFFF;
    }

    PRINTF("\nSTOP\n");
    /* Application ends really - release all resources. */
    Stop_CAN();
    DISABLE_CPU_INTERRUPTS();
    releaseTimer();
    ResetIntMask();

    deinit_Library();
    return 0; 
}
/*______________________________________________________________________EOF_*/
