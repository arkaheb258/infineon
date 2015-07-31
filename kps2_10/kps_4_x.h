#ifndef _KPS_4_X_H_
#define _KPS_4_X_H_

#include <spi.h>
#include <mcp23S17.h>
#include <ad54x2.h>
#include <ad779x.h>

UNSIGNED8 KPS_check(UNSIGNED8 port);

void KPS_4_1_init(UNSIGNED8 port);
void KPS_4_2_init(UNSIGNED8 port);
void KPS_5_1_init(UNSIGNED8 port);
void KPS_5_2_init(UNSIGNED8 port);

void KPS_4_1_loop(UNSIGNED8 port, INTEGER16* p401_write_output, UNSIGNED8 write_offset, INTEGER16* p401_read_output, UNSIGNED8 read_offset);
void KPS_4_2_loop(UNSIGNED8 port, INTEGER16* p401_write_output, UNSIGNED8 write_offset);
void KPS_5_1_loop(UNSIGNED8 port, INTEGER16* p401_read_output, UNSIGNED8 read_offset);
void KPS_5_2_pt100_loop(UNSIGNED8 port, INTEGER16* p401_an_in, UNSIGNED8 offset, UNSIGNED8 count, UNSIGNED8 sma);
void KPS_5_2_mA_loop(UNSIGNED8 port, INTEGER16* p401_an_in, UNSIGNED8 offset, UNSIGNED8 count);
void KPS_5_2_namur(UNSIGNED8 port, BOOL_T* p401_read_state, UNSIGNED8 read_offset);

#endif	//_KPS_4_X_H_