EXE = main
SRC = main.c ./CAN/can.c ./MPI204A/MPI204A.c ./Timer/Timer.c ./PGV150/PGV150.c ./UART/UART.c ./Odom_Calib/Odom_Calib.c ./Math/Trigonometric.c\
	 ./App/Interpolation.c ./App/Movement.c


$(EXE):
	$(CC) -I"/home/siasunhebo/workspace/IMUtest/" -I"/home/siasunhebo/workspace/IMUtest/App" -I"/home/siasunhebo/workspace/IMUtest/CAN/" -I"/home/siasunhebo/workspace/IMUtest/Math/" -I"/home/siasunhebo/workspace/IMUtest/Odom_Calib/" -I"/home/siasunhebo/workspace/IMUtest/MPI204A" -I"/home/siasunhebo/workspace/IMUtest/Timer" -I"/home/siasunhebo/workspace/IMUtest/UART" -I"/home/siasunhebo/workspace/IMUtest/PGV150" -include"/home/siasunhebo/workspace/IMUtest/main.h" -include"/home/siasunhebo/workspace/IMUtest/CAN/can.h" -include"/home/siasunhebo/workspace/IMUtest/Odom_Calib/Odom_Calib.h" -include"/home/siasunhebo/workspace/IMUtest/App/Interpolation.h" -include"/home/siasunhebo/workspace/IMUtest/Math/Trigonometric.h" -include"/home/siasunhebo/workspace/IMUtest/UART/UART.h" -include"/home/siasunhebo/workspace/IMUtest/MPI204A/MPI204A.h" -include"/home/siasunhebo/workspace/IMUtest/Timer/Timer.h" -include"/home/siasunhebo/workspace/IMUtest/PGV150/PGV150.h" $(SRC) -o $(EXE) -g -lpthread -lrt -lm 

.PHONY:all clean
clean:
	-rm main