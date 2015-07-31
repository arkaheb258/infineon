#ifndef _MCP23S17_H_
#define _MCP23S17_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEF_HW_PART
#include <cal_conf.h>      /* !!! first file to include for all CANopen */
#include <examples.h>
#include <dave\io.h>
#include <spi.h>

#define MCP23S17_ADDR 0x40
#define MCP23S17_READ 0x01
#define MCP23S17_WRITE 0x00

#define MCP23S17_IODIRA		0x00
#define MCP23S17_IODIRB		0x01
#define MCP23S17_IPOLA		0x02
#define MCP23S17_IPOLB		0x03
#define MCP23S17_GPINTENA	0x04
#define MCP23S17_GPINTENB	0x05
#define MCP23S17_DEFVALA	0x06
#define MCP23S17_DEFVALB	0x07
#define MCP23S17_INTCONA	0x08
#define MCP23S17_INTCONB	0x09
#define MCP23S17_IOCON		0x0A
//#define MCP23S17_IOCON		0x0B
#define MCP23S17_GPPUA		0x0C
#define MCP23S17_GPPUB		0x0D
#define MCP23S17_INTFA		0x0E
#define MCP23S17_INTFB		0x1F
#define MCP23S17_INTCAPA	0x10
#define MCP23S17_INTCAPB 	0x11
#define MCP23S17_GPIOA		0x12
#define MCP23S17_GPIOB		0x13
#define MCP23S17_OLATA		0x14
#define MCP23S17_OLATB		0x15

void mcp23S17_readData(UNSIGNED8 port, UNSIGNED16 *uwRxData, UNSIGNED8 d_addr, UNSIGNED8 r_addr, int count);
UNSIGNED8 mcp23S17_readByte(UNSIGNED8 port, UNSIGNED8 d_addr, UNSIGNED8 r_addr);
void mcp23S17_writeByte(UNSIGNED8 port, UNSIGNED8 d_addr, UNSIGNED8 r_addr, UNSIGNED8 val);
void mcp23S17_writeWord(UNSIGNED8 port, UNSIGNED8 d_addr, UNSIGNED8 r_addr, UNSIGNED16 val);
void mcp23S17_init(UNSIGNED8 port, UNSIGNED8 addr);

#endif  // ifndef _MCP23S17_H_