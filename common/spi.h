#ifndef _SPI_H_
#define _SPI_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEF_HW_PART
#include <cal_conf.h>      /* !!! first file to include for all CANopen */
#include <dave\io.h>
#include <examples.h>
#include <dave\usic0.h>
#include <dave\usic1.h>
#include <dave\usic2.h>

#if HW_KPS == 1
	#include <dave\usic3.h>
#endif

#include <serial_def.h>

void spi_setDiagOutput(UNSIGNED8 port);
void spi_setDiagInput(UNSIGNED8 port);
UNSIGNED8 spi_readPin(UNSIGNED8 port, UNSIGNED8 pin);
void spi_writePin(UNSIGNED8 port, UNSIGNED8 pin, UNSIGNED8 state);
void spi_reset(UNSIGNED8 port);
void spi_send(UNSIGNED8 port, UNSIGNED8 cs, UNSIGNED16 *uwTxData, UNSIGNED16 *uwRxData, UNSIGNED16 uwDataSize);


#define SPI_FLUSH_RX(uc)	uc##_SSC_vFlushRxFIFO()
#define SPI_FLUSH_TX(uc)	uc##_SSC_vFlushTxFIFO()
#define SPI_FILLING_RX(uc)	uc##_SSC_ubGetRxFIFOFillingLevel()
#define SPI_FILL_TX(uc,a,b)	uc##_SSC_vFillTxFIFO(a,b)
#define SPI_GET_RX(uc)	uc##_SSC_uwGetRxFIFOData()

#define SPI_NAMUR_STATE	SPI_DIAG1
#define SPI_NAMUR_ERROR	SPI_DIAG2

#define SPI_RESET 	0xEE
#define SPI_NO_CS	0x00
#define SPI_CS0		0xC0
#define SPI_CS1		0xC1
#define SPI_CS2		0xC2
#define SPI_CS3		0xC3
#define SPI_DIAG1	0xD1
#define SPI_DIAG2	0xD2
#define SPI_DIAG12	0xD3
#define SPI_MISO	0x10

#define SPI_FIFO_SIZE	16

#endif  // ifndef _SPI_H_