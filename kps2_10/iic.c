#include <iic.h>
#include <dave\rtc.h>

UNSIGNED16 uwFIFO_RecData[14]; 
UNSIGNED16 uwFIFO_SendData[16]; 
UNSIGNED8 ascii_step = 0;
UNSIGNED8 ascii_step2 = 0;

//ulong iic_last_time = 0;
UNSIGNED16 iic_led_state[LED_ROWS];
UNSIGNED16 ascii_test[LED_ROWS];
UNSIGNED8 iic_led_matrix[LED_ROWS];
UNSIGNED8 iic_led_row = 0;
UNSIGNED8 iic_led_test = 1;

extern UNSIGNED16 main_type_out;

UNSIGNED8 blink(UNSIGNED16 time, UNSIGNED8 hz){
	UNSIGNED16 temp1;
	UNSIGNED16 temp2;
	temp1 = 3277/hz;
	temp2 = 6554/hz;
	if ((time%(temp2)) > temp1)
		return 0;
	else
		return 1;
}

INTEGER16 read_temp_iic(void)
{ 
	INTEGER16 temp = 0;
	UNSIGNED16 uwData[2];
	iic_send(0x48, 0, 0, 2, uwData);
	temp = (uwData[0] & 0x00FF) << 1;
	temp |= ((uwData[1] & 0x0080) >> 7);
	temp *= 5;
	return temp;
}
/*
INTEGER16 read_temp_iic(void)
{ 
	UNSIGNED32 time = 0;
//	UNSIGNED8 err = 0;
	INTEGER16 temp = 0;
	UNSIGNED8 ubReadData = 0; 
	UNSIGNED8 ubFIFOLevel = 3; 
	UNSIGNED16 uwSlAdr = 0x48; // Address A0 - A2 = 0 
	uwSlAdr = U2C0_IIC_vSlaveAddrFormat(uwSlAdr);

	uwFIFO_SendData[0]= (((U2C0TDF_MStart << 8) & 0x0700) | ((uwSlAdr + U2C0IIC_READ)& 0x00FF)); // Read mode 
	uwFIFO_SendData[1]= ((U2C0TDF_MRxAck0 << 8) & 0x0700); // TDF_Ack0 
	uwFIFO_SendData[2]= ((U2C0TDF_MRxAck1 << 8) & 0x0700); // TDF_Ack1
	uwFIFO_SendData[3]= ((U2C0TDF_MStop << 8) & 0x0700); // TDF_Stop 

	while(!U2C0_IIC_ubIsTxFIFOempty()); // Wait for Tx FIFO Empty 
	while(U2C0_IIC_ubIsTxFIFObusy()); // Wait for TxFIFO Not Busy 
//    PRINTF("tfl1 0x%02x\n",(int)U2C0_IIC_ubGetTxFIFOFillingLevel());
	U2C0_IIC_vFlushTxFIFO(); // clear the transmit FIFO 
	U2C0_IIC_vFillTxFIFO(uwFIFO_SendData , 4); 
//    PRINTF("sr %d\n",U2C0_IIC_uwGetStatus());
//	U2C0_IIC_uwGetStatus();
//	time = RTC_ulGetTime();
	myRTC_elapsed(1);
	while (U2C0_IIC_ubGetRxFIFOFillingLevel() != 2){ // Wait for Two Data's 
//		if (RTC_ulGetTime() - time > 100){
		if (myRTC_elapsed(0) * RTC_MUL > 50){
			U2C0_IIC_vFlushTxFIFO(); // clear the transmit FIFO 
			return 0;
		}
	}
//    PRINTF("tfl3 0x%02x\n",(int)U2C0_IIC_ubGetTxFIFOFillingLevel());
	myRTC_elapsed(1);
	while (ubReadData != ubFIFOLevel) // Read Two Data's 
	{ 
		while (U2C0_IIC_ubIsRxFIFObusy()); // Wait for RxFIFO Not busy 
		uwFIFO_RecData[ubReadData] = U2C0_IIC_uwGetRxFIFOData(); // Get the Received Data 
		ubReadData++; 
//		if (RTC_ulGetTime() - time > 100){
//		if (myRTC_elapsed(0) > 100){
		if (myRTC_elapsed(0) * RTC_MUL > 50){
			U2C0_IIC_vFlushRxFIFO(); // clear the transmit FIFO 
			return 0;
		}
	} 
	U2C0_IIC_vFlushRxFIFO(); 
	while(!U2C0_IIC_ubIsRxFIFOempty()); // Wait for Tx FIFO Empty 
	temp = (uwFIFO_RecData[0] & 0x00FF) << 1;
	temp |= ((uwFIFO_RecData[1] & 0x0080) >> 7);
	temp *= 5;
	return temp;
} 
*/ 

