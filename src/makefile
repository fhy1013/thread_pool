CC=g++

INCLUDE = -I.

CFLAGS = -Wall --std=c++11 -g

LIBFLAGS = -lpthread  -L/usr/local/include -lgtest

SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SRCS))

TARGET = run

ALL:$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBFLAGS) $(CFLAGS)

%.o:%.cpp
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
