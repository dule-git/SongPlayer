/*
 * Song.h
 *
 *  Created on: Nov 12, 2021
 *      Author: dusan
 */

#ifndef SRC_SONG_H_
#define SRC_SONG_H_

#include <stdint.h>

class AT45DB161D;

class Song {
public:
	Song();
	virtual ~Song();
	void Load(AT45DB161D* flash);

	uint32_t GetSize();
	void GetNextBlock(uint8_t* data);
private:
	uint32_t extractSize(uint8_t* songInfo);
private:
	uint32_t size; // size in blocks of 528 bytes
	AT45DB161D* flash;
	uint32_t counter;
};

#endif /* SRC_SONG_H_ */
