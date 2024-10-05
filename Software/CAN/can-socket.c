#include "can-socket.h"

int CAN_sendMessage( int can_id,  int data_len,unsigned char* data)
{
    struct can_frame frame;
    struct sockaddr_can addr;

    // Set up the CAN frame
    frame.can_id = can_id;
    frame.can_dlc = data_len;
    memcpy(frame.data, data, data_len);

    // Set up the CAN socket address
    addr.can_family = AF_CAN;
    addr.can_ifindex = if_nametoindex("can0");

    // Bind the CAN socket to the address
    if (bind(can_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }

    // Send the CAN message
    if (write(can_socket, &frame, sizeof(frame)) < 0) {
        perror("write");
        return -1;
    }

    return 0;
}

void CAN_init()
{
    can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    struct ifreq ifr;
    strcpy(ifr.ifr_name, "can0");
    ioctl(can_socket, SIOCGIFINDEX, &ifr);
    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(can_socket, (struct sockaddr *)&addr, sizeof(addr));
}

void buffer_append_int32(uint8_t* buffer, int32_t number, int32_t *index) {
    buffer[(*index)++] = number >> 24;
    buffer[(*index)++] = number >> 16;
    buffer[(*index)++] = number >> 8;
    buffer[(*index)++] = number;
}
void buffer_append_int16(uint8_t* buffer, int16_t number, int16_t *index) {
    buffer[(*index)++] = number >> 8;
    buffer[(*index)++] = number;
}

int read_can_socket(const char* interface) {
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    // Open a socket for the specified interface
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("socket");
        return -1;
    }

    strcpy(ifr.ifr_name, interface);
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }

    // Read CAN frames from the socket
    while (1) {
        int nbytes = read(s, &frame, sizeof(frame));
        if (nbytes < 0) {
            perror("read");
            return -1;
        }
        if(frame.can_id == 0x80002901)
        {
            int16_t pos_int = frame.data[0] << 8 | frame.data[1];
            int16_t spd_int = frame.data[2] << 8 | frame.data[3];
            int16_t cur_int = frame.data[4] << 8 | frame.data[5];

            float motor_pos= (float)( pos_int * 0.1f); //motor position
            float motor_spd= (float)( spd_int * 10.0f);//motor speed
            float motor_cur= (float) ( cur_int * 0.01f);//motor current
            int16_t motor_temp= frame.data[6] ;//motor temperature
            int16_t motor_error= frame.data[7] ;//motor error mode

            printf("Received CAN frame: ID=%X, POS=%f, SPD=%f,CUR=%f,TMP=%d,ERR=%d\n", frame.can_id,motor_pos,motor_spd,motor_cur,motor_temp,motor_error );
        }

    }

    return 0;
}