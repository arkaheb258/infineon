
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <cal_conf.h>      /* !!! first file to include for all CANopen */
#include <examples.h>
#include <dave\usic1.h>
#include <dave\io.h>

uword uwFIFO_SendData[8]; 
uword uwFIFO_RecData[4] = {0};

uword mcp23017_readData(uword *uwRxData, uword uwSlAdr, uword uwMemAdr, uword uwFIFOLevel){
	ubyte i;
	uword uwReadData = 0; 
	ubyte ubSendData = 0; 
	uword uwStatus = 0;
	uword temp0 = 0;
	uword temp1 = 0;
	uwSlAdr <<= 1;
	if (uwFIFOLevel > 4)
		return 0;
	uwFIFO_SendData[ubSendData++]= (((U1C0TDF_MStart << 8) & 0x0700) | ((uwSlAdr + U1C0IIC_WRITE)& 0x00FF)); // Write mode 
	uwFIFO_SendData[ubSendData++]= (((U1C0TDF_MTxData << 8) & 0x0700) | (uwMemAdr& 0x00FF));  // Send Mem Address 
	uwFIFO_SendData[ubSendData++]= (((U1C0TDF_MRStart << 8) & 0x0700) | ((uwSlAdr + U1C0IIC_READ)& 0x00FF)); // Read mode 
	for (i=0;i<uwFIFOLevel-1;i++)
		uwFIFO_SendData[ubSendData++]= ((U1C0TDF_MRxAck0 << 8) & 0x0700); // TDF_Ack0 
	uwFIFO_SendData[ubSendData++]= ((U1C0TDF_MStop << 8) & 0x0700);   // TDF_Stop 

	PRINTF("tx f[0x%02x]\n", U1C0_IIC_ubGetTxFIFOFillingLevel());
//	U1C0_IIC_vResetStatus(0xFFFF);

PRINTF("st [0x%02x]\n", U1C0_IIC_uwGetStatus());
PRINTF("tcs [0x%02x]\n", U1C0_TCSRL);
PRINTF("3\n"); 
//obsluga bledow transmisji
	uwStatus = U1C0_IIC_uwGetStatus() ;
	while(!U1C0_IIC_ubIsTxFIFOempty()){ // Wait for TxFIFO Empty
		PRINTF("err [0x%02x]\n", uwStatus);
		if ((uwStatus = U1C0_IIC_uwGetStatus())& (U1C0IIC_NACK | U1C0IIC_ERR | U1C0IIC_PCR)){
			while(U1C0_IIC_ubIsTxFIFObusy()); // Wait for TxFIFO Not Busy
			U1C0_IIC_vFlushTxFIFO(); // clear the Tx FIFO
			while(U1C0_IIC_ubIsRxFIFObusy()); // Wait for RxFIFO Not Busy
			U1C0_IIC_vFlushRxFIFO();// clear the Rx FIFO
//			U1C0_IIC_vResetStatus(0xFFFF);
//			return 0;
			uwStatus = 0xFFFF;
			PRINTF("err 0xF\n");
		}
		PRINTF("err2 [0x%02x]\n", U1C0_IIC_uwGetStatus());
		if (uwStatus == 0x0000){
			PRINTF("err 0x0\n");
			break;
		}
	}
	if (uwStatus == 0xFFFF){
		PRINTF("err ret\n");
        U1C0_IIC_vResetStatus(U1C0IIC_ERR + U1C0IIC_ARL + U1C0IIC_NACK + U1C0IIC_WTDF);
//		U1C0_IIC_vResetStatus(0x00FF);
//		U1C0_IIC_vResetStatus(0xFFFF);
//		U1C0_IIC_vResetStatus(0x0000);
//		U1C0_IIC_uwGetStatus() = 0;
		U1C0_IIC_vInit();
		return 0;
//		U1C0_IIC_vResetStatus(0xFFFF);
	}
//	while(!U1C0_IIC_ubIsTxFIFOempty()){ // Wait for TxFIFO Empty
//		if ((U1C0_IIC_uwGetStatus() == 0x0000) || U1C0_IIC_uwGetStatus()& (U1C0IIC_NACK | U1C0IIC_ERR | U1C0IIC_PCR)){
//			U1C0_IIC_vFlushTxFIFO(); // clear the Tx FIFO
//			U1C0_IIC_vFlushRxFIFO();
//			U1C0_IIC_vResetStatus(0xFFFF);
//			return 0;
//		}
//	}
PRINTF("tcs [0x%02x]\n", U1C0_TCSRL);
PRINTF("4\n"); 
	while(U1C0_IIC_ubIsTxFIFObusy()); // Wait for TxFIFO Not Busy
	U1C0_IIC_vFlushTxFIFO(); // clear the Tx FIFO
	while(U1C0_IIC_ubIsRxFIFObusy()); // Wait for RxFIFO Not Busy
	U1C0_IIC_vFlushRxFIFO();// clear the Rx FIFO

	U1C0_IIC_vFillTxFIFO(uwFIFO_SendData , ubSendData);

	uwStatus = U1C0_IIC_uwGetStatus() ;
PRINTF("st [0x%02x]\n", U1C0_IIC_uwGetStatus());
PRINTF("tcs [0x%02x]\n", U1C0_TCSRL);
PRINTF("5\n"); 
	while(U1C0_IIC_ubIsTxFIFObusy()); // Wait for TxFIFO Not Busy
//PRINTF("6\n"); 

//obsluga bledow transmisji
	while(!U1C0_IIC_ubIsTxFIFOempty()){ // Wait for TxFIFO Empty
		PRINTF("err [0x%02x]\n", uwStatus);
		if ((uwStatus = U1C0_IIC_uwGetStatus())& (U1C0IIC_NACK | U1C0IIC_ERR | U1C0IIC_PCR)){
			while(U1C0_IIC_ubIsTxFIFObusy()); // Wait for TxFIFO Not Busy
			U1C0_IIC_vFlushTxFIFO(); // clear the Tx FIFO
			while(U1C0_IIC_ubIsRxFIFObusy()); // Wait for RxFIFO Not Busy
			U1C0_IIC_vFlushRxFIFO();// clear the Rx FIFO
//			U1C0_IIC_vResetStatus(0xFFFF);
//			return 0;
			uwStatus = 0xFFFF;
			PRINTF("err 0xF\n");
		}
		PRINTF("err2 [0x%02x]\n", U1C0_IIC_uwGetStatus());
		if (uwStatus == 0x0000){
			PRINTF("err 0x0\n");
			break;
		}
	}
	if (uwStatus == 0xFFFF){
		PRINTF("err ret\n");
        U1C0_IIC_vResetStatus(U1C0IIC_ERR + U1C0IIC_ARL + U1C0IIC_NACK + U1C0IIC_WTDF);
//        U1C0_IIC_vResetStatus(U1C0IIC_ERR + U1C0IIC_ARL + U1C0IIC_NACK);
//		U1C0_IIC_vResetStatus(0x00FF);
//        U1C0_IIC_vResetStatus(U1C0IIC_ERR + U1C0IIC_ARL);
//		U1C0_IIC_vResetStatus(0xFFFF);
//		U1C0_IIC_vResetStatus(0x0000);
//		U1C0_IIC_uwGetStatus() = 0;
		U1C0_IIC_vInit();
		return 0;
//		U1C0_IIC_vResetStatus(0xFFFF);
	}
PRINTF("ok [0x%02x]\n", U1C0_IIC_uwGetStatus());
PRINTF("7\n"); 
	while(U1C0_IIC_ubIsTxFIFObusy()); // Wait for TxFIFO Not Busy

	if (U1C0_IIC_ubGetRxFIFOFillingLevel() > uwFIFOLevel){
PRINTF("ovf\n"); 
			U1C0_IIC_vFlushTxFIFO(); // clear the Tx FIFO
			U1C0_IIC_vFlushRxFIFO();
//			U1C0_IIC_vResetStatus(0xFFFF);
			return 0;
	}
PRINTF("8\n"); 
	while ((temp0 = U1C0_IIC_ubGetRxFIFOFillingLevel()) < uwFIFOLevel-1){ // Wait for Data's 
	uwStatus = U1C0_IIC_uwGetStatus() ;
PRINTF("rx f[0x%02x]\n", temp0);
PRINTF("st f[0x%02x]\n", uwStatus);
//		if (U1C0_IIC_uwGetStatus()& (U1C0IIC_NACK | U1C0IIC_ERR | U1C0IIC_PCR)){
		if (uwStatus & U1C0IIC_ERR){
			U1C0_IIC_vFlushTxFIFO(); // clear the Tx FIFO
			U1C0_IIC_vFlushRxFIFO();
//			U1C0_IIC_vResetStatus(0xFFFF);
			return 0;
		}
	}
PRINTF("9\n"); 
	while (uwReadData < uwFIFOLevel){ // Read Data's 
		while (U1C0_IIC_ubIsRxFIFObusy()); // Wait for RxFIFO Not busy 
		uwRxData[uwReadData] = U1C0_IIC_uwGetRxFIFOData(); // Get the Received Data 
//		PRINTF("Rec Data\n"); 
		uwReadData++; 
	} 
PRINTF("10\n"); 
	U1C0_IIC_vFlushRxFIFO();
	while(!U1C0_IIC_ubIsRxFIFOempty()); // Wait for Rx FIFO Empty
	return uwReadData;
}

