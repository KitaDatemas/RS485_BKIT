#include "modbus.h"

uint8_t Modbus_Address;

void Modbus_Init (uint8_t address) {
	Modbus_Address = address;
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
	sendData[size + 2] = crcVal & 0xFF;//Low CRC bits
	sendData[size + 3] = (crcVal >> 8) & 0xFF;//High CRC bits

	HAL_UART_Transmit(Modbus_Uart, sendData, size + 4, 1000);

	free (sendData);
	return MODBUS_OK;
}

enum Modbus_Status Modbus_Receive (UART_HandleTypeDef * Modbus_Uart, uint8_t * buff, uint32_t size) {
	if (HAL_UARTEx_ReceiveToIdle_IT(Modbus_Uart, buff, size) == HAL_OK)
		return MODBUS_OK;
	return MODBUS_ERROR;
}
