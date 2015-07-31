#include <kps_4_x.h>

UNSIGNED16 kps_4_x_rx_ptr[16];
UNSIGNED16 kps_4_x_tx_ptr[16];
UNSIGNED16 ad54x2_vals[AD54X2_DAISY_LENGTH];

UNSIGNED8 KPS_check(UNSIGNED8 port){
	UNSIGNED8 i=0;
	UNSIGNED8 temp[8] = {0,0,0,0,0,0,0,0};
	spi_reset(port);
	mcp23S17_writeByte(port, 0x00, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
	mcp23S17_writeByte(port, 0x01, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
	mcp23S17_writeByte(port, 0x02, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
//	mcp23S17_writeByte(port, 0x03, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
	mcp23S17_writeByte(port, 0x00, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
	ad54x2_delayAll(port, SPI_NO_CS);
	mcp23S17_writeByte(port, 0x04, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
	mcp23S17_writeByte(port, 0x05, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
	mcp23S17_writeByte(port, 0x06, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
	mcp23S17_writeByte(port, 0x07, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
//	mcp23S17_writeByte(port, 0x00, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
	ad54x2_delayAll(port, SPI_NO_CS);
	for (i=0;i<4;i++){
		temp[i] = mcp23S17_readByte(port, i, MCP23S17_IOCON);
		temp[i] = (temp[i] == 0x08);
	}
	ad54x2_delayAll(port, SPI_NO_CS);
	for (i=4;i<8;i++){
		temp[i] = mcp23S17_readByte(port, i, MCP23S17_IOCON);
		temp[i] = (temp[i] == 0x08);
	}

	//4.1v10
	if (temp[1] && temp[6] && !(temp[0] || temp[4] || temp[7]))
		return 0x47;
	//4.1v11
	if (temp[2] && temp[5] && !(temp[0] || temp[4] || temp[7]))
		return 0x41;
	//4.2
	if (temp[0] && temp[4] && !(temp[1] || temp[6] || temp[7]))
		return 0x42;
	//4.2 - bez nakladki (4.0)
	if (temp[0] && !(temp[1] || temp[6] || temp[7]))
		return 0x40;
	//5.1
	if (temp[7] && !(temp[0] || temp[1] || temp[4] || temp[6]))
		return 0x51;
	ad779x_reset(port);
	//5.2
	return ad779x_readtype(port);
}

void KPS_4_1_init_old(UNSIGNED8 port){
	spi_reset(port);
	mcp23S17_writeByte(port, 0x00, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
	mcp23S17_writeByte(port, 0x01, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
	mcp23S17_writeByte(port, 0x06, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu

	mcp23S17_writeWord(port, 0x01, MCP23S17_IODIRA, 0xFFFF);	//wszystkie bity jako wejscia
	mcp23S17_writeWord(port, 0x01, MCP23S17_IPOLA, 0xFFFF);	//odwrocenie polaryzacji
	mcp23S17_writeByte(port, 0x06, MCP23S17_IODIRA, 0x07);	//bity 3-7 jako wyjscia
//	mcp23S17_writeByte(port, 0x06, MCP23S17_IPOLA, ~0x07);		// odwrocenie polaryzacji
	mcp23S17_writeByte(port, 0x06, MCP23S17_IODIRB, 0xE0);	//bity 0-4 jako wyjscia
//	mcp23S17_writeByte(port, 0x06, MCP23S17_IPOLB, ~0xE0);		// odwrocenie polaryzacji
}

void KPS_4_1_init(UNSIGNED8 port){
	spi_reset(port);
	mcp23S17_writeByte(port, 0x00, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
	mcp23S17_writeByte(port, 0x02, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
	mcp23S17_writeByte(port, 0x05, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu

	mcp23S17_writeWord(port, 0x02, MCP23S17_IODIRA, 0xFFFF);	//wszystkie bity jako wejscia
	mcp23S17_writeWord(port, 0x02, MCP23S17_IPOLA, 0xFFFF);	//odwrocenie polaryzacji
	mcp23S17_writeByte(port, 0x05, MCP23S17_IODIRA, 0x03);	//bity 2-7 jako wyjscia
//	mcp23S17_writeByte(port, 0x05, MCP23S17_IPOLA, ~0x03);		// odwrocenie polaryzacji
	mcp23S17_writeByte(port, 0x05, MCP23S17_IODIRB, 0xC0);	//bity 0-5 jako wyjscia
//	mcp23S17_writeByte(port, 0x05, MCP23S17_IPOLB, ~0xC0);		// odwrocenie polaryzacji
}

void KPS_4_1_loop_w_old(UNSIGNED8 port, INTEGER16* p401_write_output, UNSIGNED8 write_offset){
	mcp23S17_readData(port, kps_4_x_rx_ptr, 0x06, MCP23S17_IOCON, 1);
	if (kps_4_x_rx_ptr[2] == 0x00)
		KPS_4_1_init(port);
	mcp23S17_readData(port, kps_4_x_rx_ptr, 0x01, MCP23S17_IOCON, 1);
	if (kps_4_x_rx_ptr[2] == 0x00)
		KPS_4_1_init(port);
	mcp23S17_writeByte(port, 0x06, MCP23S17_GPIOA, ~((p401_write_output[write_offset] & 0x1F) << 3) & 0xFF);
	mcp23S17_writeByte(port, 0x06, MCP23S17_GPIOB, ~((p401_write_output[write_offset] & 0x03E0) >> 5) & 0xFF);
//	mcp23S17_readData(port, kps_4_x_rx_ptr, 0x06, MCP23S17_GPIOA, 4);
//	mcp23S17_readData(port, kps_4_x_rx_ptr, 0x06, MCP23S17_IODIRA, 4);
}

void KPS_4_1_loop_w(UNSIGNED8 port, INTEGER16* p401_write_output, UNSIGNED8 write_offset){
	UNSIGNED16 temp = 0;
	UNSIGNED8 portA = 0;
	UNSIGNED8 portB = 0;
	mcp23S17_readData(port, kps_4_x_rx_ptr, 0x05, MCP23S17_IOCON, 1);
	if (kps_4_x_rx_ptr[2] == 0x00)
		KPS_4_1_init(port);
	mcp23S17_readData(port, kps_4_x_rx_ptr, 0x02, MCP23S17_IOCON, 1);
	if (kps_4_x_rx_ptr[2] == 0x00)
		KPS_4_1_init(port);

	temp = p401_write_output[write_offset];
	portA |= (temp & (1<<0))  << 2;  //S1  =  0bit = GPA2
	portA |= (temp & (1<<6))  >> 3;  //S7  =  6bit = GPA3
	portA |= (temp & (1<<1))  << 3;  //S2  =  1bit = GPA4
	portA |= (temp & (1<<7))  >> 2;  //S8  =  7bit = GPA5
	portA |= (temp & (1<<2))  << 4;  //S3  =  2bit = GPA6
	portA |= (temp & (1<<8))  >> 1;  //S9  =  8bit = GPA7

	portB |= (temp & (1<<3))  >> 3;  //S4  =  3bit = GPB0
	portB |= (temp & (1<<9))  >> 8;  //S10 =  9bit = GPB1
	portB |= (temp & (1<<4))  >> 2;  //S5  =  4bit = GPB2
	portB |= (temp & (1<<10)) >> 7;  //S11 = 10bit = GPB3
	portB |= (temp & (1<<5))  >> 1;  //S6  =  5bit = GPB4
	portB |= (temp & (1<<11)) >> 6;  //S12 = 11bit = GPB5

	mcp23S17_writeByte(port, 0x05, MCP23S17_GPIOA, ~portA);
	mcp23S17_writeByte(port, 0x05, MCP23S17_GPIOB, ~portB);
}

void KPS_4_1_loop_r_old(UNSIGNED8 port, INTEGER16* p401_read_output, UNSIGNED8 read_offset){
	UNSIGNED16 temp = 0;
	UNSIGNED8 portA = 0;
	UNSIGNED8 portB = 0;
	mcp23S17_readData(port, kps_4_x_rx_ptr, 0x01, MCP23S17_GPIOA, 1);
	portA = kps_4_x_rx_ptr[2];
	mcp23S17_readData(port, kps_4_x_rx_ptr, 0x01, MCP23S17_GPIOB, 1);
	portB = kps_4_x_rx_ptr[2];

	temp |= (portA & (1<<3)) << 6 ;  //9
	temp |= (portA & (1<<4)) << 4 ;  //8
	temp |= (portA & (1<<5)) << 2 ;  //7
	temp |= (portA & (1<<6)) >> 0 ;  //6
	temp |= (portA & (1<<7)) >> 2 ;  //5

	temp |= (portB & (1<<0)) << 4 ;  //4
	temp |= (portB & (1<<1)) << 2 ;  //3
	temp |= (portB & (1<<2)) >> 0 ;  //2
	temp |= (portB & (1<<3)) >> 2 ;  //1
	temp |= (portB & (1<<4)) >> 4 ;	 //0

	p401_read_output[read_offset] = temp;
}

void KPS_4_1_loop_r(UNSIGNED8 port, INTEGER16* p401_read_output, UNSIGNED8 read_offset){
	UNSIGNED16 temp = 0;
	UNSIGNED8 portA = 0;
	UNSIGNED8 portB = 0;
	mcp23S17_readData(port, kps_4_x_rx_ptr, 0x02, MCP23S17_GPIOA, 1);
	portA = kps_4_x_rx_ptr[2];
	mcp23S17_readData(port, kps_4_x_rx_ptr, 0x02, MCP23S17_GPIOB, 1);
	portB = kps_4_x_rx_ptr[2];

	temp |= (portA & (1<<2)) << 8 ;  //12
	temp |= (portA & (1<<3)) << 3 ;  //6
	temp |= (portA & (1<<4)) << 7 ;  //11
	temp |= (portA & (1<<5)) << 0 ;  //5
	temp |= (portA & (1<<6)) << 4 ;  //10
	temp |= (portA & (1<<7)) >> 3 ;  //4

	temp |= (portB & (1<<0)) << 9 ;  //9
	temp |= (portB & (1<<1)) << 2 ;  //3
	temp |= (portB & (1<<2)) << 6 ;  //8
	temp |= (portB & (1<<3)) >> 1 ;  //2
	temp |= (portB & (1<<4)) << 3 ;	 //7
	temp |= (portB & (1<<5)) >> 4 ;	 //1

	p401_read_output[read_offset] = temp;
}

void KPS_4_1_loop_old(UNSIGNED8 port, INTEGER16* p401_write_output, UNSIGNED8 write_offset, INTEGER16* p401_read_output, UNSIGNED8 read_offset){
	KPS_4_1_loop_w_old(port, p401_write_output, write_offset);
	KPS_4_1_loop_r_old(port, p401_read_output, read_offset);
}

void KPS_4_1_loop(UNSIGNED8 port, INTEGER16* p401_write_output, UNSIGNED8 write_offset, INTEGER16* p401_read_output, UNSIGNED8 read_offset){
	KPS_4_1_loop_w(port, p401_write_output, write_offset);
	KPS_4_1_loop_r(port, p401_read_output, read_offset);
}

void KPS_5_1_init(UNSIGNED8 port){
	mcp23S17_writeByte(port, 0x00, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
	mcp23S17_writeByte(port, 0x07, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
	mcp23S17_writeWord(port, 0x07, MCP23S17_IODIRA, 0xFFFF);	//wszystkie bity jako wejscia
	mcp23S17_writeWord(port, 0x07, MCP23S17_IPOLA, 0xFFFF);	//odwrocenie polaryzacji
	
}

void KPS_5_1_loop(UNSIGNED8 port, INTEGER16* p401_read_output, UNSIGNED8 read_offset){
	UNSIGNED16 temp = 0;
	mcp23S17_readData(port, kps_4_x_rx_ptr, 0x07, MCP23S17_IOCON, 1);
	if ((kps_4_x_rx_ptr[2] & 0xFF) != 0x08)
		KPS_5_1_init(port);
	mcp23S17_readData(port, kps_4_x_rx_ptr, 0x07, MCP23S17_GPIOA, 2);
	temp = (kps_4_x_rx_ptr[2] & 0x00FF) << 8;
	temp |= (kps_4_x_rx_ptr[3] & 0x00FF);
	p401_read_output[read_offset] = temp;
}

void KPS_4_2_init(UNSIGNED8 port){
	UNSIGNED8 i;
	spi_reset(port);
	spi_writePin(port, SPI_DIAG12, 1);
	spi_setDiagOutput(port);
	ad54x2_resetAll(port, SPI_DIAG12);
	i = 0;
	while (i<AD54X2_DAISY_LENGTH){
		ad54x2_setConfigOne(port, SPI_DIAG12, AD54X2_REG_CONTROL, 0x11BE, i++);
	}
	ad54x2_delayAll(port, SPI_DIAG12);

	mcp23S17_writeByte(port, 0x00, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
	mcp23S17_writeByte(port, 0x04, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu
	mcp23S17_writeByte(port, 0x00, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu

	mcp23S17_writeWord(port, 0x00, MCP23S17_IODIRA, 0x0000);	//wszystkie bity jako wyjscia
	mcp23S17_writeWord(port, 0x04, MCP23S17_IODIRA, 0x0000);	//wszystkie bity jako wyjscia
//	mcp23S17_writeWord(port, 0x00, MCP23S17_IPOLA, 0xFFFF);	//odwrocenie polaryzacji
//	mcp23S17_writeWord(port, 0x04, MCP23S17_IPOLA, 0xFFFF);	//odwrocenie polaryzacji
	mcp23S17_writeWord(port, 0x00, MCP23S17_GPIOA, 0xFFFF);
	mcp23S17_writeWord(port, 0x04, MCP23S17_GPIOA, 0xFFFF);
}

void KPS_4_2_loop(UNSIGNED8 port, INTEGER16* p401_write_output, UNSIGNED8 write_offset){
	UNSIGNED8 temp;
	UNSIGNED8 temp2;
	INTEGER16 temp3;
	UNSIGNED8 i;
/*
	for (i=0;i<AD54X2_DAISY_LENGTH;i++)
		if (kps_4_x_rx_ptr[i] != 0x11BE){
			KPS_4_2_init(port);
			return;
		}
*/
	ad54x2_readDataAll(port, SPI_DIAG2, kps_4_x_rx_ptr, AD54X2_REG_R_CTRL);
	i = 0;	  
	while (i<AD54X2_DAISY_LENGTH){
		temp3 = p401_write_output[write_offset+AD54X2_DAISY_LENGTH-i-1];
		ad54x2_vals[i] = (temp3 & 0x7FFF) << 1;
		if (temp3 < 0)
			ad54x2_vals[i] = ~ad54x2_vals[i];
		i++;
	}
	ad54x2_writeDataAll(port, SPI_DIAG2, AD54X2_REG_DATA, ad54x2_vals);

	ad54x2_readDataAll(port, SPI_DIAG1, kps_4_x_rx_ptr, AD54X2_REG_R_CTRL);
	i = 0;	  
	while (i<AD54X2_DAISY_LENGTH){
		temp3 = p401_write_output[write_offset+AD54X2_DAISY_LENGTH-i-1+5];
		ad54x2_vals[i] = (temp3 & 0x7FFF) << 1;
		if (temp3 < 0)
			ad54x2_vals[i] = ~ad54x2_vals[i];
		i++;
	}
	ad54x2_writeDataAll(port, SPI_DIAG1, AD54X2_REG_DATA, ad54x2_vals);

	ad54x2_delayAll(port, SPI_DIAG12);


	if (mcp23S17_readByte(port, 0x00, MCP23S17_IOCON) != 0x08){
		mcp23S17_writeByte(port, 0x00, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu;
		mcp23S17_writeWord(port, 0x00, MCP23S17_IODIRA, 0x0000);	//wszystkie bity jako wyjscia
	}
	i = 6;
	if (p401_write_output[write_offset+i-1] < 0)
		temp2 = 0x01;
	else if (p401_write_output[write_offset+i-1] > 0)
		temp2 = 0x02;
	else
		temp2 = 0x03;
	mcp23S17_writeByte(port, 0x00, MCP23S17_GPIOB, temp2);	//kanal 6
	temp = 0x00;
	for (i=7;i<=10;i++){
		if (p401_write_output[write_offset+i-1] < 0)
			temp2 = 0x01;
		else if (p401_write_output[write_offset+i-1] > 0)
			temp2 = 0x02;
		else
			temp2 = 0x03;
		temp <<= 2;
		temp |= temp2;
	}
	mcp23S17_writeByte(port, 0x00, MCP23S17_GPIOA, temp);	//kanaly 7-10

	if (mcp23S17_readByte(port, 0x04, MCP23S17_IOCON) != 0x08){
		mcp23S17_writeByte(port, 0x04, MCP23S17_IOCON, 0x08);	//wlaczenie adresowania chipu;
		mcp23S17_writeWord(port, 0x04, MCP23S17_IODIRA, 0x0000);	//wszystkie bity jako wyjscia
	}

	i = 1;
	if (p401_write_output[write_offset+i-1] < 0)
		temp2 = 0x01;
	else if (p401_write_output[write_offset+i-1] > 0)
		temp2 = 0x02;
	else
		temp2 = 0x03;
	mcp23S17_writeByte(port, 0x04, MCP23S17_GPIOB, temp2);	//kanal 1
	temp = 0x00;
	for (i=2;i<=5;i++){
		if (p401_write_output[write_offset+i-1] < 0)
			temp2 = 0x01;
		else if (p401_write_output[write_offset+i-1] > 0)
			temp2 = 0x02;
		else
			temp2 = 0x03;
		temp <<= 2;
		temp |= temp2;
	}
	mcp23S17_writeByte(port, 0x04, MCP23S17_GPIOA, temp);	//kanaly 2-5
}

void KPS_5_2_init(UNSIGNED8 port){
	ad779x_init(port);
}

void KPS_5_2_pt100_loop(UNSIGNED8 port, INTEGER16* p401_an_in, UNSIGNED8 offset, UNSIGNED8 count, UNSIGNED8 sma){
	ad7794_loop(port, p401_an_in, offset, count, sma);
}

void KPS_5_2_mA_loop(UNSIGNED8 port, INTEGER16* p401_an_in, UNSIGNED8 offset, UNSIGNED8 count){
	ad7795_loop(port, p401_an_in, offset, count);
}

void KPS_5_2_namur(UNSIGNED8 port, BOOL_T* p401_read_state, UNSIGNED8 read_offset){
//    PRINTF("0x%02x 0x%02x 0x%02x\n",port,spi_namur_state(port),spi_namur_err(port));
	if (!spi_readPin(port, SPI_DIAG1))
		p401_read_state[read_offset] = 0x1;
	else
		p401_read_state[read_offset] = 0x0;
	if (!spi_readPin(port, SPI_DIAG2))
		p401_read_state[read_offset+1] = 0x1;
	else
		p401_read_state[read_offset+1] = 0x0;
}

