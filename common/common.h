#ifndef _COMMON_H_
#define _COMMON_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define DEF_HW_PART
#include <cal_conf.h>      /* !!! first file to include for all CANopen */

#define COMMON_SMA_CH_COUNT	30
#define COMMON_SMA_MAX			6

UNSIGNED16 sma_16(UNSIGNED8 sma_ch, UNSIGNED16 pom_R, UNSIGNED8 sma);
UNSIGNED16 string2int(unsigned char *buf, int len);
UNSIGNED8 blink(uword time, UNSIGNED8 hz);
UNSIGNED8 err_code(UNSIGNED16 time, UNSIGNED8 code);
UNSIGNED8 byte_arr_2_word_arr(UNSIGNED16 *out_arr, UNSIGNED8 *in_arr, UNSIGNED8 len);


#endif  // ifndef _COMMON_H_
