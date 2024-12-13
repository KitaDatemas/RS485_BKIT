/*
 * modbus.h
 *
 *  Created on: Dec 12, 2024
 *      Author: ASUS
 */

#ifndef INC_MODBUS_H_
#define INC_MODBUS_H_
#include "main.h"
#include "crc.h"
#include "button.h"
#include <stdbool.h>



#define RESPONSE_TIMEOUT 1000  // Thời gian timeout (ms)
#define MAX_RETRIES 3         // Số lần thử lại tối đa


#define READ_REGISTER 0x03
#define WRITE_REGISTER 0x06

enum Modbus_Role {MODBUS_MASTER, MODBUS_SLAVE};
enum Modbus_Status {MODBUS_NORMAL, MODBUS_BUSY, MODBUS_ERROR};

extern uint8_t Modbus_Address;
extern uint8_t no_ports;
extern UART_HandleTypeDef ** Port;
extern enum Modbus_Role ModbusRole;

extern uint8_t ** buffer;
extern uint32_t * buffer_len;
extern uint8_t * buffer_flag;
extern uint16_t no_regs;
extern uint16_t * Register;

void Modbus_Init (enum Modbus_Role role, uint8_t address, uint32_t no_modbus_ports, uint32_t no_modbus_regs);
enum Modbus_Status Modbus_Transmit (UART_HandleTypeDef * Modbus_Uart, uint8_t address, uint8_t opcode, uint8_t *data, uint32_t size);
enum Modbus_Status Modbus_Receive (UART_HandleTypeDef * Modbus_Uart, uint8_t * buff, uint32_t size);
enum Modbus_Status Modbus_Receive_Callback (UART_HandleTypeDef * Modbus_Uart);
void Modbus_Process();
void readRegister (int Modbus_Port, uint16_t start_reg, uint16_t no_regs);
void writeRegister (uint16_t reg, uint16_t data);
uint8_t validateAddress (uint8_t buffer_idx);

#endif /* INC_MODBUS_H_ */
