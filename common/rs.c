#include <rs.h>

#if HW_UPT == 1
//void rs_send(UNSIGNED8 port, UNSIGNED16 *uwTxData, UNSIGNED16 *uwRxData, UNSIGNED16 uwDataSize){
//	UNSIGNED16 i;
//	UNSIGNED8 ptr = 0;
//			SPI_FLUSH_RX(U2C0);
//			SPI_FLUSH_TX(U2C0);
//			SPI_FILL_TX(U2C0, uwTxData + ptr, SPI_FIFO_SIZE);
//					while(SPI_FILLING_RX(U2C0) < SPI_FIFO_SIZE);
//					for (i=0;i<SPI_FIFO_SIZE;i++)
//						uwRxData[i + ptr] = SPI_GET_RX(U2C0);
//					ptr += SPI_FIFO_SIZE;
//}

#endif
