#ifndef _ARKA_H_
#define _ARKA_H_
#include "crc.h"

#define C_PS2_MOUSE 	'M'
#define C_PS2_KEY  		'K'
#define C_PS2_PRESS  	'+'
#define C_PS2_RELEASE	'-'
#define C_IPS_PUSH  	'='
#define C_IPS_MAP  		'P'
#define C_IPS_USB  		'U'
#define C_UPT  			'A'
#define C_IPS  			'I'
#define C_DIODE  		'D'
#define C_CONTROL  		'C'	//0x53
#define C_STATUS  		'S'	//0x43

#ifndef DEBUG_EN
  #define DEBUG_EN 0
#endif
#ifndef BUFF_SIZE
  #define BUFF_SIZE 16
#endif
#ifndef FREQ
  #define FREQ 10
#endif

unsigned int temp_crc;

unsigned char prepare_serial(unsigned char * buff_tx, unsigned char * data, unsigned char len) {
  unsigned char i = 0;
  buff_tx[0] = 0x0D;
  buff_tx[1] = 0x0A;
  buff_tx[2] = len;
  for (i=0;i<len;i++){
    buff_tx[i+3] = data[i];
  }
  temp_crc = crc16(buff_tx, len+3);
  buff_tx[len+3] = (temp_crc >> 8) & 0x00FF;
  buff_tx[len+4] = temp_crc & 0x00FF;
  return len+5;
}

unsigned char get_serial(unsigned char inByte, unsigned char * in_iter, unsigned char * buff_rx){
  unsigned char iter;
  iter = *in_iter;
  if (iter < BUFF_SIZE) { 
    buff_rx[iter] = inByte;
  } else {
    *in_iter = 0;
    return 0; 
  }
#if DEBUG_EN == 1
  Serial.print(inByte, HEX);
  Serial.print(" - ");
  Serial.println(iter, DEC);
#endif
  switch (iter){
  case 0:
    iter += 1;
    if (inByte != 13) iter = 0;
    break;
  case 1:
    iter += 1;
    if (inByte != 10) iter = 0;
    break;
  default:
    iter += 1;
    if (buff_rx[2] > BUFF_SIZE - 5) { 
      *in_iter = 0;
      return 0; 
    }
    if (iter == buff_rx[2] + 5) { 
      temp_crc = crc16(buff_rx, iter-2);
      if ((buff_rx[iter-2] == ((temp_crc >> 8) & 0x00FF)) && (inByte == (temp_crc & 0x00FF))){
        #if DEBUG_EN == 1
          Serial.println(temp_crc, HEX);
          Serial.println("CRC OK");
        #endif
        *in_iter = iter;
        for (iter=0;iter<*in_iter-5;iter+=1){
          buff_rx[iter] = buff_rx[iter+3];
        }
        iter = *in_iter;
        *in_iter = 0;
        return iter-5;
      } else {
        *in_iter = 0;
        return 0;
      }
    }
    break;
  }
  *in_iter = iter;
  return 0;
}

//odwrocenie kolejnosci bitow w slowie
unsigned char reverse_bits(unsigned char val){
  unsigned char temp;
  unsigned char i;
  temp = 0;
  for (i = 0; i < 4; i += 1){
    temp |= (val & (1<<i)) << (7-2*i) ;
  }
  for (i = 4; i < 8; i += 1){
    temp |= (val & (1<<i)) >> (2*i-7) ;
  }
  return temp;
}

#endif  // ifndef _CRC_H_
