/*
 * FlashProgramator.cpp
 *
 *  Created on: Nov 11, 2021
 *      Author: dusan
 */

#include "main.h"
#include "FlashProgramator.h"
#include "AT45DB161D.hpp"

#include <exception>

FlashProgramator::FlashProgramator()
{


}

FlashProgramator::~FlashProgramator() {
	// TODO Auto-generated destructor stub
}

void FlashProgramator::Program(AT45DB161D* flash, UART_HandleTypeDef* huart)
{
	uint8_t data[528];
	uint32_t page_offset = 1;
	HAL_StatusTypeDef halStatus;

	uint8_t ok = 61;
	uint8_t error = 60;

	while (true)
	{
		halStatus = HAL_UART_Receive(huart, data, 528*sizeof(uint8_t), 10000);

		if (halStatus == HAL_OK)
		{
			flash->main_mem_page_program(BUFFER_1, page_offset, 0, data, 528*sizeof(uint8_t));
			HAL_UART_Transmit(huart, &ok, sizeof(uint8_t), 100);
			page_offset++;
		}
		else if (halStatus == HAL_TIMEOUT)
		{
			flash->main_mem_page_program(BUFFER_1, page_offset, 0, data, 528*sizeof(uint8_t));
			HAL_UART_Transmit(huart, &ok, sizeof(uint8_t), 100);
			page_offset++;
			break;
		}
		else
		{
			HAL_UART_Transmit(huart, &error, sizeof(uint8_t), 100);
		}


	}

	uint8_t size[4];
	uint32_t mask = 0x000000FF;
	size[0] = (uint8_t)((page_offset >> 24) & mask);
	size[1] = (uint8_t)((page_offset >> 16) & mask);
	size[2] = (uint8_t)((page_offset >> 8) & mask);
	size[3] = (uint8_t)(page_offset & mask);

	flash->main_mem_page_program(BUFFER_1, 0, 0, size, sizeof(uint32_t));

//	flash->main_mem_page_read(0, 0, data, 528*sizeof(uint8_t));
//	halStatus = HAL_TIMEOUT;
}