UNSIGNED16 iic_send(UNSIGNED16 uwSlAdr, UNSIGNED8 nr_wr, UNSIGNED16 addr, UNSIGNED8 count, UNSIGNED16 *uwRecData) {
	UNSIGNED32 time = 0;
	UNSIGNED8 i = 0;
	UNSIGNED8 ubReadData = 0; 
	uwSlAdr = U2C0_IIC_vSlaveAddrFormat(uwSlAdr);

	uwFIFO_SendData[0]= (((U2C0TDF_MStart << 8) & 0x0700) | ((uwSlAdr + U2C0IIC_WRITE)& 0x00FF)); // Write mode 
	uwFIFO_SendData[1]= ((U2C0TDF_MTxData << 8) & 0x0700) | (addr & 0x00FF); // TDF_Ack0 

	if (nr_wr) {
		uwFIFO_SendData[2]= (((U2C0TDF_MRStart << 8) & 0x0700) | ((uwSlAdr + U2C0IIC_WRITE)& 0x00FF)); // Write mode 
	} else {
		uwFIFO_SendData[2]= (((U2C0TDF_MRStart << 8) & 0x0700) | ((uwSlAdr + U2C0IIC_READ)& 0x00FF)); // Read mode 
	}

	if (count < 1) { 
		return 0;
	} 
	if (nr_wr) {
		for (i=0; i<count; i++){
			uwFIFO_SendData[i+3] = ((U2C0TDF_MTxData << 8) & 0x0700) | ((uwRecData[i]) & 0x00FF); // TDF_Ack0 
		}
	} else {
		for (i=0; i<count-1; i++){
			uwFIFO_SendData[i+3] = ((U2C0TDF_MRxAck0 << 8) & 0x0700);// | ((uwRecData[i]) & 0x00FF); // TDF_Ack0 
		}
		uwFIFO_SendData[count+2] = ((U2C0TDF_MRxAck1 << 8) & 0x0700);// | ((uwRecData[count-1]) & 0x00FF); // TDF_Ack0 
	}
	uwFIFO_SendData[count+3]= ((U2C0TDF_MStop << 8) & 0x0700); // TDF_Stop 

	while(!U2C0_IIC_ubIsTxFIFOempty()); // Wait for Tx FIFO Empty 
	while(U2C0_IIC_ubIsTxFIFObusy()); // Wait for TxFIFO Not Busy 
	U2C0_IIC_vFlushTxFIFO(); // clear the transmit FIFO 
	U2C0_IIC_vFillTxFIFO(uwFIFO_SendData , count+4); 
	while (U2C0_IIC_ubIsTxFIFObusy()); // Wait for TxFIFO Not busy 
//	time = RTC_ulGetTime();
	myRTC_elapsed(1);
	while (U2C0_IIC_ubGetTxFIFOFillingLevel()){
//		if (RTC_ulGetTime() - time > 500){
//		if (myRTC_elapsed(0) > 100){
		if (myRTC_elapsed(0) * RTC_MUL > 50){
			U2C0_IIC_vFlushTxFIFO(); // clear the transmit FIFO 
//PRINTF("return a\n");
			return 0;
		}
	}; // Wait for Data to send 
	if (!nr_wr) {
//		time = RTC_ulGetTime();
		myRTC_elapsed(1);
		while (U2C0_IIC_ubGetRxFIFOFillingLevel() != count){ // Wait for Two Data's 
//			PRINTF("st %d\n",U2C0_IIC_uwGetStatus());
//			PRINTF("rfl %d\n",U2C0_IIC_ubGetRxFIFOFillingLevel());
//			if (RTC_ulGetTime() - time > 100){
//			if (myRTC_elapsed(0) > 100){
			if (myRTC_elapsed(0) * RTC_MUL > 50){
				U2C0_IIC_vFlushTxFIFO(); // clear the transmit FIFO 
//				PRINTF("return b\n");
				return 0;
			}
		}
//		time = RTC_ulGetTime();
		myRTC_elapsed(1);
		while (ubReadData != count)
		{ 
			while (U2C0_IIC_ubIsRxFIFObusy()); // Wait for RxFIFO Not busy 
		 	uwRecData[ubReadData] = U2C0_IIC_uwGetRxFIFOData(); // Get the Received Data 
		 	ubReadData++; 
//			if (RTC_ulGetTime() - time > 100){
//			if (myRTC_elapsed(0) > 100){
			if (myRTC_elapsed(0) * RTC_MUL > 50){
				U2C0_IIC_vFlushRxFIFO(); // clear the transmit FIFO 
//				PRINTF("return c\n");
				return 0;
			}
		} 
	}
//	U2C0_IIC_vFlushTxFIFO(); // clear the transmit FIFO 
	U2C0_IIC_vFlushRxFIFO(); 
	while(!U2C0_IIC_ubIsRxFIFOempty()); // Wait for Rx FIFO Empty 
	return 0;
}

