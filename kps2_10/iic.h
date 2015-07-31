#ifndef _IIC_H_
#define _IIC_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <cal_conf.h>      /* !!! first file to include for all CANopen */
#include <examples.h>
#include <dave\usic0.h>
#include <dave\usic1.h>
#include <dave\usic2.h>
#include <dave\usic3.h>
#include <dave\io.h>

#define LED_ROWS 0x06
#define LED_COLS 0x08

static const UNSIGNED16 ascii_all[LED_ROWS] = {0xff,0xff,0xff,0xff,0xff,0xff};
static const UNSIGNED16 ascii_K[LED_ROWS] = {9,5,3,3,5,9};
static const UNSIGNED16 ascii_P[LED_ROWS] = {7,9,9,7,1,1};
static const UNSIGNED16 ascii_S[LED_ROWS] = {6,9,2,4,9,6};
static const UNSIGNED16 ascii__[LED_ROWS] = {0,0,7,0,0,0};
static const UNSIGNED16 ascii_2[LED_ROWS] = {6,9,4,2,1,15};
static const UNSIGNED16 ascii_DOT[LED_ROWS] = {0,0,0,0,0,1};
static const UNSIGNED16 ascii_1[LED_ROWS] = {6,5,4,4,4,4};
static const UNSIGNED16 ascii_X[LED_ROWS] = {9,9,6,6,9,9};

INTEGER16 write_test(UNSIGNED8 a, UNSIGNED8 b);
INTEGER16 read_temp_iic(void);
UNSIGNED8 blink(UNSIGNED16 time, UNSIGNED8 hz);
UNSIGNED16 iic_send(UNSIGNED16 uwSlAdr, UNSIGNED8 nr_wr, UNSIGNED16 addr, UNSIGNED8 count, UNSIGNED16 *uwRecData);
UNSIGNED16 iic_writeWord(UNSIGNED16 uwSlAdr, UNSIGNED16 addr, UNSIGNED16 val);
void iic_led_loop();
void iic_led_setd(UNSIGNED8 slot, UNSIGNED8 type, UNSIGNED16 val);
void iic_led_seta(UNSIGNED8 slot, UNSIGNED8 type, INTEGER16 *vals, UNSIGNED16 offset, UNSIGNED16 count);

#endif  // ifndef _IIC_H_