CC = g++

CFLAGS = -Wall -Wextra -Werror -g 

# src = $(wildcard *.cpp)
# obj = $(patsubst %.cpp,%.o,$(SOURCES))

# dynamicpf: $(obj)
# 	$(CC) $(CFLAGS) -o $@ $^

# .cpp.o:
# 	$(CC) -c $<

# clean:
# 	rm -f $(obj) dynamicpf

src = $(wildcard *.cpp)
obj = $(patsubst %.cpp,%.o,$(SOURCES))

dynamicpf: main.o DynamicOccupancyGrid.o DynamicOccupancyGridMap.o FileManager.o History.o RobotState.o LaserZ.o ControlU.o
	$(CC) $(CFLAGS) -o dynamicpf main.o DynamicOccupancyGrid.o DynamicOccupancyGridMap.o FileManager.o History.o RobotState.o LaserZ.o ControlU.o

main.o: main.cpp DynamicOccupancyGrid.h DynamicOccupancyGridMap.h FileManager.h History.h RobotState.h LaserZ.h ControlU.h
	$(CC) $(CFLAGS) -c main.cpp

LaserZ.o: LaserZ.h

DynamicOccupancyGrid.o: DynamicOccupancyGrid.h

DynamicOccupancyGridMap.o: DynamicOccupancyGridMap.h

FileManager.o: FileManager.h

History.o: History.h

RobotState.o: RobotState.h

ControlU.o: ControlU.h

clean:
	rm -f *.o dynamicpf
