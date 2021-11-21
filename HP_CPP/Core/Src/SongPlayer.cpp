/*
 * SongPlayer.cpp
 *
 *  Created on: Nov 12, 2021
 *      Author: dusan
 */

#include "main.h"
#include "SongPlayer.h"
#include "AT45DB161D.hpp"
#include "Song.h"

bool SongPlayer::periodElapsed = false;

SongPlayer::SongPlayer():
	song(nullptr)
{

}

SongPlayer::~SongPlayer()
{

}
Song* song1;
uint8_t data[528];
int counter=0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (counter == 528)
	{
		counter = 0;
		song1->GetNextBlock(data);
	}
	htim->Instance->CCR1 = data[counter];
}

void SongPlayer::Play(Song* song, TIM_HandleTypeDef* htim)
{
	HAL_TIM_Base_Start_IT(htim);
	HAL_TIM_PWM_Start_IT(htim, TIM_CHANNEL_1);
//	uint8_t data[528];
//
//	for (uint32_t i = 0; i < song->GetSize(); i++)
//	{
//		song->GetNextBlock(data);
//
//		for (uint32_t j = 0; j < 528; j++)
//		{
//			while (!periodElapsed) {}
//			periodElapsed = false;
//
//			htim->Instance->CCR1 = data[j];
//		}
//	}
}
