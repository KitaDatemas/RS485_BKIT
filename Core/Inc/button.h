/*
 * button.h
 *
 *  Created on: Dec 13, 2024
 *      Author: ASUS
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

/* Includes */
#include <stdint.h>
#include "main.h"
#define TI_BUTTON_TIME 10
/* Variables */
extern uint16_t button_count[16];
extern SPI_HandleTypeDef * spi;
/* Functions */
extern void button_init(SPI_HandleTypeDef * hspi);
extern void button_scan();

extern uint8_t isPress(uint8_t index);
extern uint8_t isLongPress(uint8_t index);
extern void resetButton(uint8_t index);

#endif /* INC_BUTTON_H_ */
