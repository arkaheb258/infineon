# include "ad779x.h"


void ad779x_reset(UNSIGNED8 port);
void ad779x_setFilter(UNSIGNED8 port, UNSIGNED8 filter);
void ad779x_setChannel(UNSIGNED8 port, UNSIGNED8 gain, UNSIGNED8 ch);
void ad779x_setCurrent(UNSIGNED8 port, UNSIGNED8 prad);

#define SMA_CH_COUNT	30
#define SMA_MAX			64
UNSIGNED32 ulSMA[SMA_CH_COUNT][SMA_MAX];
UNSIGNED8 ulSMA_iter[SMA_CH_COUNT];

UNSIGNED16 ad779x_tx_ptr[13];
UNSIGNED16 ad779x_rx_ptr[13];

UNSIGNED8 ad7794_curr[8*AD7794_CHANELLS];
//UNSIGNED8 ad7795_curr[AD7795_CHANELLS];
//UNSIGNED8 ad7794_ch = 0;
//UNSIGNED8 ad7795_ch = 0;
UNSIGNED8 ad7794_err = 0;
UNSIGNED8 ad7795_err = 0;
UNSIGNED8 ad7795_initiated = 0;


//debug
//extern CO_DT_INIT_VAR UNSIGNED16	p401_fault_mode_16[9];
//extern CO_DT_INIT_VAR INTEGER32	p401_read_anin_32[5];



/*
//niewykorzystywane w obecenj chwili

CO_CONST UNSIGNED16 Pt100[36] = {8031, 8229, 8427, 8625, 8822, 9019, 9216, 9412, 9609, 9804, 10000, 10195, 10390, 10585, 10779, 10973, 11167, 11361, 11554, 11747, 11940, 12132, 12324, 12516, 12707, 12898, 13089, 13280, 13470, 13660, 13850, 14039, 14229,
 15731, 
 17584, 
 19584};
 
INTEGER16 GetPt100Temperature2(UNSIGNED16 r){
	INTEGER16 t = -5000;
	INTEGER16 i = 0;
	INTEGER16 dt = 0;
 
	if (r > Pt100[0])
		while (25000 > t){
			dt = (t < 11000) ? 500 : (t > 11000) ? 5000 : 4000;
			if (r < Pt100 [++i])
				return t + ( r - Pt100[i-1])*dt/(Pt100[i]-Pt100[i-1]);
			t += dt;
		};
	return t;
}

INTEGER16 GetPt100Temperature1(UNSIGNED16 r){
	INTEGER32 t = 0;
//	INTEGER16 a = 0.1;
	INTEGER32 b = 235;
	INTEGER32 c = -24503;
	t = (r*r)/1000;
	t += r*b;
	t /= 100;
	t += c;
	if (t<-5000)
		return -5000;
	if (t>25000)
		return 25000;
	return (INTEGER16)t;
}
*/

INTEGER16 GetPt100Temperature(UNSIGNED16 r){
	INTEGER32 t = 0;
	INTEGER32 b = 263;
	INTEGER32 c = -26405;
	t = r;
	t *= b;
	t /= 100;
	t += c;
	if (t<-5000) { return -5000; }
	if (t>25000) { return 25000; }
	return (INTEGER16)t;
//	return (r*b)/100 + c;
}

//sprawdzenie czy dane sa gotowe
UNSIGNED8 ad779x_ready(UNSIGNED8 port){
	return !spi_readPin(port, SPI_MISO);
}

/*
void U1C0_send_SPI(UNSIGNED16 *uwTxData, UNSIGNED16 *uwRxData, UNSIGNED16 uwDataSize){
	UNSIGNED16 i;
	U1C0_SSC_vFlushRxFIFO();
	U1C0_SSC_vFlushTxFIFO();
	IO_vResetPin(SPI01_CS_IO_P0_3);
	U1C0_SSC_vFillTxFIFO(uwTxData, uwDataSize);
	while(U1C0_SSC_ubGetRxFIFOFillingLevel() < uwDataSize);
	IO_vSetPin(SPI01_CS_IO_P0_3);
	for (i=0;i<uwDataSize;i++)
		uwRxData[i] = U1C0_SSC_uwGetRxFIFOData();
}
*/

