#include <stdio.h>
#include "CAN/can-socket.h"
#include "TMotor/TMotor.h"

#include "IMU/Adafruit_Sensor.h"
#include "IMU/Adafruit_LSM303_U.h"
#include "Fusion/Fusion.h"
#include <time.h>
int main(int argc, char *argv[]) {
//    // Check that two arguments were provided
//    if (argc != 4) {
//        printf("Usage: %s current seconds\n", argv[0]);
//        return 1;
//    }
//
//    // Parse the arguments as floats
//    int id = atoi(argv[1]);
//    float current = atof(argv[2]);
//    float seconds = atof(argv[3]);
//
//
//    // Print the values to the screen
//    printf("ID: %d\nCurrent: %f\nSeconds: %f\n",id, current, seconds);
//    useconds_t usec = (useconds_t)(seconds * 1000000);
//
//    CAN_init();
//    TMotorHandle motor = TMotor_Constructor(id);
//    TMotor_setDesiredCurrent(motor,current);
//    usleep(usec);
//    TMotor_setDesiredCurrent(motor,0);
//    TMotor_Destructor(motor);
    FusionAhrs ahrs;
    FusionAhrsInitialise(&ahrs);

    struct accel_t *accel;
    sensor_t sensor;
    printf("Accelerometer Test\n");

    /* Initialise the sensor */
    if(!accel_create(&accel, 54321)) {
        printf("No LSM303 detected!");
        return -1;
    }

    // Display sensor information
    accel_getSensor(accel,&sensor);
    printf("------------------------------------\n");
    printf("Sensor:       %s\n", sensor.name);
    printf("Driver Ver:   %i\n", sensor.version);
    printf("Unique ID:    %i\n", sensor.sensor_id);
    printf("Max Value:    %f m/s^2\n", sensor.max_value);
    printf("Min Value:    %f m/s^2\n", sensor.max_value);
    printf("Resolution:   %f m/s^2\n", sensor.resolution);
    printf("------------------------------------\n\n\n");
    sleep(2);

    for (;;) {
        /* Get a new sensor event */
        sensors_event_t event;
        accel_getEvent(accel, &event);
        printf("X: % 010.6f   ", event.acceleration.x);
        printf("Y: % 010.6f   ", event.acceleration.y);
        printf("Z: % 010.6f   ", event.acceleration.z);
        printf(" m/s^2 \n");
        usleep(100000);
    }



    return 0;


}