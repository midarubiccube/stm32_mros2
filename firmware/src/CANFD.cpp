#include <string.h>

#include "CANFD.hpp"
#include "fdcan.h"

void CANFD::init(){
	if(HAL_FDCAN_Start(fdcan_)!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_FDCAN_ActivateNotification(fdcan_, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
	   /* Notification Error */
	   Error_Handler();
	}
}

bool CANFD::tx(CANFD_Frame &tx_data){
	FDCAN_TxHeaderTypeDef	TxHeader;
	TxHeader.Identifier = tx_data.id;
	TxHeader.IdType = FDCAN_EXTENDED_ID;
	TxHeader.TxFrameType = FDCAN_DATA_FRAME;
	TxHeader.DataLength = FDCAN_DLC_BYTES_32;
	TxHeader.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
	TxHeader.BitRateSwitch = FDCAN_BRS_ON;
	TxHeader.FDFormat = FDCAN_FD_CAN;
	TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	TxHeader.MessageMarker = 0;

	// Prepare first Tx data for fdcan1
	for (size_t i = 0; i < tx_data.size; i++) {
		TxData[i] = tx_data.data[i];
	}

	// Put Tx data to Txfifo
	if (HAL_FDCAN_AddMessageToTxFifoQ(fdcan_, &TxHeader, TxData)!= HAL_OK) {
		Error_Handler();
	}
	return true;
}

uint32_t CANFD::rx_available(void){
	uint32_t count = 0;
	for(uint32_t i = 0; i < CAN_RX_BUFF_N;i++){
		if(!rx_buff[i].is_free) count ++;
	}
	return count;
}

void CANFD::rx_interrupt_task(void){
	FDCAN_RxHeaderTypeDef	RxHeader;
	uint8_t		fdcan1RxData[64];

    if (HAL_FDCAN_GetRxMessage(fdcan_, FDCAN_RX_FIFO0, &RxHeader, fdcan1RxData) != HAL_OK) {
		Error_Handler();
    }

    if (HAL_FDCAN_ActivateNotification(fdcan_, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
          /* Notification Error */
    	Error_Handler();
    }

	rx_buff[head].id = RxHeader.Identifier;
	rx_buff[head].size = RxHeader.DataLength;
	memcpy(&rx_buff[head].data, fdcan1RxData, 64);
	rx_buff[head].is_free = false;

	head = (head+1)&CAN_RX_BUFF_AND;
}

bool CANFD::rx(CANFD_Frame &rx_frame){
	if(!rx_buff[tail].is_free){
		rx_frame = rx_buff[tail];
		rx_buff[tail].is_free = true;
		tail = (tail+1)&CAN_RX_BUFF_AND;
		return true;
	}else{
		return false;
	}
}

//filter///////////////////////////////////////////////////////////////////////////////////
/*void CANFD::set_filter_mask(uint32_t id,uint32_t mask,filter_mode mode,bool as_std){
	FDCAN_FilterTypeDef filter;
	uint32_t filter_id;
	uint32_t filter_mask;
	switch(mode){
	case filter_mode::only_std:
		if(as_std){
			filter_id = id << 21;
			filter_mask = mask << 21 | 0x4;
		}else{
			filter_id = id << 3;
			filter_mask = mask << 3 | 0x4;
		}
		break;
	case filter_mode::only_ext:
		if(as_std){
//			filter_id = id << 21 | 0x4;
			filter_mask = mask << 21 | 0x4;
		}else{
			filter_id = id << 3 | 0x4;
			filter_mask = mask << 3 | 0x4;
		}
		break;
	case filter_mode::std_and_ext:
		if(as_std){
			filter_id = id << 21;
			filter_mask = mask << 21;
		}else{
			filter_id = id << 3;
			filter_mask = mask << 3;
		}

	}

	filter.FilterIdHigh         = filter_id >> 16;
	filter.FilterIdLow          = filter_id;
	filter.FilterMaskIdHigh     = filter_mask >> 16;
	filter.FilterMaskIdLow      = filter_mask;
	filter.FilterScale          = CAN_FILTERSCALE_32BIT; // 32モード
	filter.FilterFIFOAssignment = rx_fifo;      // FIFO0へ格納
	filter.FilterBank           = 0;
	filter.FilterMode           = CAN_FILTERMODE_IDMASK; // IDマスクモード
	filter.SlaveStartFilterBank = 14;
	filter.FilterActivation     = ENABLE;

	HAL_FDCAN_ConfigGlobalFilter(can, &filter);
}
void CANFD::set_filter_free(void){
	FDCAN_FilterTypeDef filter;
	filter.FilterIdHigh         = 0;
	filter.FilterIdLow          = 0;
	filter.FilterMaskIdHigh     = 0;
	filter.FilterMaskIdLow      = 0;
	filter.FilterScale          = CAN_FILTERSCALE_32BIT;
	filter.FilterFIFOAssignment = rx_fifo;
	filter.FilterBank           = 0;
	filter.FilterMode           = CAN_FILTERMODE_IDMASK;
	filter.SlaveStartFilterBank = 14;
	filter.FilterActivation     = ENABLE;
	HAL_FDCAN_ConfigGlobalFilter(can, &filter);
}*/