cmake_minimum_required(VERSION 3.24)


set(CMAKE_SYSTEM_NAME Linux)



#Tool Locations
set(stretch_tools C:/SysGCC/raspberry_stretch) #Location for Raspbian Buster tools (PI 3)

set(CMAKE_LIBRARY_ARCHITECTURE arm-linux-gnueabihf)
set(BUSTER_REMOTE_LIBS "${CMAKE_SOURCE_DIR}/philadelphia-libraries/RemoteLibraries_buster/")
set(BUSTER_REMOTE_HEADERS "${CMAKE_SOURCE_DIR}/philadelphia-libraries/RemoteHeaders_buster/")
set(BULLSEYE_REMOTE_LIBS "${CMAKE_SOURCE_DIR}/philadelphia-libraries/RemoteLibraries/")
set(BULLSEYE_REMOTE_HEADERS "${CMAKE_SOURCE_DIR}/philadelphia-libraries/RemoteHeaders/")

message(STATUS "COMPILING FOR RASPIAN STRETCH")
set(tools ${stretch_tools})

set(FUSION_FOLDER ${CMAKE_SOURCE_DIR}/Fusion)
set(CAN_FOLDER ${CMAKE_SOURCE_DIR}/CAN)
set(IMU_FOLDER ${CMAKE_SOURCE_DIR}/IMU)
set(TMOTOR_FOLDER ${CMAKE_SOURCE_DIR}/TMotor)


# specify cross compilers and tools
set(CMAKE_C_COMPILER ${tools}/bin/arm-linux-gnueabihf-gcc.exe)
set(CMAKE_CXX_COMPILER ${tools}/bin/arm-linux-gnueabihf-g++.exe)
set(CMAKE_ASM_COMPILER ${tools}/bin/arm-linux-gnueabihf-gcc.exe)
set(CMAKE_AR  ${tools}/bin/arm-linux-gnueabihf-ar.exe)
set(CMAKE_OBJCOPY  ${tools}/bin/arm-linux-gnueabihf-objcopy.exe)
set(CMAKE_OBJDUMP  ${tools}/bin/arm-linux-gnueabihf-objdump.exe)
set(SIZE  ${tools}/bin/arm-linux-gnueabihf-size.exe)
set(CMAKE_VERBOSE_MAKEFILE ON)

#File Locations
file(GLOB_RECURSE FUSION  "${FUSION_FOLDER}/*.h" "${FUSION_FOLDER}/*.c")
file(GLOB_RECURSE CAN  "${CAN_FOLDER}/*.h" "${CAN_FOLDER}/*.c")
file(GLOB_RECURSE IMU  "${IMU_FOLDER}/*.h" "${IMU_FOLDER}/*.c")
file(GLOB_RECURSE TMOTOR  "${TMOTOR_FOLDER}/*.h" "${TMOTOR_FOLDER}/*.c")

project(BalancingRobot C CXX)
add_compile_options(-O0)
add_compile_options(-fabi-version=0)
add_compile_options(-std=gnu++11)
add_compile_options(-fmessage-length=0)
add_compile_options(-Wall)
add_compile_options(-g3)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)




add_executable(bot main.c rpi.c rpi.h common.c ${FUSION} ${CAN} ${IMU} ${TMOTOR} )
target_link_libraries(bot m)