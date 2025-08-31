#pragma once

#include "fdcan.h"

#define CAN_RX_BUFF_N 16
#define CAN_RX_BUFF_AND 0xF

struct CANFD_Frame{
	uint32_t id=0;
	uint8_t data[64]={0};
	size_t size=0;
	bool  is_free = true;
};

class CANFD{
private:
	FDCAN_HandleTypeDef *fdcan_;

	CANFD_Frame rx_buff[CAN_RX_BUFF_N];
	uint8_t		TxData[64];
	uint32_t head = 0;
	uint32_t tail = 0;

public:
	CANFD(FDCAN_HandleTypeDef *can) : fdcan_(can)
	{
	}
	//can tx CANFD_Frame/////////////////////////////
	void init();
	bool tx(CANFD_Frame &tx_data);

	//can rx fuctions//////////////////////////////
	uint32_t rx_available(void);
	void rx_interrupt_task(void);
	bool rx(CANFD_Frame &rx_frame);

	//can filter setting///////////////////////////
	//void set_filter_mask(uint32_t id,uint32_t mask,filter_mode mode,bool as_std);
	void set_filter_free(void);
};

extern CANFD *canfd;