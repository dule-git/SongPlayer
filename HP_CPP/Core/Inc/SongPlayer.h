/*
 * SongPlayer.h
 *
 *  Created on: Nov 12, 2021
 *      Author: dusan
 */

#ifndef SONGPLAYER_H_
#define SONGPLAYER_H_

#include "stm32f1xx_hal_tim.h"

class AT45DB161D;
class Song;

class SongPlayer {

public:
	SongPlayer();
	virtual ~SongPlayer();
	void Play(Song* song, TIM_HandleTypeDef* htim);

	static bool periodElapsed;
private:
	Song* song;
};

#endif /* SONGPLAYER_H_ */
