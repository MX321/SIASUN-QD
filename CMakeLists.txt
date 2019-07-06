cmake_minimum_required(VERSION 3.12)
project(IMUtest)

set(BUILD_DIR .)
message(${BUILD_DIR})

set(CMAKE_C_STANDARD 99)

include_directories(./Timer)
include_directories(./)
include_directories(./App)
include_directories(./CAN)
include_directories(./Math)
include_directories(./MPI204A)
include_directories(./Odom_Calib)
include_directories(./PGV150)
include_directories(./UART)




add_executable(IMUtest main.c ./PGV150/PGV150.c ./UART/UART.c ./CAN/can.c ./Math/Trigonometric.c ./App/Interpolation.c ./App/Movement.c ./MPI204A/MPI204A.c ./Odom_Calib/Odom_Calib.c ./Timer/Timer.c TCP/TCP.c TCP/TCP.h)

target_link_libraries(IMUtest pthread m rt)

#add_custom_target(IMUtest COMMAND make WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})