UNSIGNED16 iic_writeWord(UNSIGNED16 uwSlAdr, UNSIGNED16 addr, UNSIGNED16 val) {
	UNSIGNED16 uwData[2];
	uwData[0] = (val >> 8) & 0x00FF;
	uwData[1] = val & 0x00FF;
	iic_send(uwSlAdr, 1, addr, 2, uwData);
}

void iic_led_blink(UNSIGNED32 time, UNSIGNED16 *led_state){
	UNSIGNED8 led_5hz = 0;
	UNSIGNED8 led_1hz = 0;
	UNSIGNED8 i = 0, j = 0;
	UNSIGNED8 temp0 = 0, temp1 = 0, temp2 = 0;
	if (blink(time, 5))
		led_5hz = 0xFF;
	if (blink(time, 1))
		led_1hz = 0xFF;
	for (i=0; i<LED_ROWS; i++){
		iic_led_matrix[i] = 0;
		for (j=0; j<LED_COLS; j++){
			temp0 = 1 << j;
			temp1 = ~led_state[i] & temp0;			//stan
			temp2 = (led_state[i] >> 8) & temp0;	//czy ma migac
			if (temp2){
				if (temp1) {
					iic_led_matrix[i] |= temp0 & led_1hz;
				} else {
					iic_led_matrix[i] |= temp0 & led_5hz;
				}
			} else {
				iic_led_matrix[i] |= temp1;
			}
		}		
	}
}

void iic_led_setd(UNSIGNED8 slot, UNSIGNED8 type, UNSIGNED16 val){
	UNSIGNED16 temp = 0;
	switch (type){
	case 0x41:
		iic_led_state[slot*2] = val & 0x003F;
		iic_led_state[slot*2+1] = (val >> 6) & 0x003F;
		break;
	case 0x51:	//DIN
		iic_led_state[slot*2] = val & 0x00FF;
		iic_led_state[slot*2+1] = (val >> 8) & 0x00FF;
		break;
	case 0x52D:	//NAMUR
		iic_led_state[slot*2] &= ~(0x303 << 4);
		iic_led_state[slot*2+1] &= ~(0x303 << 4);
		temp = (val >> (slot*4)) & 0x000F;
		iic_led_state[slot*2] |= (temp & 0x0002) << 11;		//stan
		iic_led_state[slot*2] |= (temp & 0x0001) << 4;		//diagn
		iic_led_state[slot*2+1] |= (temp & 0x0008) << 9;	//stan
		iic_led_state[slot*2+1] |= (temp & 0x0004) << 2;	//diagn
		break;
	default:
		break;
	}
}

void iic_led_seta(UNSIGNED8 slot, UNSIGNED8 type, INTEGER16 *vals, UNSIGNED16 offset, UNSIGNED16 count){
	UNSIGNED16 temp = 0;
	UNSIGNED8 i =0;
	switch (type){
	case 0x42:
		iic_led_state[slot*2] = 0;
		iic_led_state[slot*2+1] = 0;
		for (i=0; i<5; i++) {
			if (vals[offset+i] > 0) {
				iic_led_state[slot*2] |= 0x0001 << i;
			} else if (vals[offset+i] < 0) {
				iic_led_state[slot*2] |= 0x0101 << i;
			}
			if (vals[offset+5+i] > 0) {
				iic_led_state[slot*2+1] |= 0x0001 << i;
			} else if (vals[offset+5+i] < 0) {
				iic_led_state[slot*2+1] |= 0x0101 << i;
			}
		}
		break;
	case 0x52A: //AIN mA
		iic_led_state[slot*2] &= ~(0x707);
		iic_led_state[slot*2+1] &= ~(0x707);
		if (vals[offset+5] > 0x0003) { iic_led_state[slot*2] |= 0x0001; }
		if (vals[offset+1] > 0x0003) { iic_led_state[slot*2] |= 0x0001 << 1; }
		if (vals[offset+4] > 0x0003) { iic_led_state[slot*2] |= 0x0001 << 2; }
		if (vals[offset+0] > 0x0003) { iic_led_state[slot*2+1] |= 0x0001; }
		if (vals[offset+2] > 0x0003) { iic_led_state[slot*2+1] |= 0x0001 << 1; }
		if (vals[offset+3] > 0x0003) { iic_led_state[slot*2+1] |= 0x0001 << 2; }
		iic_led_state[slot*2+1] |= (vals[offset+0] & 0x0001) << 8;
		iic_led_state[slot*2] |= (vals[offset+1] & 0x0001) << 9;
		iic_led_state[slot*2+1] |= (vals[offset+2] & 0x0001) << 9;
		iic_led_state[slot*2+1] |= (vals[offset+3] & 0x0001) << 10;
		iic_led_state[slot*2] |= (vals[offset+4] & 0x0001) << 10;
		iic_led_state[slot*2] |= (vals[offset+5] & 0x0001) << 8;
		break;
	case 0x520:	//AIN PTS
		iic_led_state[slot*2] &= ~(0x303 << 6);
		iic_led_state[slot*2+1] &= ~(0x303 << 6);
		if (vals[offset+0] < 0) { iic_led_state[slot*2+0] |= 0x0101 << 6; }
		else if (vals[offset+0] > 20000) { iic_led_state[slot*2+0] |= 0x0100 << 6; }
		else { iic_led_state[slot*2+0] |= 0x0001 << 6; }
		if (vals[offset+1] < 0) { iic_led_state[slot*2+1] |= 0x0101 << 6; }
		else if (vals[offset+1] > 20000) { iic_led_state[slot*2+1] |= 0x0100 << 6; }
		else { iic_led_state[slot*2+1] |= 0x0001 << 6; }
		break;
	default:
		break;
	}
}

