#ifndef _SPI_H_
#define _SPI_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEF_HW_PART
#include <cal_conf.h>      /* !!! first file to include for all CANopen */
#include <examples.h>
#include <dave\usic0.h>
#include <dave\usic1.h>
#include <dave\usic2.h>
#include <dave\usic3.h>
#include <dave\io.h>

void spi_setDiagOutput(UNSIGNED8 port);
void spi_setDiagInput(UNSIGNED8 port);
UNSIGNED8 spi_readPin(UNSIGNED8 port, UNSIGNED8 pin);
void spi_writePin(UNSIGNED8 port, UNSIGNED8 pin, UNSIGNED8 state);
void spi_reset(UNSIGNED8 port);
void spi_send(UNSIGNED8 port, UNSIGNED8 cs, UNSIGNED16 *uwTxData, UNSIGNED16 *uwRxData, UNSIGNED16 uwDataSize);

#define USIC0_CH0 0x00
#define USIC0_CH1 0x01
#define USIC1_CH0 0x10
#define USIC1_CH1 0x11
#define USIC2_CH0 0x20
#define USIC2_CH1 0x21
#define USIC3_CH0 0x30
#define USIC3_CH1 0x31

#define U0C0 USIC0_CH0
#define U0C1 USIC0_CH1
#define U1C0 USIC1_CH0
#define U1C1 USIC1_CH1
#define U2C0 USIC2_CH0
#define U2C1 USIC2_CH1
#define U3C0 USIC3_CH0
#define U3C1 USIC3_CH1

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
#define SPI_DIAG1	0xD1
#define SPI_DIAG2	0xD2
#define SPI_DIAG12	0xD3
#define SPI_MISO	0x10

#define SPI_FIFO_SIZE	16

#endif  // ifndef _SPI_H_