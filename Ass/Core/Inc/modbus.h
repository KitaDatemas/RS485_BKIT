/*
 * modbus.h
 *
 *  Created on: Nov 30, 2024
 *      Author: ASUS
 */

#ifndef INC_MODBUS_H_
#define INC_MODBUS_H_
#include "stdlib.h"
#include "main.h"
#include "crc16.h"

extern uint8_t Modbus_Address;

enum Modbus_Status {MODBUS_OK, MODBUS_ERROR};

void Modbus_Init (uint8_t address);

enum Modbus_Status Modbus_Transmit (UART_HandleTypeDef * Modbus_Uart, uint8_t address, uint8_t opcode, uint8_t *data, uint32_t size);

enum Modbus_Status Modbus_Receive (UART_HandleTypeDef * Modbus_Uart, uint8_t * buff, uint32_t size);
#endif /* INC_MODBUS_H_ */
