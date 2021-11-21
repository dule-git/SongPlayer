/*
 * Song.cpp
 *
 *  Created on: Nov 12, 2021
 *      Author: dusan
 */

#include "Song.h"
#include "AT45DB161D.hpp"

Song::Song():
	flash(nullptr),
	counter(1)
{ }

Song::~Song()
{ }

uint32_t Song::extractSize(uint8_t * songInfo)
{
	uint32_t size = 0;

	size |= songInfo[0] << 24;
	size |= songInfo[1] << 16;
	size |= songInfo[2] << 8;
	size |= songInfo[3];

	return size;
}

void Song::Load(AT45DB161D* flash)
{
	if (this->flash != nullptr)
		delete flash;

	this->flash = flash;

	uint8_t songInfo[528];
	this->flash->main_mem_page_read(0, 0, songInfo, 528*sizeof(uint8_t));

	this->size = extractSize(songInfo);
}

uint32_t Song::GetSize()
{
	return size;
}

void Song::GetNextBlock(uint8_t* data)
{
	flash->main_mem_page_read(counter++, 0, data, 528*sizeof(uint8_t));
}
