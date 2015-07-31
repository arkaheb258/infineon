#include <spi.h>

void spi_diag_out(UNSIGNED8 port){
	switch (port){
		case USIC0_CH1 :
			IO_vSetPinOut(SPI_DIAG1_1,IO_PUSH_PULL);
			IO_vSetPinOut(SPI_DIAG1_2,IO_PUSH_PULL);
		break;
		case USIC1_CH0 :
			IO_vSetPinOut(SPI_DIAG6_1,IO_PUSH_PULL);
			IO_vSetPinOut(SPI_DIAG6_2,IO_PUSH_PULL);
		break;
		case USIC1_CH1 :
			IO_vSetPinOut(SPI_DIAG3_1,IO_PUSH_PULL);
			IO_vSetPinOut(SPI_DIAG3_2,IO_PUSH_PULL);
		break;
		case USIC2_CH1 :
			IO_vSetPinOut(SPI_DIAG4_1,IO_PUSH_PULL);
			IO_vSetPinOut(SPI_DIAG4_2,IO_PUSH_PULL);
		break;
		case USIC3_CH0 :
			IO_vSetPinOut(SPI_DIAG5_1,IO_PUSH_PULL);
			IO_vSetPinOut(SPI_DIAG5_2,IO_PUSH_PULL);
		break;
		case USIC3_CH1 :
			IO_vSetPinOut(SPI_DIAG2_1,IO_PUSH_PULL);
			IO_vSetPinOut(SPI_DIAG2_2,IO_PUSH_PULL);
		break;
	}
}

void spi_diag_in(UNSIGNED8 port){
	switch (port){
		case USIC0_CH1 :
			IO_vSetPinIn(SPI_DIAG1_1,IO_NO_PULL);
			IO_vSetPinIn(SPI_DIAG1_2,IO_NO_PULL);
		break;
		case USIC1_CH0 :
			IO_vSetPinIn(SPI_DIAG6_1,IO_NO_PULL);
			IO_vSetPinIn(SPI_DIAG6_2,IO_NO_PULL);
		break;
		case USIC1_CH1 :
			IO_vSetPinIn(SPI_DIAG3_1,IO_NO_PULL);
			IO_vSetPinIn(SPI_DIAG3_2,IO_NO_PULL);
		break;
		case USIC2_CH1 :
			IO_vSetPinIn(SPI_DIAG4_1,IO_NO_PULL);
			IO_vSetPinIn(SPI_DIAG4_2,IO_NO_PULL);
		break;
		case USIC3_CH0 :
			IO_vSetPinIn(SPI_DIAG5_1,IO_NO_PULL);
			IO_vSetPinIn(SPI_DIAG5_2,IO_NO_PULL);
		break;
		case USIC3_CH1 :
			IO_vSetPinIn(SPI_DIAG2_1,IO_NO_PULL);
			IO_vSetPinIn(SPI_DIAG2_2,IO_NO_PULL);
		break;
	}
}


UNSIGNED8 spi_miso(UNSIGNED8 port){
	switch (port){
		case USIC0_CH1 :
			return IO_ubReadPin(SPI_MISO1);
//		break;
		case USIC1_CH0 :
			return IO_ubReadPin(SPI_MISO6);
//		break;
		case USIC1_CH1 :
			return IO_ubReadPin(SPI_MISO3);
//		break;
		case USIC2_CH1 :
			return IO_ubReadPin(SPI_MISO4);
//		break;
		case USIC3_CH0 :
			return IO_ubReadPin(SPI_MISO5);
//		break;
		case USIC3_CH1 :
			return IO_ubReadPin(SPI_MISO2);
//		break;
	}
	return 0;
}

UNSIGNED8 spi_diagn_1(UNSIGNED8 port){
	switch (port){
		case USIC0_CH1 :
			return IO_ubReadPin(SPI_DIAG1_1);
		case USIC1_CH0 :
			return IO_ubReadPin(SPI_DIAG6_1);
		case USIC1_CH1 :
			return IO_ubReadPin(SPI_DIAG3_1);
		case USIC2_CH1 :
			return IO_ubReadPin(SPI_DIAG4_1);
		case USIC3_CH0 :
			return IO_ubReadPin(SPI_DIAG5_1);
		case USIC3_CH1 :
			return IO_ubReadPin(SPI_DIAG2_1);
	}
	return 0;
}

UNSIGNED8 spi_diagn_2(UNSIGNED8 port){
	switch (port){
		case USIC0_CH1 :
			return IO_ubReadPin(SPI_DIAG1_2);
		case USIC1_CH0 :
			return IO_ubReadPin(SPI_DIAG6_2);
		case USIC1_CH1 :
			return IO_ubReadPin(SPI_DIAG3_2);
		case USIC2_CH1 :
			return IO_ubReadPin(SPI_DIAG4_2);
		case USIC3_CH0 :
			return IO_ubReadPin(SPI_DIAG5_2);
		case USIC3_CH1 :
			return IO_ubReadPin(SPI_DIAG2_2);
	}
	return 0;
}

