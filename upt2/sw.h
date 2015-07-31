#ifndef _SW_H_
#define _SW_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEF_HW_PART
#include <cal_conf.h>      /* !!! first file to include for all CANopen */
#include <dave\io.h>

void sw_init(void);
void sw_loop(void);

void safe_state(void);

#endif  // ifndef _SW_H_
