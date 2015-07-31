#include "ad54x2.h"

UNSIGNED16 ad54x2_nop_ptr[3] = {AD54X2_REG_NOP,AD54X2_REG_NOP,AD54X2_REG_NOP};
UNSIGNED16 ad54x2_tx_ptr[13];
UNSIGNED16 ad54x2_rx_ptr[13];

#ifndef AD54X2_DAISY_LENGTH
//	#error "AD54X2_DAISY_LENGTH undef"
	#pragma message ( "AD54X2_DAISY_LENGTH undef" )
	#define AD54X2_DAISY_LENGTH 0
#else 

void ad54x2_delayAll(UNSIGNED8 port, UNSIGNED8 cs){
	UNSIGNED8 i = AD54X2_DAISY_LENGTH;
	UNSIGNED16 uwDataSize = 0;
	ad54x2_tx_ptr[uwDataSize++] = 0xFF;
	ad54x2_tx_ptr[uwDataSize++] = 0xFF;
	ad54x2_tx_ptr[uwDataSize++] = 0xFF;

	spi_writePin(port, cs, 0); //IO_vResetPin(SPI01_CS_IO_P0_3);
	while (i--)
		spi_send(port, SPI_NO_CS, ad54x2_tx_ptr, ad54x2_rx_ptr, 3);
	spi_writePin(port, cs, 1); //IO_vSetPin(SPI01_CS_IO_P0_3);
}

void ad54x2_resetAll(UNSIGNED8 port, UNSIGNED8 cs){
	UNSIGNED8 i = AD54X2_DAISY_LENGTH;
	UNSIGNED16 uwDataSize = 0;
	ad54x2_tx_ptr[uwDataSize++] = AD54X2_REG_RESET;
	ad54x2_tx_ptr[uwDataSize++] = 0x00;
	ad54x2_tx_ptr[uwDataSize++] = 0x01;

	spi_writePin(port, cs, 0); //IO_vResetPin(SPI01_CS_IO_P0_3);
	while (i--)
		spi_send(port, SPI_NO_CS, ad54x2_tx_ptr, ad54x2_rx_ptr, 3);
	spi_writePin(port, cs, 1); //IO_vSetPin(SPI01_CS_IO_P0_3);
}

void ad54x2_readDataAll(UNSIGNED8 port, UNSIGNED8 cs, UNSIGNED16 *val, UNSIGNED8 r_reg){
	UNSIGNED8 i = AD54X2_DAISY_LENGTH;
	ad54x2_tx_ptr[0] = AD54X2_REG_READBACK;
	ad54x2_tx_ptr[1] = 0x00;
	ad54x2_tx_ptr[2] = r_reg;

	spi_writePin(port, cs, 0); //IO_vResetPin(SPI01_CS_IO_P0_3);
	while (i--)
		spi_send(port, SPI_NO_CS, ad54x2_tx_ptr, ad54x2_rx_ptr, 3);
	spi_writePin(port, cs, 1); //IO_vSetPin(SPI01_CS_IO_P0_3);

//	ad54x2_tx_ptr[0] = ad54x2_tx_ptr[1] = ad54x2_tx_ptr[2] = AD54X2_REG_NOP;

	i = AD54X2_DAISY_LENGTH;
	spi_writePin(port, cs, 0); //IO_vResetPin(SPI01_CS_IO_P0_3);
	while (i--){
		spi_send(port, SPI_NO_CS, ad54x2_nop_ptr, ad54x2_rx_ptr, 3);
		val[i] = (ad54x2_rx_ptr[1] & 0x00FF) << 8 | (ad54x2_rx_ptr[2] & 0x00FF);
	}
	spi_writePin(port, cs, 1); //IO_vSetPin(SPI01_CS_IO_P0_3);
}

