/*
 * crc.h
 *
 *  Created on: Dec 13, 2024
 *      Author: ASUS
 */

#ifndef INC_CRC_H_
#define INC_CRC_H_

#include <stdint.h>
#include <stdlib.h>

extern uint16_t crc16_lookup_table[256];

uint16_t crc16(const uint8_t *data, uint16_t num_bytes);

#endif /* INC_CRC_H_ */
