#ifndef _AD779X_H_
#define _AD779X_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEF_HW_PART
#include <cal_conf.h>      /* !!! first file to include for all CANopen */
#include <examples.h>
#include <dave\io.h>
#include <spi.h>

#define AD7794_CHANELLS	0x06
#define AD7795_CHANELLS	0x06

#define AD779X_REG_STATUS 0x00	//8bit
#define AD779X_REG_CONF 0x10	//16bit
#define AD779X_REG_MODE 0x08	//16bit
#define AD779X_REG_DATA 0x18	//16/24bit
#define AD779X_REG_IO	0x28	//8bit

#define AD779X_AD7794	0x88
#define AD779X_AD7795 	0x80
#define AD779X_READ 	0x40
#define AD779X_WRITE 	0x00

void ad779x_init(UNSIGNED8 port);
void ad7794_loop(UNSIGNED8 port, INTEGER16* p401_an_in, UNSIGNED8 offset, UNSIGNED8 count, UNSIGNED8 sma);
void ad7795_loop(UNSIGNED8 port, INTEGER16* p401_an_in, UNSIGNED8 offset, UNSIGNED8 count);
//void ad779x_cykl_check(UNSIGNED8 port, INTEGER16* p401_an_in, UNSIGNED8 offset, UNSIGNED8 count);

#endif  // ifndef _AD779X_H_