void ad779x_reset(UNSIGNED8 port){
	UNSIGNED16 uwDataSize = 0;
	ad779x_tx_ptr[uwDataSize++] = 0xFF;
	ad779x_tx_ptr[uwDataSize++] = 0xFF;
	ad779x_tx_ptr[uwDataSize++] = 0xFF;
	ad779x_tx_ptr[uwDataSize++] = 0xFF;
	spi_send(port, SPI_CS0, ad779x_tx_ptr, ad779x_rx_ptr, uwDataSize);
}

void ad779x_setFilter(UNSIGNED8 port, UNSIGNED8 filter){
	// 08 - zapis Mode, , 04 - Fadc 4=62Hz 3=123Hz
	UNSIGNED16 uwDataSize = 0;
	ad779x_tx_ptr[uwDataSize++] = AD779X_REG_MODE | AD779X_WRITE; 
//	ad779x_tx_ptr[uwDataSize++] = 0x00;
//	ad779x_tx_ptr[uwDataSize++] = 0x22;	//Single Conv + AMP-CM
	ad779x_tx_ptr[uwDataSize++] = 0x02;	//AMP-CM
	ad779x_tx_ptr[uwDataSize++] = (0x0F & filter);
	spi_send(port, SPI_CS0, ad779x_tx_ptr, ad779x_rx_ptr, uwDataSize);
}

UNSIGNED8 ad779x_readtype(UNSIGNED8 port, UNSIGNED8 *ch){
	UNSIGNED16 uwDataSize = 0;
	ad779x_tx_ptr[uwDataSize++] = AD779X_REG_STATUS | AD779X_READ;
	ad779x_tx_ptr[uwDataSize++] = 0x00;
	spi_send(port, SPI_CS0, ad779x_tx_ptr, ad779x_rx_ptr, uwDataSize);
	if (ad779x_rx_ptr[1] == 0xFF) { return 0x00; }
	*ch = (ad779x_rx_ptr[1] & 0x07);
	if (ad779x_rx_ptr[1] & 0x08) {
		return AD779X_AD7794;
	} else {
		return AD779X_AD7795;
	}
}
														  
void ad779x_setChannel(UNSIGNED8 port, UNSIGNED8 gain, UNSIGNED8 ch){
	// 010 - zapis Config, Unipolar, G=gain, Internal Uref=1,17V
	UNSIGNED16 uwDataSize = 0;
	ad779x_tx_ptr[uwDataSize++] = AD779X_REG_CONF | AD779X_WRITE;
	ad779x_tx_ptr[uwDataSize++] = 0x10 | (0x07 & gain);
	ad779x_tx_ptr[uwDataSize++] = (0x80 | ch);
	spi_send(port, SPI_CS0, ad779x_tx_ptr, ad779x_rx_ptr, uwDataSize);
}

void ad779x_setCurrent(UNSIGNED8 port, UNSIGNED8 prad){
    // 1->Io=10uA, 2->Io=210uA, 3->IO=1mA
	UNSIGNED16 uwDataSize = 0;
//	ad779x_curr = prad;
	ad779x_tx_ptr[uwDataSize++] = AD779X_REG_IO | AD779X_WRITE;
//	ad779x_tx_ptr[uwDataSize++] = 0x40 | (prad & 0x03);	//AIN6+/- set as IO
	ad779x_tx_ptr[uwDataSize++] = 0x00 | (prad & 0x03);
	spi_send(port, SPI_CS0, ad779x_tx_ptr, ad779x_rx_ptr, uwDataSize);
}

//odczyt data
void ad779x_readData(UNSIGNED8 port, UNSIGNED16 *uwRxData, int count){
	UNSIGNED16 uwDataSize = 0;
	ad779x_tx_ptr[uwDataSize++] = AD779X_REG_DATA | AD779X_READ;
	while (count-- > 0)
		ad779x_tx_ptr[uwDataSize++] = 0x00;
//	ad779x_tx_ptr[uwDataSize++] = 0x00;
//	ad779x_tx_ptr[uwDataSize++] = 0x00;
	spi_send(port, SPI_CS0, ad779x_tx_ptr, uwRxData, uwDataSize);
}

