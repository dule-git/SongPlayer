/*
 * FlashProgramator.h
 *
 *  Created on: Nov 11, 2021
 *      Author: dusan
 */

#ifndef SRC_FLASHPROGRAMATOR_H_
#define SRC_FLASHPROGRAMATOR_H_

class AT45DB161D;

class FlashProgramator {
public:
	FlashProgramator();
	virtual ~FlashProgramator();
	void Program(AT45DB161D* flash, UART_HandleTypeDef* huart);
};

#endif /* SRC_FLASHPROGRAMATOR_H_ */
