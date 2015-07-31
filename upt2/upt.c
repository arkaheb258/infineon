#include <sw.h>
#if HW_UPT == 1
	#include "objects.h"            /* object dictionary */
	#include "spi.h"
	#include <ad54x2.h>
	#include "common.h"
	#include "eeprom.h"
	#include "arka.h"
	#include "pt100.h"
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

	unsigned char in_iter_u1c1 = 0;
	unsigned char in_iter_u2c1 = 0;

	void safe_state(void){
	}

	void sw_init(void){
		unsigned char temp = 0;
	//inicjalizacja analog out
		spi_writePin(U2C0, SPI_CS1, 1);
		spi_writePin(U2C0, SPI_CS2, 1);
		ad54x2_resetAll(U2C0, SPI_CS1);
		ad54x2_resetAll(U2C0, SPI_CS2);

		dev_led_st = 0;
		temp = EEPROM_Init ();
		if (temp == COMPLETE) {	//	 PRINTF("\nEEPROM Initialization Success\n\r");
		} else { zapis = temp; }

		temp = odczyt_parametrow ();
		if (temp == COMPLETE) {	//	PRINTF("\nRead success\n\r");
		} else {
			dev_led_st = 2;
			domyslne_parametry(); 
		}
	}

	void sw_loop(void){
		unsigned char temp = 0, temp2 = 0;
		UNSIGNED16	temp_u16 = 0;
		INTEGER16	temp_s16 = 0;
		float		temp_f = 0;
	//odczyt stanu DIP switch
		status_wewn[1] = ((~IO_uwReadPort(P10))&0xFF) << 8 | ((~IO_uwReadPort(P2))&0xFF);

		p401_read_input_8_bit[1] = status_upt;
		p401_read_analogue_input_16_bit[1] = rezystancja_ch1/10;
		p401_read_analogue_input_16_bit[2] = rezystancja_ch2/10;
		p401_read_analogue_input_16_bit[3] = temperatura_ch1;
		p401_read_analogue_input_16_bit[4] = temperatura_ch2;

		if (zapis == 1) {
			temp = zapis_parametrow ();
			if (temp == COMPLETE) {
				zapis = 0;
			} else {
				zapis = (temp << 4) + 1;
				dev_led_st = 3;
			}
		} else if (zapis == 0xFF) {
			domyslne_parametry();
			zapis = 0;
		}

	//odbieranie danych z kanalu CH1
		while (RS_FILLING_RX(U1C1) > 0) {
			temp = RS_GET_RX(U1C1);
			temp = get_serial(temp, &in_iter_u1c1, buff_rx_u1c1);
		    if (temp > 0) {
				if (buff_rx_u1c1[0] == C_UPT) {
					rezystancja_ch1 = (UNSIGNED32)string2int(buff_rx_u1c1, temp);
					rezystancja_ch1 = sma_16(0, (UNSIGNED16)rezystancja_ch1, status_wewn[3]);
					myRTC_ulGetTime(&time_lo_ch1, &time_hi_ch1);		 //pobranie czasu do zmiennych
				} else {
					temp2 = prepare_serial(buff_tx, buff_rx_u1c1, temp);
					byte_arr_2_word_arr(str_u2c1_temp, buff_tx, temp2);
					RS_FILL_TX(U2C1, (uword *)str_u2c1_temp, temp2);
				}
			}
		}

	//odbieranie danych z kanalu CH2
		while (RS_FILLING_RX(U2C1) > 0) {
			temp = RS_GET_RX(U2C1);
			temp = get_serial(temp, &in_iter_u2c1, buff_rx_u2c1);
		    if (temp > 0) {
				if (buff_rx_u2c1[0] == C_UPT) {
					rezystancja_ch2 = (UNSIGNED32)string2int(buff_rx_u2c1, temp);
					rezystancja_ch2 = sma_16(1, (UNSIGNED16)rezystancja_ch2, status_wewn[4]);
					myRTC_ulGetTime(&time_lo_ch2, &time_hi_ch2);		 //pobranie czasu do zmiennych
				} else {
					temp2 = prepare_serial(buff_tx, buff_rx_u2c1, temp);
					byte_arr_2_word_arr(str_u1c1_temp, buff_tx, temp2);
					RS_FILL_TX(U1C1, (uword *)str_u1c1_temp, temp2);
				}
			}
		}

		if (config_upt & 0x80) {	//IO1
		}
		if (config_upt & 0x40) {	//IO2
		}

		if (config_upt & 0x08) {	//Pt1
			temperatura_ch1 = GetPt100Temperature(rezystancja_ch1 * 10) / 10;
		} else {
			temperatura_ch1 = PT100MIN_T / 10;
		}

		if (config_upt & 0x04) {	//Pt2
			temperatura_ch2 = GetPt100Temperature(rezystancja_ch2 * 10) / 10;
		} else {
			temperatura_ch2 = PT100MIN_T / 10;
		}

	//wyliczenie stanu wyjsc przekaznikowych na podstawie podanych zakresow
		if (config_upt & 0x20) {	//PK1
			if (config_upt & 0x08) {	//Pt1
				if ((temperatura_ch1 >= zakresy_przekaznika_ch1[6]*10) && (temperatura_ch1 <= zakresy_przekaznika_ch1[7]*10))
					status_upt |= 0x20;
				if ((temperatura_ch1 < zakresy_przekaznika_ch1[5]*10) || (temperatura_ch1 >= zakresy_przekaznika_ch1[8]*10))
					status_upt &= 0xDF;
			} else {
				if ((rezystancja_ch1 >= zakresy_przekaznika_ch1[2]*10) && (rezystancja_ch1 <= zakresy_przekaznika_ch1[3]*10))
					status_upt |= 0x20;
				if ((rezystancja_ch1 < zakresy_przekaznika_ch1[1]*10) || (rezystancja_ch1 >= zakresy_przekaznika_ch1[4]*10))
					status_upt &= 0xDF;
			}
		} else {
			status_upt &= 0xDF;
		}

		if (config_upt & 0x10) {	//PK2
			if (config_upt & 0x04) {	//Pt2
				if ((temperatura_ch2 >= zakresy_przekaznika_ch2[6]*10) && (temperatura_ch2 <= zakresy_przekaznika_ch2[7]*10))
					status_upt |= 0x10;
				if ((temperatura_ch2 < zakresy_przekaznika_ch2[5]*10) || (temperatura_ch2 >= zakresy_przekaznika_ch2[8]*10))
					status_upt &= 0xEF;
			} else {
				if ((rezystancja_ch2 > zakresy_przekaznika_ch2[2]*10) && (rezystancja_ch2 < zakresy_przekaznika_ch2[3]*10))
					status_upt |= 0x10;
				if ((rezystancja_ch2 < zakresy_przekaznika_ch2[1]*10) || (rezystancja_ch2 > zakresy_przekaznika_ch2[4]*10))
					status_upt &= 0xEF;
			}
		} else {
			status_upt &= 0xEF;
		}

	//konfiguracja wyjsc analogowych
		//	U - 0x11B1	(0-10V)
		//	I - 0x11B6	(0-20mA)
		//	I - 0x11B7	(0-24mA)
	//	if (config_upt & 0x02) {	//AO1
			if (!IO_ubReadPin(IO_P10_7_S51) && IO_ubReadPin(IO_P10_6_S52)){
				ad54x2_setConfigOne(U2C0, SPI_CS2, AD54X2_REG_CONTROL, 0x11B1, 0);
			} else if (!IO_ubReadPin(IO_P10_6_S52) && IO_ubReadPin(IO_P10_7_S51)){
				ad54x2_setConfigOne(U2C0, SPI_CS2, AD54X2_REG_CONTROL, 0x11B7, 0);
			} else {
				ad54x2_setConfigOne(U2C0, SPI_CS2, AD54X2_REG_CONTROL, 0x0000, 0);
			}
	//	} else {
	//		ad54x2_setConfigOne(U2C0, SPI_CS2, AD54X2_REG_CONTROL, 0x0000, 0);
	//	}

	//	if (config_upt & 0x01) {	//AO2
			if (!IO_ubReadPin(IO_P10_5_S53) && IO_ubReadPin(IO_P10_4_S54)){
				ad54x2_setConfigOne(U2C0, SPI_CS1, AD54X2_REG_CONTROL, 0x11B1, 0);
			} else if (!IO_ubReadPin(IO_P10_4_S54) && IO_ubReadPin(IO_P10_5_S53)){
				ad54x2_setConfigOne(U2C0, SPI_CS1, AD54X2_REG_CONTROL, 0x11B7, 0);
			} else {
				ad54x2_setConfigOne(U2C0, SPI_CS1, AD54X2_REG_CONTROL, 0x0000, 0);
			}
	//	} else {
	//		ad54x2_setConfigOne(U2C0, SPI_CS1, AD54X2_REG_CONTROL, 0x0000, 0);
	//	}

	//Periodical read-back of configuration registers
		status_ad5412_ch1[1] = ad54x2_readRegOne(U2C0, SPI_CS2, AD54X2_REG_R_CTRL, 0);
		status_ad5412_ch1[2] = ad54x2_readRegOne(U2C0, SPI_CS2, AD54X2_REG_R_STAT, 0);
		status_ad5412_ch1[3] = ad54x2_readRegOne(U2C0, SPI_CS2, AD54X2_REG_R_DATA, 0);

		status_ad5412_ch2[1] = ad54x2_readRegOne(U2C0, SPI_CS1, AD54X2_REG_R_CTRL, 0);
		status_ad5412_ch2[2] = ad54x2_readRegOne(U2C0, SPI_CS1, AD54X2_REG_R_STAT, 0);
		status_ad5412_ch2[3] = ad54x2_readRegOne(U2C0, SPI_CS1, AD54X2_REG_R_DATA, 0);

//		status_upt &= ~(0x02);
		status_upt &= 0xFD;
		if (!IO_ubReadPin(IO_P10_7_S51) && IO_ubReadPin(IO_P10_6_S52)){
			if (status_ad5412_ch1[1] != 0x11B1) dev_led_st = 4;
			else status_upt |= 0x02;
		} else if (!IO_ubReadPin(IO_P10_6_S52) && IO_ubReadPin(IO_P10_7_S51)){
			if (status_ad5412_ch1[1] != 0x11B7) dev_led_st = 4;
			else status_upt |= 0x02;
		} else {
			if (status_ad5412_ch1[1] != 0x0000) dev_led_st = 4;
		}

//		status_upt &= ~(0x01);
		status_upt &= 0xFE;
		if (!IO_ubReadPin(IO_P10_5_S53) && IO_ubReadPin(IO_P10_4_S54)){
			if (status_ad5412_ch2[1] != 0x11B1) dev_led_st = 6;
			else status_upt |= 0x01;
		} else if (!IO_ubReadPin(IO_P10_4_S54) && IO_ubReadPin(IO_P10_5_S53)){
			if (status_ad5412_ch2[1] != 0x11B7) dev_led_st = 6;
			else status_upt |= 0x01;
		} else {
			if (status_ad5412_ch2[1] != 0x0000) dev_led_st = 6;
		}

	//wykrywanie awarii komunikacji z CH1
		temp_u16 = myRTC_sub(time_lo_ch1, time_hi_ch1) / RTC_MUL; //roznica czasu w zmiennych do chwili obecnej
	//		status_wewn[4] = temp_u16;
		if (temp_u16 > 1000 || (time_lo_ch1 == 0 && time_hi_ch1 == 0)) {	// 1s
			status_upt |= 0x80;
			status_upt &= 0xDF;	// wylaczenie przekaznika PK1
			rezystancja_ch1 = 0;
		} else {
			status_upt &= 0x7F;
		}

	//wykrywanie awarii komunikacji z CH2
		temp_u16 = myRTC_sub(time_lo_ch2, time_hi_ch2) / RTC_MUL; //roznica czasu w zmiennych do chwili obecnej
		if (temp_u16 > 1000 || (time_lo_ch2 == 0 && time_hi_ch2 == 0)) {	// 1s
			status_upt |= 0x40;
			status_upt &= 0xEF;	// wylaczenie przekaznika PK2
			rezystancja_ch2 = 0;
		} else {
			status_upt &= 0xBF;
		}

	//przepisanie typu czujnikow z config_upt do status_upt (bity 3 i 2)
		status_upt = (status_upt & 0xF3) | (config_upt & 0x0C);

	// wysterowanie przekaznikow
		if (status_upt & 0x20) {	//PK1
			IO_vSetPin(IO_P7_1_K1_EN);
			ch1_led_st = 0xff;
		} else {
			IO_vResetPin(IO_P7_1_K1_EN);
			ch1_led_st = 0;
		}

		if (status_upt & 0x10) {	//PK2
			IO_vSetPin(IO_P7_2_K2_EN);
			ch2_led_st = 0xff;
		} else {
			IO_vResetPin(IO_P7_2_K2_EN);
			ch2_led_st = 0;
		}

	//wysterowanie wyjsc analogowych
		if (status_upt & 0x02) {	//AO1
			status_ad5412_ch1[4] = 0x0000; 
			temp_f = 0;
			if (config_upt & 0x08) {	//Pt1
				if (temperatura_ch1 < zakresy_analog_out_ch1[3]*10) {
					status_ad5412_ch1[4] = 0x0000; 
				} else if (temperatura_ch1 > zakresy_analog_out_ch1[4]*10) {
					status_ad5412_ch1[4] = 0xffff; 
				} else { 
					temp_f = (temperatura_ch1 - zakresy_analog_out_ch1[3]*10);
					if (zakresy_analog_out_ch1[4] <= zakresy_analog_out_ch1[3]) {
						status_ad5412_ch1[4] = 0;
					} else {
						temp_f /= (zakresy_analog_out_ch1[4] - zakresy_analog_out_ch1[3])*10;
						if (!IO_ubReadPin(IO_P10_7_S51)){
							temp_f *= 0xCCCC;
							status_ad5412_ch1[4] = 0x1999;
						} else { //zgodnie z Namur NE43
							temp_f *= 0xAAAA;
							status_ad5412_ch1[4] = 0x2AAA;
						}
						status_ad5412_ch1[4] += (UNSIGNED16)temp_f;
					}
				}
			} else {
				temp_s16 = rezystancja_ch1 / 10;
				if (temp_s16 < zakresy_analog_out_ch1[1]) {
					status_ad5412_ch1[4] = 0x0000; 
				} else if (temp_s16 > zakresy_analog_out_ch1[2]) {
					status_ad5412_ch1[4] = 0xffff; 
				} else { 
					temp_f = (temp_s16 - zakresy_analog_out_ch1[1]);
					if (zakresy_analog_out_ch1[2] <= zakresy_analog_out_ch1[1]) {
						status_ad5412_ch1[4] = 0;
					} else {
						temp_f /= (zakresy_analog_out_ch1[2] - zakresy_analog_out_ch1[1]);
						if (!IO_ubReadPin(IO_P10_7_S51)){
							temp_f *= 0xCCCC;
							status_ad5412_ch1[4] = 0x1999;
						} else { //zgodnie z Namur NE43
							temp_f *= 0xAAAA;
							status_ad5412_ch1[4] = 0x2AAA;
						}
						status_ad5412_ch1[4] += (UNSIGNED16)temp_f;
					}
				}
			}
			// wyzerowanie wyjscia AO1 przy bledzie odczytu CH1
			if (status_upt & 0x80) { status_ad5412_ch1[4] = 0x0000; } 
			ad54x2_writeDataAll(U2C0, SPI_CS2, AD54X2_REG_DATA, status_ad5412_ch1 + 4);
		} else {
			//reset
			ad54x2_resetAll(U2C0, SPI_CS2);
		}

		if (status_upt & 0x01) {	//AO2
			status_ad5412_ch2[4] = 0x0000; 
			if (config_upt & 0x04) {	//Pt2
				if (temperatura_ch2 < zakresy_analog_out_ch2[3]*10) {
					status_ad5412_ch2[4] = 0x0000; 
				} else if (temperatura_ch2 > zakresy_analog_out_ch2[4]*10) {
					status_ad5412_ch2[4] = 0xffff; 
				} else { 
					if (zakresy_analog_out_ch2[4] <= zakresy_analog_out_ch2[3]) {
						status_ad5412_ch2[4] = 0;
					} else {
						temp_f = (temperatura_ch2 - zakresy_analog_out_ch2[3]*10);
						temp_f /= (zakresy_analog_out_ch2[4] - zakresy_analog_out_ch2[3])*10;
						if (!IO_ubReadPin(IO_P10_5_S53)){
							temp_f *= 0xCCCC;
							status_ad5412_ch2[4] = 0x1999;
						} else { //zgodnie z Namur NE43
							temp_f *= 0xAAAA;
							status_ad5412_ch2[4] = 0x2AAA;
						}
						status_ad5412_ch2[4] += (UNSIGNED16)temp_f;
					}
				}
			} else {
				temp_s16 = rezystancja_ch2 / 10;
				if (temp_s16 < zakresy_analog_out_ch2[1]) {
					status_ad5412_ch2[4] = 0x0000; 
				} else if (temp_s16 > zakresy_analog_out_ch2[2]) {
					status_ad5412_ch2[4] = 0xffff; 
				} else { 
					temp_f = (temp_s16 - zakresy_analog_out_ch2[1]);
					if (zakresy_analog_out_ch2[2] <= zakresy_analog_out_ch2[1]) {
						status_ad5412_ch2[4] = 0;
					} else {
						temp_f /= (zakresy_analog_out_ch2[2] - zakresy_analog_out_ch2[1]);
						if (!IO_ubReadPin(IO_P10_5_S53)){
							temp_f *= 0xCCCC;
							status_ad5412_ch2[4] = 0x1999;
						} else { //zgodnie z Namur NE43
							temp_f *= 0xAAAA;
							status_ad5412_ch2[4] = 0x2AAA;
						}
						status_ad5412_ch2[4] += (UNSIGNED16)temp_f;
					}
				}
			}
			// wyzerowanie wyjscia AO2 przy bledzie odczytu CH2
			if (status_upt & 0x40) { status_ad5412_ch2[4] = 0x0000; } 
			ad54x2_writeDataAll(U2C0, SPI_CS1, AD54X2_REG_DATA, status_ad5412_ch2 + 4);
		} else {
			//reset 
			ad54x2_resetAll(U2C0, SPI_CS1);
		}
		
	//obsluga sygnalizacji LED
		if (status_upt & 0x80) {	//Err1
			ch1_led_err = 0xff;
		} else {
			if (rezystancja_ch1 < 100) {			//zwarcie
				ch1_led_err = 5;			//dioda miga z czestotliwoscia 5Hz
			} else if (rezystancja_ch1 > 50000) {	//przerwa
				ch1_led_err = 1;			//dioda miga z czestotliwoscia 1Hz
			} else {
				ch1_led_err = 0;
			}
		}

		if (status_upt & 0x40) {	//Err2
			ch2_led_err = 0xff;
		} else {
			if (rezystancja_ch2 < 100) {			//zwarcie
				ch2_led_err = 5;			//dioda miga z czestotliwoscia 5Hz
			} else if (rezystancja_ch2 > 50000) {	//przerwa
				ch2_led_err = 1;			//dioda miga z czestotliwoscia 1Hz
			} else {
				ch2_led_err = 0;
			}
		}
	}
#endif