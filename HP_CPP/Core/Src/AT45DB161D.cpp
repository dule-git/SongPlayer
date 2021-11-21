/*
 * AT45DB161D.cpp
 *
 *  Created on: Jul 14, 2021
 *      Author: dule
 */

#include "AT45DB161D.hpp"

// General
// ---------------------------------------------------------------------------------------------------------------------
AT45DB161D::AT45DB161D(SPI_HandleTypeDef* hspi)
{
	this->hspi = hspi;

	this->cs_high();

	string dummy = "dummy_string";

	this->buffer_write(BUFFER_1, 0, (uint8_t*)dummy.c_str(), 12);
	this->buffer_write(BUFFER_2, 0, (uint8_t*)dummy.c_str(), 12);

	uint8_t status_register = this->read_status_register();
	status_register &= SR_POWER_2_BIT;

	this->page_size = status_register & SR_POWER_2_CONFIG ? 512 : 528;

	this->cs_low();
}

uint8_t* AT45DB161D::get_instruction(uint8_t opcode, uint16_t page_offset, uint16_t byte_offset)
{
	uint8_t* instruction = new uint8_t[4];
	instruction[0] = opcode;
	instruction[1] = ((page_offset >> 6) & 0x3F);
	instruction[2] = (((page_offset & 0x3F) << 2) | byte_offset >> 8);
	instruction[3] = (byte_offset & 0xFF);

	return instruction;
}

void AT45DB161D::cs_high()
{
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
	HAL_Delay(20);
}

void AT45DB161D::cs_low()
{
	HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
	HAL_Delay(20);
}

// Util Read
// ---------------------------------------------------------------------------------------------------------------------
void AT45DB161D::main_mem_page_read(uint16_t page_offset, uint16_t byte_offset, uint8_t *page_buffer, uint16_t size)
{
    uint8_t* instruction;
    instruction = this->get_instruction(MAIN_MEMORY_PAGE_READ_OPCODE, page_offset, byte_offset);

	this->cs_low();

    HAL_SPI_Transmit(this->hspi, instruction, 8 * sizeof(uint8_t), HAL_MAX_DELAY);
    HAL_SPI_Receive(this->hspi, page_buffer, size * sizeof(uint8_t), HAL_MAX_DELAY);

	this->cs_high();
}

void AT45DB161D::start_continuous_array_read(uint16_t page_offset, uint16_t byte_offset)
{
	uint8_t* instruction;
    instruction = this->get_instruction(CONTINUOUS_ARRAY_READ_LOWFREQ_OPCODE, page_offset, byte_offset);

    this->cs_low();

    HAL_SPI_Transmit(this->hspi, instruction, 4 * sizeof(uint8_t), HAL_MAX_DELAY);
}

void AT45DB161D::end_continuous_array_read()
{
	this->cs_high();
}

void AT45DB161D::buffer_read(BufferType buffer_type, uint16_t address_offset, uint8_t *buffer, uint16_t size)
{
	uint8_t* instruction;
	uint8_t opcode = buffer_type == BUFFER_1 ? BUFFER_1_READ_OPCODE_LOWFREQ_OPCODE : BUFFER_2_READ_OPCODE_LOWFREQ_OPCODE;
	instruction = this->get_instruction(opcode, 0, address_offset);

	this->cs_low();

	HAL_SPI_Transmit(this->hspi, instruction, 5 * sizeof(uint8_t), HAL_MAX_DELAY);
	HAL_SPI_Receive(this->hspi, buffer, size * sizeof(uint8_t), HAL_MAX_DELAY);

	this->cs_high();
}

// Util Program and Erase
// ---------------------------------------------------------------------------------------------------------------------
void AT45DB161D::buffer_write(BufferType buffer_type, uint16_t address, uint8_t *data, uint16_t size)
{
	uint8_t* instruction;
	uint8_t opcode = buffer_type == BUFFER_1 ? BUFFER_1_WRITE_OPCODE : BUFFER_2_WRITE_OPCODE;
	instruction = this->get_instruction(opcode, 0, address);

	this->cs_low();

	HAL_SPI_Transmit(this->hspi, instruction, 4 * sizeof(uint8_t), HAL_MAX_DELAY);
	HAL_SPI_Transmit(this->hspi, data, size * sizeof(uint8_t), HAL_MAX_DELAY);

	this->cs_high();
}

void AT45DB161D::buffer_to_main_mem_page(BufferType buffer_type, uint16_t page_offset)
{
	uint8_t* instruction;
    uint8_t opcode = buffer_type == BUFFER_1 ?
                     BUFFER_1_TO_MAIN_MEMORY_PAGE_WITH_BUILT_IN_ERASE_OPCODE :
                     BUFFER_2_TO_MAIN_MEMORY_PAGE_WITH_BUILT_IN_ERASE_OPCODE;
    instruction = this->get_instruction(opcode, page_offset, 0);

    this->cs_low();

    HAL_SPI_Transmit(this->hspi, instruction, 4 * sizeof(uint8_t), HAL_MAX_DELAY);

    this->cs_high();

    this->wait_for_ready_signal();
}

void AT45DB161D::page_erase(uint16_t page_offset)
{
	uint8_t* instruction;
    instruction = this->get_instruction(PAGE_ERASE_OPCODE, page_offset, 0);

    this->cs_low();

    HAL_SPI_Transmit(this->hspi, instruction, 4 * sizeof(uint8_t), HAL_MAX_DELAY);

    this->cs_high();

    this->wait_for_ready_signal();
}