void ad779x_readReg(UNSIGNED8 port, UNSIGNED16 *uwRxData, UNSIGNED8 reg, int count){
	UNSIGNED16 uwDataSize = 0;
	ad779x_tx_ptr[uwDataSize++] = reg | AD779X_READ;
	while (count-- > 0)
		ad779x_tx_ptr[uwDataSize++] = 0x00;
//	ad779x_tx_ptr[uwDataSize++] = 0x00;
//	ad779x_tx_ptr[uwDataSize++] = 0x00;
	spi_send(port, SPI_CS0, ad779x_tx_ptr, uwRxData, uwDataSize);
}

UNSIGNED16 ad779x_readReg1(UNSIGNED8 port, UNSIGNED8 reg){
	UNSIGNED16 readreg[4];
	ad779x_readReg(port, readreg, reg, 1);
	return readreg[1];
}

/*
UNSIGNED32 ad779x_data2u32(UNSIGNED16 *uwTxData, UNSIGNED16 uwDataSize){
	UNSIGNED32 ad_val = 0x00000000UL;
	UNSIGNED8 i;
	for (i=1;i<uwDataSize;i++){
		ad_val <<= 8;
		ad_val |= uwTxData[i] & 0xff;
	}
	return ad_val;
}
*/

UNSIGNED32 ad779x_data2ohm(UNSIGNED16 *uwTxData, UNSIGNED16 uwDataSize, UNSIGNED8 wzmoc, UNSIGNED8 ad779x_curr){
	UNSIGNED32 ad_val = 0x00000000UL;
	UNSIGNED32 pom_R;
	UNSIGNED32 pom_RH;
	UNSIGNED32 pom_RL;
	UNSIGNED8 i;

	for (i=1;i<uwDataSize;i++){
		ad_val <<= 8;
		ad_val |= uwTxData[i] & 0xff;
	}
	pom_RH = (ad_val >> 16);
	pom_RL = (ad_val & 0x0000FFFFUL);

    if (ad779x_curr == 1){
		pom_RH *= 45703;	//1.17*100000*100/256
		pom_RL *= 45703;	//1.17*100000*100/256
    } else if (ad779x_curr == 2){
		pom_RH *= 557142;	//~1.17*4761.9*100
		pom_RL *= 557142;	//~1.17*4761.9*100
		pom_RH >>= 8;
		pom_RL >>= 8;
    } else if (ad779x_curr == 3) {
		pom_RH *= 58500;	//1.17*1000*100 /2
		pom_RL *= 58500;	//1.17*1000*100	/2
		pom_RH >>= 7;
		pom_RL >>= 7;
	}
	pom_RH >>= wzmoc;
	pom_RL >>= 16+wzmoc;

	pom_R = pom_RH + pom_RL;

	return pom_R;
}

