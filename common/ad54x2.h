#ifndef _AD54X2_H_
#define _AD54X2_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEF_HW_PART
#include <cal_conf.h>      /* !!! first file to include for all CANopen */
#include <examples.h>
#include <dave\io.h>
#include "spi.h"

//#define AD54X2_REG_STATUS  	0x00	//16bit
#define AD54X2_REG_NOP	   	0x00
#define AD54X2_REG_DATA	   	0x01	//16bit
#define AD54X2_REG_READBACK	0x02	//16bit
#define AD54X2_REG_CONTROL	0x55	//16bit
#define AD54X2_REG_RESET   	0x56	//16bit

#define AD54X2_REG_R_STAT	0x00	//16bit
#define AD54X2_REG_R_DATA	0x01	//16bit
#define AD54X2_REG_R_CTRL  	0x02	//16bit

#if HW_KPS == 1
	#define AD54X2_DAISY_LENGTH	5
#elif HW_UPT == 1
	#define AD54X2_DAISY_LENGTH	1
#endif
//#define AD54X2_DAISY_LENGTH	10
#define AD54X2_ADDR			

// extern UNSIGNED16 ad54x2_vals[];
// extern UNSIGNED16 ad54x2_nop_ptr[];
void ad54x2_resetAll(UNSIGNED8 port, UNSIGNED8 cs);
void ad54x2_delayAll(UNSIGNED8 port, UNSIGNED8 cs);
void ad54x2_readDataAll(UNSIGNED8 port, UNSIGNED8 cs, UNSIGNED16 *val, UNSIGNED8 r_reg);
void ad54x2_setConfigOne(UNSIGNED8 port, UNSIGNED8 cs, UNSIGNED8 reg, UNSIGNED16 val, UNSIGNED8 dc_poz);
void ad54x2_writeDataAll(UNSIGNED8 port, UNSIGNED8 cs, UNSIGNED8 reg, UNSIGNED16 *val);
UNSIGNED16 ad54x2_readRegOne(UNSIGNED8 port, UNSIGNED8 cs, UNSIGNED8 r_reg, UNSIGNED8 dc_poz);
UNSIGNED16 ad7814_read(UNSIGNED8 port, UNSIGNED8 cs);
#endif  // ifndef _AD54X2_H_