void spi_cs(UNSIGNED8 port, UNSIGNED8 pin, UNSIGNED8 state){
	if (pin == SPI_NO_CS) return;
	switch (port){
		case USIC0_CH1 :
			switch (pin){
				case SPI_CS0 :
					if (state)
						IO_vSetPin(SPI_CS1_0);
					else
						IO_vResetPin(SPI_CS1_0);
				break;
				case SPI_RESET :
					if (state)
						IO_vSetPin(SPI_RESET1);
					else
						IO_vResetPin(SPI_RESET1);
				break;
				case SPI_DIAG1 :
					if (state)
						IO_vSetPin(SPI_DIAG1_1);
					else
						IO_vResetPin(SPI_DIAG1_1);
				break;
				case SPI_DIAG2 :
					if (state)
						IO_vSetPin(SPI_DIAG1_2);
					else
						IO_vResetPin(SPI_DIAG1_2);
				break;
				case SPI_DIAG12 :
					if (state)
						IO_vSetPin(SPI_DIAG1_1);
					else
						IO_vResetPin(SPI_DIAG1_1);
					if (state)
						IO_vSetPin(SPI_DIAG1_2);
					else
						IO_vResetPin(SPI_DIAG1_2);
				break;
			}
		break;
		case USIC1_CH0 :
			switch (pin){
				case SPI_CS0 :
					if (state)
						IO_vSetPin(SPI_CS6_0);
					else
						IO_vResetPin(SPI_CS6_0);
				break;
				case SPI_RESET :
					if (state)
						IO_vSetPin(SPI_RESET6);
					else
						IO_vResetPin(SPI_RESET6);
				break;
				case SPI_DIAG1 :
					if (state)
						IO_vSetPin(SPI_DIAG6_1);
					else
						IO_vResetPin(SPI_DIAG6_1);
				break;
				case SPI_DIAG2 :
					if (state)
						IO_vSetPin(SPI_DIAG6_2);
					else
						IO_vResetPin(SPI_DIAG6_2);
				break;
				case SPI_DIAG12 :
					if (state)
						IO_vSetPin(SPI_DIAG6_1);
					else
						IO_vResetPin(SPI_DIAG6_1);
					if (state)
						IO_vSetPin(SPI_DIAG6_2);
					else
						IO_vResetPin(SPI_DIAG6_2);
				break;
			}
		break;
		case USIC1_CH1 :
			switch (pin){
				case SPI_CS0 :
					if (state)
						IO_vSetPin(SPI_CS3_0);
					else
						IO_vResetPin(SPI_CS3_0);
				break;
				case SPI_RESET :
					if (state)
						IO_vSetPin(SPI_RESET3);
					else
						IO_vResetPin(SPI_RESET3);
				break;
				case SPI_DIAG1 :
					if (state)
						IO_vSetPin(SPI_DIAG3_1);
					else
						IO_vResetPin(SPI_DIAG3_1);
				break;
				case SPI_DIAG2 :
					if (state)
						IO_vSetPin(SPI_DIAG3_2);
					else
						IO_vResetPin(SPI_DIAG3_2);
				break;
				case SPI_DIAG12 :
					if (state)
						IO_vSetPin(SPI_DIAG3_1);
					else
						IO_vResetPin(SPI_DIAG3_1);
					if (state)
						IO_vSetPin(SPI_DIAG3_2);
					else
						IO_vResetPin(SPI_DIAG3_2);
				break;
			}
		break;
		case USIC2_CH1 :
			switch (pin){
				case SPI_CS0 :
					if (state)
						IO_vSetPin(SPI_CS4_0);
					else
						IO_vResetPin(SPI_CS4_0);
				break;
				case SPI_RESET :
					if (state)
						IO_vSetPin(SPI_RESET4);
					else
						IO_vResetPin(SPI_RESET4);
				break;
				case SPI_DIAG1 :
					if (state)
						IO_vSetPin(SPI_DIAG4_1);
					else
						IO_vResetPin(SPI_DIAG4_1);
				break;
				case SPI_DIAG2 :
					if (state)
						IO_vSetPin(SPI_DIAG4_2);
					else
						IO_vResetPin(SPI_DIAG4_2);
				break;
				case SPI_DIAG12 :
					if (state)
						IO_vSetPin(SPI_DIAG4_1);
					else
						IO_vResetPin(SPI_DIAG4_1);
					if (state)
						IO_vSetPin(SPI_DIAG4_2);
					else
						IO_vResetPin(SPI_DIAG4_2);
				break;
			}
		break;
		case USIC3_CH0 :
			switch (pin){
				case SPI_CS0 :
					if (state)
						IO_vSetPin(SPI_CS5_0);
					else
						IO_vResetPin(SPI_CS5_0);
				break;
				case SPI_RESET :
					if (state)
						IO_vSetPin(SPI_RESET5);
					else
						IO_vResetPin(SPI_RESET5);
				break;
				case SPI_DIAG1 :
					if (state)
						IO_vSetPin(SPI_DIAG5_1);
					else
						IO_vResetPin(SPI_DIAG5_1);
				break;
				case SPI_DIAG2 :
					if (state)
						IO_vSetPin(SPI_DIAG5_2);
					else
						IO_vResetPin(SPI_DIAG5_2);
				break;
				case SPI_DIAG12 :
					if (state)
						IO_vSetPin(SPI_DIAG5_1);
					else
						IO_vResetPin(SPI_DIAG5_1);
					if (state)
						IO_vSetPin(SPI_DIAG5_2);
					else
						IO_vResetPin(SPI_DIAG5_2);
				break;
			}
		break;
		case USIC3_CH1 :
			switch (pin){
				case SPI_CS0 :
					if (state)
						IO_vSetPin(SPI_CS2_0);
					else
						IO_vResetPin(SPI_CS2_0);
				break;
				case SPI_RESET :
					if (state)
						IO_vSetPin(SPI_RESET2);
					else
						IO_vResetPin(SPI_RESET2);
				break;
				case SPI_DIAG1 :
					if (state)
						IO_vSetPin(SPI_DIAG2_1);
					else
						IO_vResetPin(SPI_DIAG2_1);
				break;
				case SPI_DIAG2 :
					if (state)
						IO_vSetPin(SPI_DIAG2_2);
					else
						IO_vResetPin(SPI_DIAG2_2);
				break;
				case SPI_DIAG12 :
					if (state)
						IO_vSetPin(SPI_DIAG2_1);
					else
						IO_vResetPin(SPI_DIAG2_1);
					if (state)
						IO_vSetPin(SPI_DIAG2_2);
					else
						IO_vResetPin(SPI_DIAG2_2);
				break;
			}
		break;
	}
}

