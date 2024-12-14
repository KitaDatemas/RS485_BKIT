/*
 * modbus.c
 *
 *  Created on: Dec 13, 2024
 *      Author: ASUS
 */

#include "modbus.h"

uint8_t Modbus_Address;
uint8_t no_ports;
UART_HandleTypeDef ** Port;
enum Modbus_Role ModbusRole;

uint8_t ** buffer;
uint32_t * buffer_len;
uint8_t * buffer_flag;

uint16_t no_regs;
uint16_t * Register;




void Modbus_Init (enum Modbus_Role role,UART_HandleTypeDef * Modbus_Uart, uint8_t address, uint32_t no_modbus_ports, uint32_t no_modbus_regs) {
	ModbusRole = role;
	Modbus_Address = address;

	Port = malloc (no_modbus_ports * sizeof(UART_HandleTypeDef *));

	buffer = (uint8_t **)malloc (no_modbus_ports * sizeof(uint8_t *));
	buffer_len = malloc (no_modbus_ports * sizeof(uint32_t));
	buffer_flag = malloc (no_modbus_ports * sizeof(uint8_t));

	Register = malloc (no_modbus_regs * sizeof(uint16_t));

	for (int idx = 0; idx < no_modbus_ports; idx++){
		buffer_flag[idx] = 0;
		Port[idx] = NULL;
	}
	Port[3] = Modbus_Uart;

	for (int idx = 0; idx < no_modbus_regs; idx++) {
		Register[idx] = idx + 1;
	}

	no_ports = no_modbus_ports;
	no_regs = no_modbus_regs;
}

enum Modbus_Status Modbus_Transmit (UART_HandleTypeDef * Modbus_Uart, uint8_t address, uint8_t opcode, uint8_t *data, uint32_t size) {
	uint8_t * sendData = malloc((4 + size) * sizeof(uint8_t));

	//Assign data to dataframe
	sendData[0] = address;
	sendData[1] = opcode;
	for (int idx = 0; idx < size; idx++) {
		sendData[idx + 2] = data[idx];
	}

	//CRC Generate
	uint16_t crcVal = crc16(sendData, size + 2);
	sendData[size + 3] = crcVal & 0xFF;//Low CRC bits
	sendData[size + 2] = (crcVal >> 8) & 0xFF;//High CRC bits

	HAL_UART_Transmit(Modbus_Uart, sendData, size + 4, 1000);

	free (sendData);
	return MODBUS_NORMAL;
}

enum Modbus_Status Modbus_Receive (UART_HandleTypeDef * Modbus_Uart, uint8_t * buff, uint32_t size) {
	if (HAL_UARTEx_ReceiveToIdle_IT(Modbus_Uart, buff, size) == HAL_OK) {
		if (Modbus_Uart->Instance == USART1) {
			buffer[0] = buff;
			buffer_len[0] = size;
			Port[0] = Modbus_Uart;
		}
		else if (Modbus_Uart->Instance == USART2) {
			buffer[1] = buff;
			buffer_len[1] = size;
			Port[1] = Modbus_Uart;
		}
		else if (Modbus_Uart->Instance == USART3) {
			buffer[2] = buff;
			buffer_len[2] = size;
			Port[2] = Modbus_Uart;
		}
		return MODBUS_NORMAL;
	}
	return MODBUS_ERROR;
}

enum Modbus_Status Modbus_Receive_Callback (UART_HandleTypeDef * Modbus_Uart) {
	uint8_t * checkBuffer;
	uint32_t bufferLen;
	uint8_t * flag;

	if (Modbus_Uart->Instance == USART1) {
		checkBuffer = buffer[0];
		bufferLen = buffer_len[0];
		flag = &buffer_flag[0];
		Port[0] = Modbus_Uart;
	}
	else if (Modbus_Uart->Instance == USART2) {
		checkBuffer = buffer[1];
		bufferLen = buffer_len[1];
		flag = &buffer_flag[1];
		Port[1] = Modbus_Uart;
	}
	else if (Modbus_Uart->Instance == USART3) {
		checkBuffer = buffer[2];
		bufferLen = buffer_len[2];
		flag = &buffer_flag[2];
		Port[2] = Modbus_Uart;
	}
	uint16_t crc = crc16(checkBuffer, bufferLen - 2);
	if ((checkBuffer[bufferLen - 1] != (crc & 0xFF))
	 || (checkBuffer[bufferLen - 2] != (crc >> 8))){
		Modbus_Receive(Modbus_Uart, checkBuffer, bufferLen);
		return MODBUS_ERROR;
	}

