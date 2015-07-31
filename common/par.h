#ifndef _PAR_H_
#define _PAR_H_

uword flashword [64];

int odczyt_parametrow (void){
	int RetStatus;
	uword *Readbuff;
	ubyte address = 0;
	ubyte i = 0;
//	uword temp_crc;
	/* Point the pointer to the buffer */
	Readbuff = &flashword[0];
	/* Fetch EEPROM data with the EEPROM logical address */
	RetStatus = EEPROM_intRead(Readbuff, address);
	if (RetStatus == COMPLETE) {
		if (crc16w((unsigned short *)flashword, 27) != flashword[27]) return FAILED;
//		PRINTF("\nRead success\n\r");
		for (i = 0; i < 4; i += 1) {
			zakresy_analog_out_ch1[ i + 1 ] = flashword[i];
		}
		for (i = 0; i < 4; i += 1) {
			zakresy_analog_out_ch2[ i + 1 ] = flashword[ i + 4 ];
		}
		for (i = 0; i < 8; i += 1) {
			zakresy_przekaznika_ch1[ i + 1 ] = flashword[ i + 8 ];
		}
		for (i = 0; i < 8; i += 1) {
			zakresy_przekaznika_ch2[ i + 1 ] = flashword[ i + 16 ];
		}
		config_upt = flashword[ 24 ];
		status_wewn[3] = flashword[ 25 ];
		status_wewn[4] = flashword[ 26 ];
//		flashword[ 27 ] = temp_crc;
	} //else {
//		PRINTF("\nRead Failed\n\r");
		return RetStatus;
//	}
}

int odczyt_mapowania (void){
/*
	int RetStatus;
	uword *Readbuff;
	ubyte address = 1;
	ubyte i = 0;
	Readbuff = &flashword[0];
	RetStatus = EEPROM_intRead(Readbuff, address);
	if (RetStatus == COMPLETE) {
		for (i = 0; i < 8; i += 1) {
			key_map_usb1[ i + 1 ] = flashword[i];
		}
		for (i = 0; i < 8; i += 1) {
			key_map_usb2[ i + 1 ] = flashword[ i + 8 ];
		}
	} 
	return RetStatus;
*/
	return COMPLETE;
}
void domyslne_parametry (void){
	ubyte i = 1;
	zakresy_analog_out_ch1[ i++ ] = 100;
	zakresy_analog_out_ch1[ i++ ] = 176;
	zakresy_analog_out_ch1[ i++ ] = 0;
	zakresy_analog_out_ch1[ i++ ] = 200;
	i = 1;
	zakresy_analog_out_ch2[ i++ ] = 100;
	zakresy_analog_out_ch2[ i++ ] = 176;
	zakresy_analog_out_ch2[ i++ ] = 0;
	zakresy_analog_out_ch2[ i++ ] = 200;
	i = 1;
	zakresy_przekaznika_ch1[ i++ ] = 70;
	zakresy_przekaznika_ch1[ i++ ] = 90;
	zakresy_przekaznika_ch1[ i++ ] = 150;
	zakresy_przekaznika_ch1[ i++ ] = 154;
	zakresy_przekaznika_ch1[ i++ ] = -65;
	zakresy_przekaznika_ch1[ i++ ] = -15;
	zakresy_przekaznika_ch1[ i++ ] = 130;
	zakresy_przekaznika_ch1[ i++ ] = 140;
	i = 1;
	zakresy_przekaznika_ch2[ i++ ] = 70;
	zakresy_przekaznika_ch2[ i++ ] = 90;
	zakresy_przekaznika_ch2[ i++ ] = 150;
	zakresy_przekaznika_ch2[ i++ ] = 154;
	zakresy_przekaznika_ch2[ i++ ] = -65;
	zakresy_przekaznika_ch2[ i++ ] = -15;
	zakresy_przekaznika_ch2[ i++ ] = 130;
	zakresy_przekaznika_ch2[ i++ ] = 140;
	config_upt = 0x0C;
	status_wewn[3] = 4;
	status_wewn[4] = 4;
}

int zapis_parametrow (void){
//	int RetStatus;
//	uword *Readbuff;
	ubyte address = 0;
	ubyte i = 0;

	for (i = 0; i < 4; i += 1) {
		flashword[i] = zakresy_analog_out_ch1[ i + 1 ];
	}
	for (i = 0; i < 4; i += 1) {
		flashword[ i + 4 ] = zakresy_analog_out_ch2[ i + 1 ];
	}
	for (i = 0; i < 8; i += 1) {
		flashword[ i + 8 ] = zakresy_przekaznika_ch1[ i + 1 ];
	}
	for (i = 0; i < 8; i += 1) {
		flashword[ i + 16 ] = zakresy_przekaznika_ch2[ i + 1 ];
	}
	flashword[ 24 ] = config_upt;
	flashword[ 25 ] = status_wewn[3];
	flashword[ 26 ] = status_wewn[4];
	flashword[ 27 ] = crc16w((unsigned short *)flashword, 27);
	return EEPROM_intWrite (flashword, address);
}

int zapis_mapowania (void){
	ubyte address = 1;
	ubyte i = 0;

/*
	for (i = 0; i < 8; i += 1) {
		flashword[i] = key_map_usb1[ i + 1 ];
	}
	for (i = 0; i < 8; i += 1) {
		flashword[ i + 8 ] = key_map_usb2[ i + 1 ];
	}
	return EEPROM_intWrite (flashword, address);
*/
	return COMPLETE;
}
#endif  // ifndef _PAR_H_
