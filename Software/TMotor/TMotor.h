/*
* @file TMotor.h
* @brief 
* @author John Taylor
* @date 23-Jul-2022 (14:33:25.442719)
*/


#ifndef TMOTOR_H
#define TMOTOR_H

#include "stdlib.h"
#include <stdint.h>
#include "../CAN/can-socket.h"

typedef enum {
	CAN_PACKET_SET_DUTY = 0, //Duty cycle mode
	CAN_PACKET_SET_CURRENT = 1, //Current loop mode
	CAN_PACKET_SET_CURRENT_BRAKE = 2, // Current brake mode
	CAN_PACKET_SET_RPM = 3, //Velocity mode
	CAN_PACKET_SET_POS = 4, // Position mode
	CAN_PACKET_SET_ORIGIN_HERE = 5, //Set origin mode
	CAN_PACKET_SET_POS_SPD = 6, //Position velocity loop mode
} CAN_PACKET_ID;

typedef struct TMotor
{
	uint16_t id;
	float position;
	float speed;
	float current;
	float temperature;
    float desiredCurrent;
	uint16_t error;
}TMotor;

typedef struct TMotor *TMotorHandle;


/*
* @brief TMotor_Constructor
* @return TMotorHandle 
*/
TMotorHandle TMotor_Constructor(uint16_t id);

/*
* @brief TMotor_Destructor
* @return void 
*/
void TMotor_Destructor(TMotorHandle handle);

/*
* @brief enable motor
* @return void
*/
void TMotor_enable(TMotorHandle handle);

void TMotor_zero(TMotorHandle handle);
void TMotor_readState(TMotorHandle handle);

/*
* @brief TMotor_getPosition
* @param TMotorHandle handle 
* @return float 
*/
float TMotor_getPosition(TMotorHandle handle);

/*
* @brief TMotor_getSpeed
* @param TMotorHandle handle 
* @return float 
*/
float TMotor_getSpeed(TMotorHandle handle);

/*
* @brief TMotor_getCurrent
* @param TMotorHandle handle 
* @return float 
*/
float TMotor_getCurrent(TMotorHandle handle);

/*
* @brief TMotor_getTemperature
* @param TMotorHandle handle 
* @return float 
*/
float TMotor_getTemperature(TMotorHandle handle);

/*
* @brief TMotor_getError
* @param TMotorHandle handle 
* @return uint16_t
*/
uint16_t TMotor_getError(TMotorHandle handle);

/*
* @brief TMotor_setPosition
* @param TMotorHandle handle 
* @param float position 
* @return void
*/
void TMotor_setPosition(TMotorHandle handle, float position);

/*
* @brief TMotor_setSpeed
* @param TMotorHandle handle 
* @param float speed 
* @return void
*/
void TMotor_setSpeed(TMotorHandle handle, float speed);

/*
* @brief TMotor_setCurrent
* @param TMotorHandle handle 
* @param float current 
* @return void
*/
void TMotor_setCurrent(TMotorHandle handle, float current);

/*
* @brief TMotor_setCurrent
* @param TMotorHandle handle
* @param float current
* @return void
*/
void TMotor_setDesiredCurrent(TMotorHandle handle, float current);

/*
* @brief TMotor_setTemperature
* @param TMotorHandle handle 
* @param float temperature 
* @return void
*/
void TMotor_setTemperature(TMotorHandle handle, float temperature);

/*
* @brief TMotor_setError
* @param TMotorHandle handle 
* @param uint16_t error
* @return void
*/
void TMotor_setError(TMotorHandle handle, uint16_t error);

#endif