/*
//niewykorzystywane w obecenj chwili
void ad779x_data_0_20mA(UNSIGNED16 *uwTxData, UNSIGNED16 uwDataSize, UNSIGNED8 wzmoc, INTEGER16 * ret_val, UNSIGNED8 offset){
	UNSIGNED32 ad_val = 0x00000000UL;
//	UNSIGNED32 pom_R;
	UNSIGNED32 pom_RH;
	UNSIGNED32 pom_RL;
	UNSIGNED16 i;

	for (i=1;i<uwDataSize;i++){
		ad_val <<= 8;
		ad_val |= uwTxData[i] & 0xff;
	}
	pom_RH = (ad_val >> 16);
	pom_RL = (ad_val & 0x0000FFFFUL);

	pom_RH *= 3000; //2^16/1,17V/10 Ohm
	pom_RL *= 3000; //2^16/1,17V/10 Ohm
	pom_RL >>= 16-5;
	pom_RH <<= 5;
//dodac korekcje dla wzmocnienia  != 1 (x2)
//	pom_R = pom_RH + pom_RL;
//	pom_RL = pom_RL & 0xFFFC;
//	if (pom_RL < 0x1000)
//		pom_RL |= 0x03;
	if (pom_RL > 0x7FFF)
		pom_RL = 0x7FFF;
	ret_val[offset] = pom_RL;
}

void ad779x_data_0_24mA(UNSIGNED16 *uwTxData, UNSIGNED16 uwDataSize, UNSIGNED8 wzmoc, INTEGER16 * ret_val, UNSIGNED8 offset){
	UNSIGNED32 ad_val = 0x00000000UL;
//	UNSIGNED32 pom_R;
	UNSIGNED32 pom_RH;
	UNSIGNED32 pom_RL;
	UNSIGNED16 i;

	for (i=1;i<uwDataSize;i++){
		ad_val <<= 8;
		ad_val |= uwTxData[i] & 0xff;
	}
	pom_RH = (ad_val >> 16);
	pom_RL = (ad_val & 0x0000FFFFUL);

	pom_RH *= 2500; //2^16/1,17V/10 Ohm
	pom_RL *= 2500; //2^16/1,17V/10 Ohm
	pom_RL >>= 16-5;
	pom_RH <<= 5;
//dodac korekcje dla wzmocnienia  != 1 (x2)
//	pom_R = pom_RH + pom_RL;
//	pom_RL = pom_RL & 0xFFFC;
//	if (pom_RL < 0x1000)
//		pom_RL |= 0x03;
	if (pom_RL > 0x7FFF)
		pom_RL = 0x7FFF;
	ret_val[offset] = pom_RL;
}
*/

void ad779x_data_4_20mA(UNSIGNED16 *uwTxData, UNSIGNED16 uwDataSize, UNSIGNED8 wzmoc, INTEGER16 * ret_val, UNSIGNED8 offset){
	UNSIGNED32 ad_val = 0x00000000UL;
//	UNSIGNED32 pom_R;
	UNSIGNED32 pom_RH;
	UNSIGNED32 pom_RL;
	UNSIGNED16 i;

	for (i=1;i<uwDataSize;i++){
		ad_val <<= 8;
		ad_val |= uwTxData[i] & 0xff;
	}
	pom_RH = (ad_val >> 16);
	pom_RL = (ad_val & 0x0000FFFFUL);

	pom_RH *= 3750; //2^16/1,17V/10 Ohm
	pom_RL *= 3750; //2^16/1,17V/10 Ohm
	pom_RL >>= 16-5;
	pom_RH <<= 5;
//dodac korekcje dla wzmocnienia  != 1 (x2)
//	pom_R = pom_RH + pom_RL;
	if (pom_RL < 0x13FF)
		pom_RL = 0x0003;
	else if (pom_RL < 0x1FFF)
		pom_RL = 0x0000;
	else if (pom_RL > 0xABFF)
		pom_RL = 0x7FFF;
	else if (pom_RL > 0x9FFF)
		pom_RL = 0x7FFC;
	else
		pom_RL = (pom_RL-0x1FFF) & 0xFFFC;
	ret_val[offset] = pom_RL;
}