void ad54x2_setConfigOne(UNSIGNED8 port, UNSIGNED8 cs, UNSIGNED8 reg, UNSIGNED16 val, UNSIGNED8 dc_poz){
	UNSIGNED8 i = AD54X2_DAISY_LENGTH;
	UNSIGNED16 j = 0;
	UNSIGNED16 uwDataSize = 0;
	ad54x2_tx_ptr[uwDataSize++] = reg;
	ad54x2_tx_ptr[uwDataSize++] = (val >> 8) & 0xFF;
	ad54x2_tx_ptr[uwDataSize++] = val & 0xFF;

	spi_writePin(port, cs, 0); //IO_vResetPin(SPI01_CS_IO_P0_3);
	while (i--)
		if (dc_poz==i){
			spi_send(port, SPI_NO_CS, ad54x2_tx_ptr, ad54x2_rx_ptr, 3);
			//delay 11us
			for (j=0;j<0xff;j++) //0xff = 11us; 0xffff = 3ms
				_nop();
		} else
			spi_send(port, SPI_NO_CS, ad54x2_nop_ptr, ad54x2_rx_ptr, 3);
	spi_writePin(port, cs, 1); //IO_vSetPin(SPI01_CS_IO_P0_3);
}

UNSIGNED16 ad54x2_readRegOne(UNSIGNED8 port, UNSIGNED8 cs, UNSIGNED8 r_reg, UNSIGNED8 dc_poz){
	UNSIGNED16 i = AD54X2_DAISY_LENGTH;
	UNSIGNED16 ret = 0;
	ad54x2_tx_ptr[0] = AD54X2_REG_READBACK;
	ad54x2_tx_ptr[1] = 0x00;
	ad54x2_tx_ptr[2] = r_reg;

	spi_writePin(port, cs, 0); //IO_vResetPin(SPI01_CS_IO_P0_3);
	while (i--)
		if (dc_poz==i){
			spi_send(port, SPI_NO_CS, ad54x2_tx_ptr, ad54x2_rx_ptr, 3);
		} else
			spi_send(port, SPI_NO_CS, ad54x2_nop_ptr, ad54x2_rx_ptr, 3);
	spi_writePin(port, cs, 1); //IO_vSetPin(SPI01_CS_IO_P0_3);

	for (i=0;i<0xff;i++) _nop();//0xff = 11us; 0xffff = 3ms

	i = AD54X2_DAISY_LENGTH;
	spi_writePin(port, cs, 0); //IO_vResetPin(SPI01_CS_IO_P0_3);
	while (i--){
		spi_send(port, SPI_NO_CS, ad54x2_nop_ptr, ad54x2_rx_ptr, 3);
		if (dc_poz==i){
			ret = (ad54x2_rx_ptr[1] & 0x00FF) << 8 | (ad54x2_rx_ptr[2] & 0x00FF);
		}
	}
	spi_writePin(port, cs, 1); //IO_vSetPin(SPI01_CS_IO_P0_3);
	return ret;
}


//zapis
void ad54x2_writeDataAll(UNSIGNED8 port, UNSIGNED8 cs, UNSIGNED8 reg, UNSIGNED16 *val){
	UNSIGNED8 i = AD54X2_DAISY_LENGTH;
	UNSIGNED16 j = 0;
	UNSIGNED16 uwDataSize = 0;
	ad54x2_tx_ptr[0] = reg;

	spi_writePin(port, cs, 0); //IO_vResetPin(SPI01_CS_IO_P0_3);
	while (i--){
		ad54x2_tx_ptr[1] = (val[i] >> 8) & 0xFF;
		ad54x2_tx_ptr[2] = val[i] & 0xFF;
		spi_send(port, SPI_NO_CS, ad54x2_tx_ptr, ad54x2_rx_ptr, 3);
	}
	spi_writePin(port, cs, 1); //IO_vSetPin(SPI01_CS_IO_P0_3);
	//delay 11us
	for (j=0;j<0xff;j++) //0xff = 11us; 0xffff = 3ms
		_nop();
}

#endif

//do przeniesienia do nowego pliku
UNSIGNED16 ad7814_read(UNSIGNED8 port, UNSIGNED8 cs){
	UNSIGNED16 ret = 0;
	spi_writePin(port, cs, 0);
	spi_send(port, SPI_NO_CS, ad54x2_nop_ptr, ad54x2_rx_ptr, 2);
//	ret = ((ad54x2_rx_ptr[0] & 0x00FF) << 7) | ((ad54x2_rx_ptr[1] & 0x00FF) >> 1);
	ret = ((ad54x2_rx_ptr[0] & 0x00FF) << 1) | ((ad54x2_rx_ptr[1] & 0x00FF) >> 7);
	spi_writePin(port, cs, 1); 	
	return ret;
}