void AT45DB161D::chip_erase()
{
    this->cs_low();
    uint8_t CHIP_ERASE_INSTRUCTION[4] = {0xC7, 0x94, 0x80, 0x9A};

    HAL_SPI_Transmit(this->hspi, CHIP_ERASE_INSTRUCTION, 4 * sizeof(uint8_t), HAL_MAX_DELAY);

    this->cs_high();

    this->wait_for_ready_signal();
}

void AT45DB161D::main_mem_page_program(BufferType buffer_type, uint16_t page_offset, uint16_t byte_offset,
                                                   uint8_t *data, uint16_t size)
{
	uint8_t* instruction;
    uint8_t opcode = buffer_type == BUFFER_1 ?
                     MAIN_MEMORY_PAGE_THROUGH_BUFFER_1_OPCODE :
                     MAIN_MEMORY_PAGE_THROUGH_BUFFER_2_OPCODE;
    instruction = this->get_instruction(opcode, page_offset, byte_offset);

    this->cs_low();

    HAL_SPI_Transmit(this->hspi, instruction, 4 * sizeof(uint8_t), HAL_MAX_DELAY);
    HAL_SPI_Transmit(this->hspi, data, size * sizeof(uint8_t), HAL_MAX_DELAY);

    this->cs_high();

    this->wait_for_ready_signal();
}

void AT45DB161D::write(BufferType buffer_type, uint16_t page_offset, uint8_t *data, uint16_t size)
{
    //  Ovo bi trebalo da stoji umesto data, ovo sam uradio jer je fajl previse veliki
//data + i * this->page_size
    for (uint32_t i = page_offset; i < size / this->page_size + page_offset; i += 1)
        this->main_mem_page_program(buffer_type, i, 0, data + i * this->page_size,
                                                      this->page_size);

    this->main_mem_page_program(buffer_type,
    							size / this->page_size,
								0,
								(uint8_t *) (data +
								(size / this->page_size) * this->page_size),
								size - (size / this->page_size) * this->page_size
    );
}

// Util Additional Commands
// ---------------------------------------------------------------------------------------------------------------------

uint8_t AT45DB161D::read_status_register()
{
    uint8_t instruction = READ_STATUS_REGISTER_OPCODE;
    uint8_t status_register;

    this->cs_low();

    HAL_SPI_Transmit(this->hspi, &instruction, sizeof(uint8_t), HAL_MAX_DELAY);
    HAL_SPI_Receive(this->hspi, &status_register, sizeof(uint8_t), HAL_MAX_DELAY);

    this->cs_high();

    return status_register;
}

void AT45DB161D::wait_for_ready_signal()
{
    uint8_t instruction = READ_STATUS_REGISTER_OPCODE;
    uint8_t status_register;

this->cs_low();

    HAL_SPI_Transmit(this->hspi, &instruction, sizeof(uint8_t), HAL_MAX_DELAY);
    HAL_SPI_Receive(this->hspi, &status_register, sizeof(uint8_t), HAL_MAX_DELAY);

    uint8_t device_ready = status_register & SR_DEVICE_STATE_BIT;
    while (device_ready != SR_DEVICE_STATE_READY)
    {
        HAL_SPI_Receive(this->hspi, &status_register, sizeof(uint8_t), HAL_MAX_DELAY);
        device_ready = status_register & SR_DEVICE_STATE_BIT;
    }

    this->cs_high();
}

//void AT45DB161D::test()
//{
//	uint8_t buffer[1316];
//
//	string s = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Pulvinar sapien et ligula ullamcorper. Facilisis volutpat est velit egestas dui id ornare. Nunc mattis enim ut tellus. Tellus at urna condimentum mattis pellentesque. Maecenas volutpat blandit aliquam etiam erat velit scelerisque. Faucibus et molestie ac feugiat. Ridiculus mus mauris vitae ultricies leo integer malesuada. Porttitor massa id neque aliquam vestibulum. Eu consequat ac felis donec et odio. Blandit libero volutpat sed cras ornare arcu. Nam at lectus urna duis convallis convallis. Etiam tempor orci eu lobortis elementum nibh tellus.\n"
//            "\n"
//            "Quam vulputate dignissim suspendisse in est ante in nibh. Arcu non sodales neque sodales ut etiam sit amet nisl. Adipiscing enim eu turpis egestas pretium aenean pharetra. Mollis aliquam ut porttitor leo a diam. Hendrerit dolor magna eget est lorem ipsum. Cursus eget nunc scelerisque viverra mauris in aliquam sem fringilla. Adipiscing diam donec adipiscing tristique. At elementum eu facilisis sed odio morbi quis. Fermentum dui faucibus in ornare quam. Felis eget velit aliquet sagittis id consectetur purus ut faucibus. Neque vitae tempus quam pellentesque nec nam aliquam sem. Ultrices mi tempus imperdiet nulla malesuada pellentesque.";
//
//	uint16_t size = s.size() * sizeof(uint8_t);
//
//	this->write(BUFFER_1, 0, (uint8_t*)s.c_str(), size);
//
//	uint16_t i = 0;
//	this->start_continuous_array_read(0, 0);
//	while(i < size)
//	{
//		HAL_SPI_Receive(this->hspi, (buffer+i), sizeof(uint8_t), HAL_MAX_DELAY);
//		i++;
//	}
//	this->end_continuous_array_read();
//
//	buffer[size]='\0';
//	uint8_t res = strcmp(s.c_str(), (const char*) buffer);
//	if (res == 0)
//	{
//		int l = 9;
//	}
//
//
//}


























