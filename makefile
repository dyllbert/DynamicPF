# SOURCES := $(wildcard *.cpp)
# OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
# CXX := g++ -Wall -Wvla -g

# .cpp.o:
#         ${CXX} -c $<

CC = g++

CFLAGS = -wall -g

src = $(wildcard *.cpp)
obj = $(patsubst %.cpp,%.o,$(SOURCES))

dynamicpf: $(obj)
	$(CC) $(CFLAGS) -o $@ $^

.cpp.o:
	$(CC) -c $<

clean:
	rm -f $(obj) dynamicpf

