#include <sw.h>
#if HW_UZS == 1
	#include "objects.h"            /* object dictionary */
	#include "eeprom.h"
	#include "arka.h"
	#include "par.h"
	#include <rs.h>

	extern UNSIGNED8 dev_led_st;
	extern UNSIGNED8 ch1_led_err;
	extern UNSIGNED8 ch2_led_err;
	extern UNSIGNED8 ch1_led_st;
	extern UNSIGNED8 ch2_led_st;

	unsigned char buff[BUFF_SIZE];
	unsigned char buff_tx[BUFF_SIZE];
	unsigned char buff_rx_u1c1[BUFF_SIZE];
	unsigned char buff_rx_u2c1[BUFF_SIZE];
	UNSIGNED16	str_u1c1_temp[BUFF_SIZE];
	UNSIGNED16	str_u2c1_temp[BUFF_SIZE];

	uword	time_lo_ch1 = 0, time_hi_ch1 = 0;
	uword	time_lo_ch2 = 0, time_hi_ch2 = 0;

	UNSIGNED16	time_lo_ips = 0, time_hi_ips = 0;
	UNSIGNED16	time_lo_ips2 = 0, time_hi_ips2 = 0;
	UNSIGNED8	ips_roll = 0;
	unsigned char in_iter_u1c1 = 0;
	unsigned char in_iter_u2c1 = 0;

	void safe_state(void){
	}

	void sw_init(void){
		unsigned char temp = 0;
		temp = odczyt_mapowania ();
		if (temp == COMPLETE) {	//	PRINTF("\nRead success\n\r");
		} else {
			dev_led_st = 2;
		}
	}

	void sw_loop(void){
		unsigned char i = 0;
		unsigned char temp = 0, temp2 = 0;
		UNSIGNED16 temp_u16 = 0;
	//odczyt stanu DIP switch
		status_wewn[1] = (((~IO_uwReadPort(P10))&0xC0) >> 2) | (((~IO_uwReadPort(P10))&0x30) >> 4);

	//odbieranie danych z kanalu CH1
		while (RS_FILLING_RX(U1C1) > 0) {
			temp = RS_GET_RX(U1C1);
			temp = get_serial(temp, &in_iter_u1c1, buff_rx_u1c1);
		    if (temp > 0) {
				temp2 = prepare_serial(buff_tx, buff_rx_u1c1, temp);
				byte_arr_2_word_arr(str_u2c1_temp, buff_tx, temp2);
				RS_FILL_TX(U2C1, str_u2c1_temp, temp2);
			}
		}

	//odbieranie danych z kanalu CH2
		while (RS_FILLING_RX(U2C1) > 0) {
			temp = RS_GET_RX(U2C1);
			temp = get_serial(temp, &in_iter_u2c1, buff_rx_u2c1);
		    if (temp > 0) {
				if (buff_rx_u2c1[0] == C_IPS) {
					//przemapowanie i wyslanie do usb (U1C1)
	/*				for (i=1; i<key_map_usb1[0]+1; i+=1) {
						temp = key_map_usb1[i];
						temp2 = key_map_usb1[i]/8;
						if ((temp < 64) //numer przycisku w zakresie (0xff-niezdefiniowany)
						&& (buff_rx_u2c1[temp2+1] & (1 << (temp % 8))) //przycisk obecnie wcisniety
						&& !(p401_read_input_8_bit[temp2+2] & (1 << (temp % 8))) //przycisk wczesniej nie byl wcisniety
						&& (key_map_usb2[i] != 0)){	//zdefiniowane mapowanie
							buff[0] = C_IPS_USB;
							buff[1] = C_IPS_MAP;
							buff[2] = key_map_usb2[i];
							temp2 = prepare_serial(buff_tx, buff, 3);
							byte_arr_2_word_arr(str_u1c1_temp, buff_tx, temp2);
							RS_FILL_TX(U1C1, str_u1c1_temp, temp2);							
						}
					}
	*/				
					
					temp = 0;
					for (i=0; i<8; i+=1){ 
						//wykrycie zmiany stanu
						if (p401_read_input_8_bit[i+2] != buff_rx_u2c1[i+1]) { temp = 1; }
						p401_read_input_8_bit[i+2] = buff_rx_u2c1[i+1];
						//stan przycisku + stan diody
						buff[i+3] = p401_write_state_8[i+2] | p401_read_input_8_bit[i+2];
					}
					buff[0] = C_IPS_USB;
					buff[1] = C_IPS_MAP;
					buff[2] = config_upt;
					temp2 = prepare_serial(buff_tx, buff, 11);
					byte_arr_2_word_arr(str_u1c1_temp, buff_tx, temp2);
					RS_FILL_TX(U1C1, str_u1c1_temp, temp2);							

					if ((status_wewn[1] & 0x20) == 0x20 ) {
						ch1_led_st = 0;
						if (temp) {	ch1_led_st = 0xff;	}
	//						myRTC_ulGetTime(&time_lo_ch1, &time_hi_ch1);		 //pobranie czasu do zmiennych
					}
					if ((status_wewn[1] & 0x02) == 0x02 ) {
						ch2_led_st = 0;
						if (temp) {	ch2_led_st = 0xff;	}
	//						myRTC_ulGetTime(&time_lo_ch2, &time_hi_ch2);		 //pobranie czasu do zmiennych
					}
				} else if (buff_rx_u2c1[0] == C_STATUS) {
					myRTC_ulGetTime(&time_lo_ips2, &time_hi_ips2);		 //pobranie czasu do zmiennych
	//2 mlodsze bity jako konfiguracja, 2 starsze jako status transmisji
					if ((buff_rx_u2c1[1] & 0x33) != (status_wewn[1] & 0x33)){
						ch1_led_err = 0xff;
						ch2_led_err = 0xff;
					} else {
						ch1_led_err = 0;
						ch2_led_err = 0;
						if ((status_wewn[1] & 0x20) != 0x20 ) {
							ch1_led_st = 0;
							if (buff_rx_u2c1[1] & 0x40) ch1_led_st = 0xff;
						}
						if ((status_wewn[1] & 0x02) != 0x02 ) {
							ch2_led_st = 0;
							if (buff_rx_u2c1[1] & 0x04) ch2_led_st = 0xff;
						}
						if (buff_rx_u2c1[1] & 0x80) ch1_led_err = 0xff;
						if (buff_rx_u2c1[1] & 0x08) ch2_led_err = 0xff;
					}
				} else {
					//przeslanie do USB
	//				myRTC_ulGetTime(&time_lo_ch1, &time_hi_ch1);		 //pobranie czasu do zmiennych
					temp2 = prepare_serial(buff_tx, buff_rx_u2c1, temp);
					byte_arr_2_word_arr(str_u1c1_temp, buff_tx, temp2);
					RS_FILL_TX(U1C1, str_u1c1_temp, temp2);
				}
			}
		}

	//brak odczytu statusu przez 1s
		temp_u16 = myRTC_sub(time_lo_ips2, time_hi_ips2) / RTC_MUL; //roznica czasu w zmiennych do chwili obecnej
		if (temp_u16 > 1000 || (time_lo_ips == 0 && time_hi_ips == 0)) {	// 100ms
			ch1_led_st = 0;
			ch2_led_st = 0;
			ch1_led_err = 0xff;
			ch2_led_err = 0xff;
		}


		temp_u16 = myRTC_sub(time_lo_ips, time_hi_ips) / RTC_MUL; //roznica czasu w zmiennych do chwili obecnej
		if (temp_u16 > 100 || (time_lo_ips == 0 && time_hi_ips == 0)) {	// 100ms
			if ((ips_roll < 4) 
			&& (((status_wewn[1] & 0x20) == 0x20 ) 			//IPS na kanale CH1
				|| ((status_wewn[1] & 0x02) == 0x02 ))) {	//IPS na kanale CH2
	//wysylanie stanu diod
				buff[0] = C_DIODE;
				for (i=0; i<8; i+=1){ buff[i+1] = p401_write_state_8[i+2];}
				temp2 = prepare_serial(buff_tx, buff, 9);
				byte_arr_2_word_arr(str_u2c1_temp, buff_tx, temp2);
				RS_FILL_TX(U2C1, str_u2c1_temp, temp2);
				ips_roll += 1;
			} else {
	//wyslanie konfiguracji
				buff[0] = C_CONTROL;
				buff[1] = status_wewn[1];
				temp2 = prepare_serial(buff_tx, buff, 2);
				byte_arr_2_word_arr(str_u2c1_temp, buff_tx, temp2);
				RS_FILL_TX(U2C1, str_u2c1_temp, temp2);
				ips_roll = 0;
			}
			myRTC_ulGetTime(&time_lo_ips, &time_hi_ips);		 //pobranie czasu do zmiennych
		}

		if (zapis == 1) {
			temp = zapis_mapowania ();
			if (temp == COMPLETE) {
				zapis = 0;
			} else {
				zapis = (temp << 4) + 1;
				dev_led_st = 3;
			}
		}
	}

#endif