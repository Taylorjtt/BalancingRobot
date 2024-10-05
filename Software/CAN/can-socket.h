//
// Created by John on 2/18/2023.
//

#ifndef BALANCINGROBOT_CAN_SOCKET_H
#define BALANCINGROBOT_CAN_SOCKET_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <stdint.h>

int can_socket;

void CAN_init();

int read_can_socket(const char* interface);
int CAN_sendMessage( int can_id,  int data_len,unsigned char* data);
void buffer_append_int32(uint8_t* buffer, int32_t number, int32_t *index);
void buffer_append_int16(uint8_t* buffer, int16_t number, int16_t *index);

#endif //BALANCINGROBOT_CAN_SOCKET_H
