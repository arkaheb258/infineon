#ifndef _RS_H_
#define _RS_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEF_HW_PART
#include <cal_conf.h>      /* !!! first file to include for all CANopen */
#include <examples.h>
#include <dave\usic0.h>
#include <dave\usic1.h>
#include <dave\usic2.h>
#if HW_KPS == 1
	#include <dave\usic3.h>
#endif

#include <serial_def.h>

//void rs_send(UNSIGNED8 port,UNSIGNED16 *uwTxData, UNSIGNED16 *uwRxData, UNSIGNED16 uwDataSize);
//void rs_get(UNSIGNED8 port, UNSIGNED16 *uwTxData, UNSIGNED16 *uwRxData, UNSIGNED16 uwDataSize);

#define RS_FLUSH_RX(uc)	uc##_ASC_vFlushRxFIFO()
#define RS_FLUSH_TX(uc)	uc##_ASC_vFlushTxFIFO()
#define RS_FILLING_RX(uc)	uc##_ASC_ubGetRxFIFOFillingLevel()
#define RS_FILLING_TX(uc)	uc##_ASC_ubGetTxFIFOFillingLevel()
#define RS_FILL_TX(uc,a,b)	uc##_ASC_vFillTxFIFO(a,b)
#define RS_GET_RX(uc)	uc##_ASC_uwGetRxFIFOData()

#define RS_FIFO_SIZE	8

#endif  // ifndef _SPI_H_