void ad7794_loop(UNSIGNED8 port, INTEGER16* p401_an_in, UNSIGNED8 offset, UNSIGNED8 count, UNSIGNED8 sma){
	UNSIGNED32 sma_sumH = 0;
	UNSIGNED32 sma_sumL = 0;
	UNSIGNED32 sma_temp = 0;
	UNSIGNED8 sma_ch = 0;
	UNSIGNED8 sma_i = 0;
	UNSIGNED8 sma_count = 0;
	UNSIGNED32 pom_R = 0;
	UNSIGNED16 pom_R2 = 0;
	UNSIGNED16 status = 0;
	UNSIGNED16 curr = 0;
	UNSIGNED8 ad7794_ch = 0;
	UNSIGNED8 port2 = 0;
	switch (port){
		case U0C0: port2 = 0;	break;
		case U0C1: port2 = 1;	break;
		case U1C0: port2 = 2;	break;
		case U1C1: port2 = 3;	break;
		case U2C0: port2 = 4;	break;
		case U2C1: port2 = 5;	break;
		case U3C0: port2 = 6;	break;
		case U3C1: port2 = 7;	break;
		default: break;
	}
	//sprawdzenie czy dane sa gotowe
	if (ad779x_ready(port)){
		//wylaczenie CS
		spi_writePin(port, SPI_CS0, 1);

		curr = ad779x_readReg1(port, AD779X_REG_IO);
		status = ad779x_readReg1(port, AD779X_REG_STATUS);
		ad7794_ch = status & 0x07;
		if (ad779x_readtype(port, &ad7794_ch) == AD779X_AD7794){
			ad7794_ch = status & 0x07;
			if ((status & 0xC0) != 0x80){
				ad779x_readData(port, ad779x_rx_ptr, 3);
				pom_R = ad779x_data2ohm(ad779x_rx_ptr, 4, 0, curr);
/*
if (ad7794_ch == 1 && curr == 3) {
	p401_fault_mode_16[1] = ad779x_rx_ptr[0];
	p401_fault_mode_16[2] = ad779x_rx_ptr[1];
	p401_fault_mode_16[3] = ad779x_rx_ptr[2];
	p401_fault_mode_16[4] = ad779x_rx_ptr[3];

	p401_fault_mode_16[7] = pom_R >> 16;
	p401_fault_mode_16[8] = pom_R & 0xFFFF;	
}
*/
				//korekcja o 1MOhm na wejsciu; (odczyt_Ohm = (odczyt_Ohm * 1000000)/(1000000 - odczyt_Ohm); // odczyt_Ohm = pom_R/100
				if (pom_R > 100000){ // > 1kOhm -> korekcja
					pom_R = pom_R+(pom_R>>10)*3-2000;
				}

				if (sma > 0) {
					if (sma > 6) {
						sma = 6;
					}
					sma_ch = (ad7794_ch%count)+offset+count-1;
					if (pom_R << 3000000) {
						ulSMA[sma_ch][ulSMA_iter[sma_ch]] = pom_R;
					} else {
						ulSMA[sma_ch][ulSMA_iter[sma_ch]] = 0;
					}

					ulSMA_iter[sma_ch] = ulSMA_iter[sma_ch] + 1;
					sma_count = 1 << sma;
					if (ulSMA_iter[sma_ch] >= sma_count) {
						ulSMA_iter[sma_ch] = 0;
					}
					sma_sumH = 0;
					sma_sumL = 0;
					for (sma_i=0;sma_i<sma_count;sma_i++){
						sma_temp = ulSMA[sma_ch][sma_i];
						sma_sumH += (sma_temp >> 16);
						sma_sumL += (sma_temp & 0xFFFF);
					}
					pom_R = (sma_sumL >> sma) + (sma_sumH << (16-sma));
				}

//				if (sma_ch == 12){
//					p401_read_anin_32[1] = sma_count;
//					p401_read_anin_32[2] = ulSMA[sma_ch][0];
//					p401_read_anin_32[3] = ulSMA[sma_ch][1];
//					p401_read_anin_32[4] = sma_sumH;
//					p401_read_anin_32[5] = sma_sumL;
//					p401_read_anin_32[6] = (sma_sumL >> sma);
//					p401_read_anin_32[7] = (sma_sumH << (16-sma));
//					p401_read_anin_32[8] = pom_R;
//				}


				pom_R2 = pom_R & 0xFFFF;

				p401_an_in[(ad7794_ch%count)+offset+count] = pom_R/100;

				if (curr == 3) {
					p401_an_in[(ad7794_ch)+offset] = GetPt100Temperature(pom_R2);
				} else {
					p401_an_in[(ad7794_ch%count)+offset] = -5000;
				}
				if ((pom_R > 60000) && (ad7794_curr[port2*AD7794_CHANELLS+ad7794_ch] != 1)){			//PRINTF("Nad zakresem\n");
					ad7794_curr[port2*AD7794_CHANELLS+ad7794_ch] = 1;	//10uA
				} else if ((pom_R < 40000) && (ad7794_curr[port2*AD7794_CHANELLS+ad7794_ch] != 3)){	//PRINTF("Pod zakresem\n");
					ad7794_curr[port2*AD7794_CHANELLS+ad7794_ch] = 3;	//1mA
				}
				if (ad7794_err > 0) {
					ad7794_err--;
				}
			} else {
				if (ad7794_err < 100){
					ad7794_err++;
				} else {
					ad779x_init(port);
					ad7794_err = 0;
				}
			}

			ad7794_ch++;
			if (ad7794_ch >= count) {
				ad7794_ch = 0;
			}
				
			ad779x_setFilter(port, 4);
			ad779x_setCurrent(port, ad7794_curr[port2*AD7794_CHANELLS+ad7794_ch]);
			ad779x_setChannel(port, 0, ad7794_ch);
			//zalaczenie CS
			spi_writePin(port, SPI_CS0, 0);	//IO_vResetPin(SPI01_CS_IO_P0_3);
			//oczekiwanie na pomiar
		}
	}
	//pomiar czasu od ostatniego odczytu ewentualne zglocszenie bledu timeout i reinit
}

