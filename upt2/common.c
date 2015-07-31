#include "common.h"

UNSIGNED32 ulSMA[COMMON_SMA_CH_COUNT][1 << COMMON_SMA_MAX];
UNSIGNED8 ulSMA_iter[COMMON_SMA_CH_COUNT];

UNSIGNED16 sma_16(UNSIGNED8 sma_ch, UNSIGNED16 pom_R, UNSIGNED8 sma){
	UNSIGNED32 sma_sumH = 0;
	UNSIGNED32 sma_sumL = 0;
	UNSIGNED32 sma_temp = 0;
	UNSIGNED8 sma_i = 0;
	UNSIGNED8 sma_count = 0;
	if (sma > 0) {
		if (sma > 6) {
			sma = 6;
		}
		ulSMA[sma_ch][ulSMA_iter[sma_ch]] = pom_R;

		ulSMA_iter[sma_ch] = ulSMA_iter[sma_ch] + 1;
		sma_count = 1 << sma;
		if (ulSMA_iter[sma_ch] >= sma_count) {
			ulSMA_iter[sma_ch] = 0;
		}
		sma_sumH = 0;
		sma_sumL = 0;
		for (sma_i=0;sma_i<sma_count;sma_i++){
			sma_temp = ulSMA[sma_ch][sma_i];
			sma_sumH += (sma_temp >> 16);
			sma_sumL += (sma_temp & 0xFFFF);
		}
		pom_R = (sma_sumL >> sma) + (sma_sumH << (16-sma));
	}
	return pom_R;
}

UNSIGNED16 string2int(unsigned char *buf, int len){
	int counter;
	unsigned short out = 0;
	for( counter = 0; counter < len; counter++) {
		out *= 10;
		if (buf[counter] > 0x30 && buf[counter] < 0x3A) 
			out += (buf[counter] - 0x30);	
	}
	return out;
}

UNSIGNED8 blink(uword time, UNSIGNED8 hz){
	UNSIGNED16 temp1;
	UNSIGNED16 temp2;
	if (hz == 0) return 0;
	if (hz == 0xff) return 1;
	temp1 = 3277/hz;
	temp2 = 6554/hz;
	if ((time%(temp2)) > temp1)
		return 0;
	else
		return 1;
}

UNSIGNED8 err_code(UNSIGNED16 time, UNSIGNED8 code){
	if (code == 0) return 0;
	if (time < 0x2000)	//cykl 10s
		return blink(time, 5);
//	else if (time < 0x3000)
//		return 0;
	else if (time > (0x2000 + 0x2000 * code))
		return 0;
	else if (((time - 0x2000) / 0x1000) % 2)
		return 1;	
	else
		return 0;
}

UNSIGNED8 byte_arr_2_word_arr(UNSIGNED16 *out_arr, UNSIGNED8 *in_arr, UNSIGNED8 len){
	UNSIGNED8 i;
	for (i=0;i<len;i+=1){
		out_arr[i] = in_arr[i];
	}
	return i;
}