	*flag = 1;
	return MODBUS_NORMAL;
}

void Modbus_Process() {
	 if (ModbusRole == MODBUS_MASTER) {
//	        for (int idx = 0; idx < no_ports; idx++) {
	            if (buffer_flag[0]) {
	                int retries = 0; // Bộ đếm số lần gửi lại
	                bool response_received = false;
//	                HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);
	                do {

	                    // Gửi yêu cầu tới Slave
	                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);
	                    Modbus_Transmit(Port[3], buffer[0][0], buffer[0][1], &buffer[0][2], buffer_len[0] - 4);
	                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 0);
	                    uint16_t no = ((buffer[0][4] << 8) | buffer[0][5]);
	                    uint8_t* buffer_temp = malloc(sizeof (uint8_t)*(no + 5));
	                    Modbus_Receive(Port[3], buffer_temp, no+5);
	                    //Bắt đầu chờ phản hồi
	                    uint32_t start_time = HAL_GetTick();
	                    while ((HAL_GetTick() - start_time) < RESPONSE_TIMEOUT) {// sua lai timeout
	                        if (buffer_flag[3]) {
	                        	buffer_flag[3]=0;
	                            uint16_t crc = crc16(buffer[3], buffer_len[3] - 2);
	                            // Kiểm tra CRC
	                            if ((buffer[3][buffer_len[3] - 1] == (crc & 0xFF)) &&
	                                (buffer[3][buffer_len[3] - 2] == (crc >> 8))) {

	                                response_received = true; // Phản hồi hợp lệ

	                                break;
	                            }
	                        }
	                    }

	                    if (!response_received) {
	                        retries++;
	                        if( retries > MAX_RETRIES)
	              	         {
	              	           HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_15);

	              	         }

	                    }
	                    free (buffer_temp);
	                }
	                while (response_received && retries < MAX_RETRIES);

	                if (response_received && retries < MAX_RETRIES) {

	                    // Xử lý phản hồi nhận được
	                    uint8_t function_code = buffer[3][1];
	                    if (function_code == READ_REGISTER) {
	                        uint8_t byte_count = buffer[3][2];
	                        for (int i = 0; i < byte_count / 2; i++) {
	                            uint16_t reg_value = (buffer[3][3 + 2 * i] << 8) | buffer[3][4 + 2 * i];
	                            // Xử lý giá trị của register
	                        }
	                    } else if (function_code & 0x80) {

	                    }
	                }
	                buffer_flag[0] = 0; // Đặt lại cờ cho buffer
	            }
//        }

    } else {
        for (int idx = 0; idx < no_ports; idx++) {
            if (buffer_flag[idx]) {
                if (validateAddress(idx)) {
                    uint16_t start_reg;
                    uint8_t no_regs;
                    uint16_t reg;
                    uint16_t data;

                    switch (buffer[idx][1]) {
                    case READ_REGISTER:
                        start_reg = (buffer[idx][2] << 8) | buffer[idx][3];
                        no_regs = (buffer[idx][4] << 8) | buffer[idx][5];
                        readRegister(idx, start_reg, no_regs);
                        break;
                    case WRITE_REGISTER:
                        reg = ((buffer[idx][2] << 8) | buffer[idx][3]);
                        data = ((buffer[idx][4] << 8) | buffer[idx][5]);  // Khai báo và gán
                        writeRegister(reg, data);
                        break;
                    default:
                        break;
                    }
                }
                buffer_flag[idx] = 0;
                Modbus_Receive(Port[idx], buffer[idx], buffer_len[idx]);
            }
        }
    }
}


void readRegister (int Modbus_Port, uint16_t start_reg, uint16_t no_regs) {
	uint8_t * frame = malloc (1 + no_regs * 2 * sizeof(uint8_t));
	frame[0] = no_regs * 2;

	for (int idx = start_reg, frameIdx = 1; idx < start_reg + no_regs; idx++, frameIdx += 2) {
		frame[frameIdx] = Register[idx] >> 8;
		frame[frameIdx + 1] = Register[idx] & 0xFF;
	}
	Modbus_Transmit(Port[Modbus_Port], buffer[Modbus_Port][0], buffer[Modbus_Port][1], frame, 1 + no_regs * 2);
	free (frame);
}

void writeRegister (uint16_t reg, uint16_t data) {
	if (reg < 0 || reg >= no_regs)		return;
	Register[reg] = data;
}

uint8_t validateAddress (uint8_t buffer_idx) {
	if (buffer[buffer_idx][0] != Modbus_Address)
		return 0;
	return 1;
}
