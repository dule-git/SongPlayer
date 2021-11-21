/*
 * AT45DB161D.hpp
 *
 *  Created on: Jul 14, 2021
 *      Author: dule
 */

#ifndef INC_AT45DB161D_HPP_
#define INC_AT45DB161D_HPP_

#include "main.h"
#include <string>
#include <cstring>

using namespace std;

#define CS_PORT GPIOB
#define CS_PIN GPIO_PIN_12
#define WP_PORT -1
#define WP_PIN -1
#define RESET_PORT -1
#define RESET_PIN -1

// Read Opcodes
#define MAIN_MEMORY_PAGE_READ_OPCODE 0xD2
#define CONTINUOUS_ARRAY_READ_LOWFREQ_OPCODE 0x03
#define CONTINUOUS_ARRAY_READ_HIGHFREQ_OPCODE 0x0B
#define BUFFER_1_READ_OPCODE_LOWFREQ_OPCODE 0xD1
#define BUFFER_2_READ_OPCODE_LOWFREQ_OPCODE 0xD3
#define BUFFER_1_READ_OPCODE 0xD4
#define BUFFER_2_READ_OPCODE 0xD6

// Write Opcodes
#define BUFFER_1_WRITE_OPCODE 0x84
#define BUFFER_2_WRITE_OPCODE 0x87
#define BUFFER_1_TO_MAIN_MEMORY_PAGE_WITH_BUILT_IN_ERASE_OPCODE 0x83
#define BUFFER_2_TO_MAIN_MEMORY_PAGE_WITH_BUILT_IN_ERASE_OPCODE 0x86
#define PAGE_ERASE_OPCODE 0x81
#define BLOCK_ERASE_OPCODE 0x50
#define SECTOR_ERASE_OPCODE 0x7C
#define MAIN_MEMORY_PAGE_THROUGH_BUFFER_1_OPCODE 0x82
#define MAIN_MEMORY_PAGE_THROUGH_BUFFER_2_OPCODE 0x85

// Additional Opcodes
#define READ_STATUS_REGISTER_OPCODE 0xD7

// Status Register Bits and Related Content
#define SR_DEVICE_STATE_BIT (1 << 7)
#define SR_DEVICE_STATE_BUSY 0x00
#define SR_DEVICE_STATE_READY (1 << 7)
#define SR_COMPARE_RESULT_BIT (1 << 6)
#define SR_COMPARE_RESULT_EQ 0x00
#define SR_COMPARE_RESULT_NEQ (1 << 6)
#define SR_SECTOR_PROTECTION_BIT (1 << 1)
#define SR_SECTOR_PROTECTION_ENABLED (1 << 1)
#define SR_SECTOR_PROTECTION_DISABLED 0x00
#define SR_POWER_2_BIT (1 << 0)
#define SR_POWER_2_CONFIG (1 << 0)
#define SR_POWER_2_NCONFIG 0x00

typedef enum {
	BUFFER_1, BUFFER_2
} BufferType;

class AT45DB161D
{
public:
	AT45DB161D(SPI_HandleTypeDef* hspi);

	// Util Read
	void buffer_read(BufferType buffer_type, uint16_t address_offset, uint8_t *buffer, uint16_t size);
	void main_mem_page_read(uint16_t page_offset, uint16_t byte_offset, uint8_t *page_buffer, uint16_t size);
	void start_continuous_array_read(uint16_t page_offset, uint16_t byte_offset);
	void end_continuous_array_read();

	// Util Program and Erase
	void buffer_write(BufferType buffer_type, uint16_t address, uint8_t *data, uint16_t size);
	void buffer_to_main_mem_page(BufferType buffer_type, uint16_t page_offset);
	void page_erase(uint16_t page_offset);
	void chip_erase();
	void main_mem_page_program(BufferType buffer_type, uint16_t page_offset, uint16_t byte_offset,
	                                                   uint8_t *data, uint16_t size);
	void write(BufferType buffer_type, uint16_t page_offset, uint8_t *data, uint16_t size);

	// Util Additional Commands
	uint8_t read_status_register();
	void wait_for_ready_signal();

//	void test();

private:
	uint8_t* get_instruction(uint8_t opcode, uint16_t page_offset, uint16_t byte_offset);
	void cs_high();
	void cs_low();

	SPI_HandleTypeDef* hspi;
	uint16_t page_size;
};



#endif /* INC_AT45DB161D_HPP_ */