void ad7795_loop(UNSIGNED8 port, INTEGER16* p401_an_in, UNSIGNED8 offset, UNSIGNED8 count){
	UNSIGNED16 status = 0;
	UNSIGNED16 curr = 0;
	UNSIGNED8 ad7795_ch = 0;
	//sprawdzenie czy dane sa gotowe
	if (ad779x_ready(port)){
		//wylaczenie CS
		spi_writePin(port, SPI_CS0, 1);
		curr = ad779x_readReg1(port, AD779X_REG_IO);
		status = ad779x_readReg1(port, AD779X_REG_STATUS);
		ad7795_ch = status & 0x07;
		if (ad779x_readtype(port, &ad7795_ch) == AD779X_AD7795){
			if ((status & 0xC0) != 0x80){
				ad779x_readData(port, ad779x_rx_ptr, 2);
				ad779x_data_4_20mA(ad779x_rx_ptr, 3, 2, p401_an_in, (ad7795_ch%count)+offset);
				if (ad7795_err > 0) {
					ad7795_err--;
				}
			} else {
				if (ad7795_err < 100) {
					ad7795_err++;
				} else {
					ad779x_init(port);
					ad7795_err = 0;
				}
			}
			ad7795_ch++;
			if (ad7795_ch >= count) { ad7795_ch = 0; }
			ad779x_setChannel(port, 1, ad7795_ch%count);
			//zalaczenie CS
			spi_writePin(port, SPI_CS0, 0);	//IO_vResetPin(SPI01_CS_IO_P0_3);
			//oczekiwanie na pomiar
		}
	}
	//pomiar czasu od ostatniego odczytu ewentualne zglocszenie bledu timeout i reinit
}

void ad779x_init(UNSIGNED8 port){
	UNSIGNED8 i = 0;
	UNSIGNED8 j = 0;
	UNSIGNED8 ch = 0;
	// SPI CS HI
	spi_reset(port);
	ad779x_reset(port);

	for (j=0;j<SMA_CH_COUNT;j++) { ulSMA_iter[j] = 0; }

	if (ad779x_readtype(port, &ch) == AD779X_AD7794){
		if (!ad7795_initiated) {
			for (i=0;i<AD7794_CHANELLS;i++){
				for (j=0;j<8;j++) { ad7794_curr[j*AD7794_CHANELLS+i] = 3; }				//1mA
			}
		}
		ad7795_initiated = 1;
//		ad7794_ch = 0;
		ad779x_setFilter(port, 4);
		ad779x_setCurrent(port, ad7794_curr[0]);
//		ad779x_setCurrent(port, ad7794_curr[ad7794_ch]);
//		ad779x_setChannel(port, 0, ad7794_ch);
		ad779x_setChannel(port, 0, 0);
	} else {
//		for (i=0;i<AD7795_CHANELLS;i++)
//			ad7795_curr[i] = 0;
//		ad7795_ch = 0;
		ad779x_setFilter(port, 2);
		ad779x_setCurrent(port, 0);
		ad779x_setChannel(port, 1, 0);	//gain = 2 -> x4
//		ad779x_setChannel(port, 1, ad7795_ch);	//gain = 2 -> x4
//		ad779x_setCurrent(port, ad7795_curr[ad7795_ch]);
	}
	spi_writePin(port, SPI_CS0, 0);	//	IO_vResetPin(SPI01_CS_IO_P0_3);
}