void ascii_move(UNSIGNED16 *ascii){
	UNSIGNED8 i = 0;
	for (i=0;i<LED_ROWS;i++){
		ascii[i] >>= 1;		//przesuniecie wyswietlenia w lewo
	}
}

void ascii_add(UNSIGNED16 *ascii, UNSIGNED16 *ascii2, UNSIGNED8 pos){
	UNSIGNED8 i = 0;
	for (i=0;i<LED_ROWS;i++){
		ascii[i] |= (ascii2[i] << pos);
	}
}

void ascii_viz(UNSIGNED16 *out, UNSIGNED16 *ascii){
	UNSIGNED8 i = 0;
	for (i=0;i<LED_ROWS;i++){
		out[i] = ascii[i] & 0x00ff;
	}
}

UNSIGNED16 iic_last_time_lo = 0;
UNSIGNED16 iic_last_time_hi = 0;

void iic_led_loop(){
	UNSIGNED8 i = 0;
	UNSIGNED16 elapsed_time;
	UNSIGNED16 time;
	UNSIGNED16 time_hi;
	elapsed_time = myRTC_sub(iic_last_time_lo, iic_last_time_hi);
	myRTC_ulGetTime(&time, &time_hi);
	if (elapsed_time < 10) return;
	if (time < 10000 && iic_led_test == 1) {	//przy starcie zapalenie wszystkich diod na 1s
		iic_writeWord(0x27, 0x00, 0x0000);	//inicjalizacja pinow jako wyjscia
		iic_writeWord(0x27, 0x12, 0x3F00);	//zapalenie wszystkich
	} else if (time < 80000 && iic_led_test) {
		iic_led_test = 2;
		switch (ascii_step2) {
		case 0:
			ascii_add(ascii_test, ascii_K, 8);
			break;
		case 5:
			ascii_add(ascii_test, ascii_P, 8);
			break;
		case 10:
			ascii_add(ascii_test, ascii_S, 8);
			break;
		case 15:
			ascii_add(ascii_test, ascii__, 8);
			break;
		case 19:
			ascii_add(ascii_test, ascii_2, 8);
			break;
		case 24:
			ascii_add(ascii_test, ascii_DOT, 8);
			break;
		case 26:
			if (main_type_out == 0x0112) {
				ascii_add(ascii_test, ascii_1, 8);
			} else if (main_type_out == 0x0344) {
				ascii_add(ascii_test, ascii_2, 8);
			} else {
				ascii_add(ascii_test, ascii_X, 8);
			}
			break;
		case 40:
			iic_led_test = 0;
		default:
			break;
		}

		if ((time - 10000) > (ascii_step2 << 9)) {
			ascii_move(ascii_test);
			ascii_step2++;
		} 
		ascii_viz(iic_led_state, ascii_test);
	} else {
		iic_led_test = 0;
	}
	myRTC_ulGetTime(&iic_last_time_lo, &iic_last_time_hi);
//	iic_last_time = time;
	iic_led_blink(time, iic_led_state);
	if (iic_led_test != 1) {
		//szukanie pierwszego niepustego wiersza
/*
		for (i=0; i<LED_ROWS ;i++){
			if (i>=iic_led_row){
				if (iic_led_matrix[iic_led_row] == 0xFF) { iic_led_row++; }
				else { break; }
			}
		}
//*/
		if (iic_led_row >= LED_ROWS) {
			iic_led_row = 0;
			iic_writeWord(0x27, 0x12, 0x00FF);
		}
		iic_writeWord(0x27, 0x12, ((1 << iic_led_row) << 8) | iic_led_matrix[iic_led_row]);
		iic_led_row++;
		if (iic_led_row >= LED_ROWS) { iic_led_row = 0; }
	}
}

