/*
* @file TMotor.c
* @brief 
* @author John Taylor
* @date 23-Jul-2022 (14:33:25.442719)
*/


#include "TMotor.h"
TMotorHandle TMotor_Constructor(uint16_t id)
{
	TMotorHandle handle = malloc(sizeof(TMotor));
	handle->id = id;
	handle->position = 0.0;
	handle->speed = 0.0;
	handle->current = 0.0;
	handle->temperature = 0.0;
	handle->error = 0;
	return handle;
}

void TMotor_enable(TMotorHandle handle)
{
	uint8_t enable_data[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC};
    CAN_sendMessage(handle->id, 8,
    		enable_data);
}

void TMotor_zero(TMotorHandle handle)
{
    uint8_t enable_data[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE};
    CAN_sendMessage( handle->id, 8,
    		enable_data);
}
void TMotor_readState(TMotorHandle handle)
{
    uint8_t enable_data[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC};
    CAN_sendMessage( handle->id, 8,
    		enable_data);
}
void TMotor_Destructor(TMotorHandle handle)
{
	free(handle);
}
float TMotor_getPosition(TMotorHandle handle)
{
	return handle->position;
}
float TMotor_getSpeed(TMotorHandle handle)
{
	return handle->speed;
}
float TMotor_getCurrent(TMotorHandle handle)
{
	return handle->current;
}
float TMotor_getTemperature(TMotorHandle handle)
{
	return handle->temperature;
}
uint16_t TMotor_getError(TMotorHandle handle)
{
	return handle->error;
}
void TMotor_setPosition(TMotorHandle handle, float position)
{
	handle->position = position;
}
void TMotor_setSpeed(TMotorHandle handle, float speed)
{
	handle->speed = speed;
}
void TMotor_setCurrent(TMotorHandle handle, float current)
{
	handle->current = current;
}
void TMotor_setDesiredCurrent(TMotorHandle handle, float current)
{
    int32_t send_index = 0;
    uint8_t buffer[4];
    buffer_append_int32(buffer, (int32_t)(current * 1000.0), &send_index);
    CAN_sendMessage(handle->id |
                          ((uint32_t)CAN_PACKET_SET_CURRENT << 8),send_index,buffer);
}


void TMotor_setTemperature(TMotorHandle handle, float temperature)
{
	handle->temperature = temperature;
}
void TMotor_setError(TMotorHandle handle, uint16_t error)
{
	handle->error = error;
}
