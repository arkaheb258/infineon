#include <mcp23S17.h>

UNSIGNED16 mcp23S17_tx_ptr[13];
UNSIGNED16 mcp23S17_rx_ptr[13];

//odczyt
void mcp23S17_readData(UNSIGNED8 port, UNSIGNED16 *uwRxData, UNSIGNED8 d_addr, UNSIGNED8 r_addr, int count){
	UNSIGNED16 uwDataSize = 0;
	mcp23S17_tx_ptr[uwDataSize++] = MCP23S17_ADDR | ((d_addr & 0x07) << 1) | MCP23S17_READ;
	mcp23S17_tx_ptr[uwDataSize++] = r_addr;
	while (count-- > 0)
		mcp23S17_tx_ptr[uwDataSize++] = 0x00;
	spi_send(port, SPI_CS0, mcp23S17_tx_ptr, uwRxData, uwDataSize);
}

UNSIGNED8 mcp23S17_readByte(UNSIGNED8 port, UNSIGNED8 d_addr, UNSIGNED8 r_addr){
	UNSIGNED16 uwDataSize = 0;
	mcp23S17_tx_ptr[uwDataSize++] = MCP23S17_ADDR | ((d_addr & 0x07) << 1) | MCP23S17_READ;
	mcp23S17_tx_ptr[uwDataSize++] = r_addr;
	mcp23S17_tx_ptr[uwDataSize++] = 0x00;
	spi_send(port, SPI_CS0, mcp23S17_tx_ptr, mcp23S17_rx_ptr, uwDataSize);
	return mcp23S17_rx_ptr[2] & 0xFF;
}

//zapis
void mcp23S17_writeByte(UNSIGNED8 port, UNSIGNED8 d_addr, UNSIGNED8 r_addr, UNSIGNED8 val){
	UNSIGNED16 uwDataSize = 0;
	mcp23S17_tx_ptr[uwDataSize++] = MCP23S17_ADDR | ((d_addr & 0x07) << 1) | MCP23S17_WRITE;
	mcp23S17_tx_ptr[uwDataSize++] = r_addr;
	mcp23S17_tx_ptr[uwDataSize++] = val;
//	while (count-- > 0)
//		mcp23S17_tx_ptr[uwDataSize++] = 0x00;
	spi_send(port, SPI_CS0, mcp23S17_tx_ptr, mcp23S17_rx_ptr, uwDataSize);
}

//zapis
void mcp23S17_writeWord(UNSIGNED8 port, UNSIGNED8 d_addr, UNSIGNED8 r_addr, UNSIGNED16 val){
	UNSIGNED16 uwDataSize = 0;
	mcp23S17_tx_ptr[uwDataSize++] = MCP23S17_ADDR | ((d_addr & 0x07) << 1) | MCP23S17_WRITE;
	mcp23S17_tx_ptr[uwDataSize++] = r_addr;
	mcp23S17_tx_ptr[uwDataSize++] = (val >> 8) & 0xFF;
	mcp23S17_tx_ptr[uwDataSize++] = val & 0xFF;
//	while (count-- > 0)
//		mcp23S17_tx_ptr[uwDataSize++] = 0x00;
	spi_send(port, SPI_CS0, mcp23S17_tx_ptr, mcp23S17_rx_ptr, uwDataSize);
}

void mcp23S17_init(UNSIGNED8 port, UNSIGNED8 addr){
	spi_writePin(port, SPI_CS0, 1); //IO_vSetPin(SPI01_CS_IO_P0_3);

	mcp23S17_writeWord(port, 0, MCP23S17_IOCON, 0x0808);	//wlaczenie adresowania chipu
	mcp23S17_writeWord(port, 0x04, MCP23S17_IOCON, 0x0808);	//wlaczenie adresowania chipu
	mcp23S17_writeWord(port, 0, MCP23S17_IOCON, 0x0808);	//wlaczenie adresowania chipu
	mcp23S17_writeWord(port, 0, MCP23S17_IODIRA, ~0xFF03);	//10 bitow jako wyjscia
	mcp23S17_writeWord(port, 0x04, MCP23S17_IODIRA, ~0xFF03);	//10 bitow jako wyjscia
	mcp23S17_writeWord(port, 0, MCP23S17_IPOLA, 0xFF03);		//10 bitow odwrocenie polaryzacji
	mcp23S17_writeWord(port, 0x04, MCP23S17_IPOLA, 0xFF03);		//10 bitow odwrocenie polaryzacji

//	mcp23S17_readData(port, mcp23S17_rx_ptr, 0x04, MCP23S17_IOCON, 2);
//	mcp23S17_readData(port, mcp23S17_rx_ptr, 0x00, MCP23S17_IOCON, 2);

//	mcp23S17_readData(port, mcp23S17_rx_ptr, 0x04, MCP23S17_IODIRA, 2);
//	mcp23S17_readData(port, mcp23S17_rx_ptr, 0x00, MCP23S17_IODIRA, 2);

//	mcp23S17_readData(port, mcp23S17_rx_ptr, 0x04, MCP23S17_IPOLA, 2);
//	mcp23S17_readData(port, mcp23S17_rx_ptr, 0x00, MCP23S17_IPOLA, 2);
}