void spi_reset(UNSIGNED8 port){
	UNSIGNED16 j = 0;
	spi_diag_in(port);
	spi_cs(port, SPI_CS0, 1);
	spi_cs(port, SPI_RESET, 0);
	for (j=0;j<0xff;j++) //0xff = 11us; 0xffff = 3ms
		_nop();
	spi_cs(port, SPI_RESET, 1);
}

void send_SPI(UNSIGNED8 port, UNSIGNED8 cs, UNSIGNED16 *uwTxData, UNSIGNED16 *uwRxData, UNSIGNED16 uwDataSize){
	UNSIGNED16 i;
	UNSIGNED8 ptr = 0;
	switch (port){
		case U0C1 :
			SPI_FLUSH_RX(U0C1);
			SPI_FLUSH_TX(U0C1);
			spi_cs(port, cs, 0);
			while (uwDataSize > ptr){
				if (uwDataSize - ptr > SPI_FIFO_SIZE){
					SPI_FILL_TX(U0C1, uwTxData + ptr, SPI_FIFO_SIZE);
					while(SPI_FILLING_RX(U0C1) < SPI_FIFO_SIZE);
					for (i=0;i<SPI_FIFO_SIZE;i++){
						uwRxData[i + ptr] = SPI_GET_RX(U0C1);
					}
					ptr += SPI_FIFO_SIZE;
				} else {
					SPI_FILL_TX(U0C1, uwTxData + ptr, uwDataSize - ptr);
					while(SPI_FILLING_RX(U0C1) < uwDataSize - ptr);
					for (i=0;i<uwDataSize - ptr;i++){
						uwRxData[i + ptr] = SPI_GET_RX(U0C1);
					}
					ptr = uwDataSize;
				}
			}
			spi_cs(port, cs, 1);
		break;
		case U1C0 :
			SPI_FLUSH_RX(U1C0);
			SPI_FLUSH_TX(U1C0);
			spi_cs(port, cs, 0);
			while (uwDataSize > ptr){
				if (uwDataSize - ptr > SPI_FIFO_SIZE){
					SPI_FILL_TX(U1C0, uwTxData + ptr, SPI_FIFO_SIZE);
					while(SPI_FILLING_RX(U1C0) < SPI_FIFO_SIZE);
					for (i=0;i<SPI_FIFO_SIZE;i++)
						uwRxData[i + ptr] = SPI_GET_RX(U1C0);
					ptr += SPI_FIFO_SIZE;
				} else {
					SPI_FILL_TX(U1C0, uwTxData + ptr, uwDataSize - ptr);
					while(SPI_FILLING_RX(U1C0) < uwDataSize - ptr);
					for (i=0;i<uwDataSize - ptr;i++)
						uwRxData[i + ptr] = SPI_GET_RX(U1C0);
					ptr = uwDataSize;
				}
			}
			spi_cs(port, cs, 1);
		break;
		case U1C1 :
			SPI_FLUSH_RX(U1C1);
			SPI_FLUSH_TX(U1C1);
			spi_cs(port, cs, 0);
			while (uwDataSize > ptr){
				if (uwDataSize - ptr > SPI_FIFO_SIZE){
					SPI_FILL_TX(U1C1, uwTxData + ptr, SPI_FIFO_SIZE);
					while(SPI_FILLING_RX(U1C1) < SPI_FIFO_SIZE);
					for (i=0;i<SPI_FIFO_SIZE;i++)
						uwRxData[i + ptr] = SPI_GET_RX(U1C1);
					ptr += SPI_FIFO_SIZE;
				} else {
					SPI_FILL_TX(U1C1, uwTxData + ptr, uwDataSize - ptr);
					while(SPI_FILLING_RX(U1C1) < uwDataSize - ptr);
					for (i=0;i<uwDataSize - ptr;i++)
						uwRxData[i + ptr] = SPI_GET_RX(U1C1);
					ptr = uwDataSize;
				}
			}
			spi_cs(port, cs, 1);
		break;
		case U2C1 :
			SPI_FLUSH_RX(U2C1);
			SPI_FLUSH_TX(U2C1);
			spi_cs(port, cs, 0);
			while (uwDataSize > ptr){
				if (uwDataSize - ptr > SPI_FIFO_SIZE){
					SPI_FILL_TX(U2C1, uwTxData + ptr, SPI_FIFO_SIZE);
					while(SPI_FILLING_RX(U2C1) < SPI_FIFO_SIZE);
					for (i=0;i<SPI_FIFO_SIZE;i++)
						uwRxData[i + ptr] = SPI_GET_RX(U2C1);
					ptr += SPI_FIFO_SIZE;
				} else {
					SPI_FILL_TX(U2C1, uwTxData + ptr, uwDataSize - ptr);
					while(SPI_FILLING_RX(U2C1) < uwDataSize - ptr);
					for (i=0;i<uwDataSize - ptr;i++)
						uwRxData[i + ptr] = SPI_GET_RX(U2C1);
					ptr = uwDataSize;
				}
			}
			spi_cs(port, cs, 1);
		break;
		case U3C0 :
			SPI_FLUSH_RX(U3C0);
			SPI_FLUSH_TX(U3C0);
			spi_cs(port, cs, 0);
			while (uwDataSize > ptr){
				if (uwDataSize - ptr > SPI_FIFO_SIZE){
					SPI_FILL_TX(U3C0, uwTxData + ptr, SPI_FIFO_SIZE);
					while(SPI_FILLING_RX(U3C0) < SPI_FIFO_SIZE);
					for (i=0;i<SPI_FIFO_SIZE;i++){
						uwRxData[i + ptr] = SPI_GET_RX(U3C0);
					}
					ptr += SPI_FIFO_SIZE;
				} else {
					SPI_FILL_TX(U3C0, uwTxData + ptr, uwDataSize - ptr);
					while(SPI_FILLING_RX(U3C0) < uwDataSize - ptr);
					for (i=0;i<uwDataSize - ptr;i++){
						uwRxData[i + ptr] = SPI_GET_RX(U3C0);
					}
					ptr = uwDataSize;
				}
			}
			spi_cs(port, cs, 1);
		break;
		case U3C1 :
			SPI_FLUSH_RX(U3C1);
			SPI_FLUSH_TX(U3C1);
			spi_cs(port, cs, 0);
			while (uwDataSize > ptr){
				if (uwDataSize - ptr > SPI_FIFO_SIZE){
					SPI_FILL_TX(U3C1, uwTxData + ptr, SPI_FIFO_SIZE);
					while(SPI_FILLING_RX(U3C1) < SPI_FIFO_SIZE);
					for (i=0;i<SPI_FIFO_SIZE;i++)
						uwRxData[i + ptr] = SPI_GET_RX(U3C1);
					ptr += SPI_FIFO_SIZE;
				} else {
					SPI_FILL_TX(U3C1, uwTxData + ptr, uwDataSize - ptr);
					while(SPI_FILLING_RX(U3C1) < uwDataSize - ptr);
					for (i=0;i<uwDataSize - ptr;i++)
						uwRxData[i + ptr] = SPI_GET_RX(U3C1);
					ptr = uwDataSize;
				}
			}
			spi_cs(port, cs, 1);
		break;
	}
}
