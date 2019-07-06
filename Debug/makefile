EXE = main
SRC = main.c ./CAN/can.c ./MPI204A/MPI204A.h ./Timer/Timer.h


$(EXE):
	$(CC) -I"/home/siasunhebo/workspace/IMUtest/" -I"/home/siasunhebo/workspace/IMUtest/CAN/" -I"/home/siasunhebo/workspace/IMUtest/MPI204A" -I"/home/siasunhebo/workspace/IMUtest/Timer" -include"/home/siasunhebo/workspace/IMUtest/main.h" -include"/home/siasunhebo/workspace/IMUtest/CAN/can.h" -include"/home/siasunhebo/workspace/IMUtest/MPI204A/MPI204A.h" -include"/home/siasunhebo/workspace/IMUtest/Timer/Timer.h" $(SRC) -o $(EXE) -g -lpthread 

.PHONY:all clean
clean:
	-rm main