void mcp23017_init(void){
	ubyte ubReadData = 0; 
	ubyte ubFIFOLevel = 3;
	uword uwSlAdr = 0x68 << 1;
	uword uwMemAdr = 0x00;
//	uwFIFO_SendData[0]= (((U1C0TDF_MStart << 8) & 0x0700) | ((uwSlAdr + U1C0IIC_READ)& 0x00FF));  // Read mode 
	uwFIFO_SendData[0]= (((U1C0TDF_MStart << 8) & 0x0700) | ((uwSlAdr + U1C0IIC_WRITE)& 0x00FF)); // Write mode 
	uwFIFO_SendData[1]= (((U1C0TDF_MTxData << 8) & 0x0700) | (uwMemAdr& 0x00FF));  // Send Mem Address 
	uwFIFO_SendData[2]= (((U1C0TDF_MRStart << 8) & 0x0700) | ((uwSlAdr + U1C0IIC_READ)& 0x00FF)); // Read mode 
	uwFIFO_SendData[3]= ((U1C0TDF_MRxAck0 << 8) & 0x0700); // TDF_Ack0 
	uwFIFO_SendData[4]= ((U1C0TDF_MRxAck0 << 8) & 0x0700); // TDF_Ack0 
	uwFIFO_SendData[5]= ((U1C0TDF_MRxAck1 << 8) & 0x0700); // TDF_Ack1 
	uwFIFO_SendData[6]= ((U1C0TDF_MStop << 8) & 0x0700);   // TDF_Stop 
/*
	PRINTF("\nSend0 = 0x%02x", uwFIFO_SendData[0]); // Read 1st Byte
	PRINTF("\nSend1 = 0x%02x", uwFIFO_SendData[1]); // Read 1st Byte
	PRINTF("\nSend2 = 0x%02x", uwFIFO_SendData[2]); // Read 1st Byte
	PRINTF("\nSend3 = 0x%02x", uwFIFO_SendData[3]); // Read 1st Byte
	PRINTF("\nSend4 = 0x%02x", uwFIFO_SendData[4]); // Read 1st Byte
	PRINTF("\nSend5 = 0x%02x", uwFIFO_SendData[5]); // Read 1st Byte
	PRINTF("\nSend6 = 0x%02x", uwFIFO_SendData[6]); // Read 1st Byte
*/
	while(!U1C0_IIC_ubIsTxFIFOempty()); // Wait for Tx FIFO Empty
	while(U1C0_IIC_ubIsTxFIFObusy()); // Wait for TxFIFO Not Busy
	U1C0_IIC_vFlushRxFIFO();
	while(!U1C0_IIC_ubIsRxFIFOempty()); // Wait for Tx FIFO Empty
	U1C0_IIC_vFlushTxFIFO(); // clear the transmit FIFO

	U1C0_IIC_vFillTxFIFO(uwFIFO_SendData , 7);
	PRINTF("Protocol Status[0x%02x]\n", U1C0_IIC_uwGetStatus());
	while (U1C0_IIC_ubGetRxFIFOFillingLevel() != 3); // Wait for 3 Data's 
	while (ubReadData != ubFIFOLevel){ // Read Two Data's 
		while (U1C0_IIC_ubIsRxFIFObusy()); // Wait for RxFIFO Not busy 
		uwFIFO_RecData[ubReadData] = U1C0_IIC_uwGetRxFIFOData(); // Get the Received Data 
//		PRINTF("Rec Data\n"); 
		ubReadData++; 
	} 
	U1C0_IIC_vFlushRxFIFO();
	while(!U1C0_IIC_ubIsRxFIFOempty()); // Wait for Tx FIFO Empty
	PRINTF("RecData0[0x%02x]\n", (uwFIFO_RecData[0] & 0x00FF)); // Read 1st Byte
	PRINTF("RecData1[0x%02x]\n", (uwFIFO_RecData[1] & 0x00FF)); // Read 2nd Byte
	PRINTF("RecData2[0x%02x]\n", (uwFIFO_RecData[2] & 0x00FF)); // Read 1st Byte
/*
//	PRINTF(s, "\r\nRecData0[%x]", (uwFIFO_RecData[0] & 0x00FF)); // Read 1st Byte
*/
}