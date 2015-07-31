#define DEF_HW_PART
#include <cal_conf.h>      /* !!! first file to include for all CANopen */
#include <co_init.h>
#include <co_drv.h>
#include "objects.h"            /* object dictionary */
#include <dave\io.h>
#include <dave\rtc.h>
#include "common.h"
#include <ad54x2.h>
#include <sw.h>
#include <examples.h>

UNSIGNED16	bitRate = 125;   /* need for example initCan() */
UNSIGNED8	lNodeId = 32;   /* need for example getNodeId() */
UNSIGNED8	dev_led_st = 0;
UNSIGNED8 	ch1_led_err = 0;
UNSIGNED8 	ch2_led_err = 0;
UNSIGNED8 	ch1_led_st = 0;
UNSIGNED8 	ch2_led_st = 0;

extern void DAVE_vUnlockProtecReg(void);
extern void DAVE_vLockProtecReg(void);

int main(void) {
	BOOL_T		err = CO_FALSE; /* error flag */
	UNSIGNED8   temp = 0;
	uword	time_lo = 0, time_hi = 0;

    /*--- hardware initialization e.g SIO, Chip-Selects, ...----------*/
    iniDevice();
	IO_vInit();
	USIC0_vInit();
	USIC1_vInit();
	USIC2_vInit();
#if HW_KPS == 1
	USIC3_vInit();
#endif
	//RTC init
    DAVE_vUnlockProtecReg();
	RTC_vInit();
    NOP();  /* one cycle delay */
    NOP();  /* one cycle delay */
    DAVE_vLockProtecReg();

	lNodeId = (~IO_uwReadPort(P2))&0xFF;
	if (lNodeId > 127) { lNodeId = 127; }
	temp = (~IO_uwReadPort(P10))&0x0F;
	switch(temp){
		case 0: bitRate = 10; break;
		case 1: bitRate = 20; break; 
		case 2: bitRate = 50; break; 
		case 3: bitRate = 125; break; 
		case 4: bitRate = 250; break; 
		case 5: bitRate = 500; break; 
		case 0x9:
		case 0xE:
		case 0xF:
			bitRate = 125;
			if (((~IO_uwReadPort(P2)) & 0xFF) == 0){ lNodeId = 127;	}
		break;
		default: bitRate = 500; break;
	}

    initCan(bitRate);
    init_Library();
    initTimer();
    Start_CAN();
    ENABLE_CPU_INTERRUPTS();
	RTC_vSetTime(0,0);

	sw_init();

	//PRINTF("loop\n");
    while (err == CO_FALSE) {
		FlushMbox();		         /* Do the CANopen job */
		myRTC_ulGetTime(&time_lo, &time_hi);

		//odczyt temperatury wewn.
		status_wewn[2] = ad7814_read(U2C0, SPI_CS3); 

		sw_loop();

		//sygnalizacja na diodach

		if (blink(time_lo, ch1_led_st)) {	//St1
			IO_vResetPin(IO_P0_0_LED_ST1);
		} else {
			IO_vSetPin(IO_P0_0_LED_ST1);
		}

		if (blink(time_lo, ch2_led_st)) {	//St2
			IO_vResetPin(IO_P4_1_LED_ST2);
		} else {
			IO_vSetPin(IO_P4_1_LED_ST2);
		}

		if (blink(time_lo, ch1_led_err)) {	//Err1
			IO_vResetPin(IO_P4_2_LED_ERR1);
		} else {
			IO_vSetPin(IO_P4_2_LED_ERR1);
		}

		if (blink(time_lo, ch2_led_err)) {	//Err2
			IO_vResetPin(IO_P4_0_LED_ERR2);
		} else {
			IO_vSetPin(IO_P4_0_LED_ERR2);
		}

//w funkcji err_code dopisac cykliczna informacje o kilku bledach (numer bledu jako bity w slowie "dev_led_st" a nie wartosc)
		if (err_code(time_lo, dev_led_st)) {	//err code - Yellow diode
			IO_vResetPin(IO_P2_8_LED_C);
		} else {
			IO_vSetPin(IO_P2_8_LED_C);
		}

		/* give a chance to finish the loop */
		err = endLoop();
    }
    PRINTF("\nSTOP\n");
    Stop_CAN();
    DISABLE_CPU_INTERRUPTS();
    releaseTimer();
    ResetIntMask();
    deinit_Library();
    return 0; 
}
/*______________________________________________________________________EOF_*/
