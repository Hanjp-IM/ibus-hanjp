CC = gcc
AR = ar
TARGET = libhanjp.a
SRC = *.c
OBJS = *.o
MAKEOPTION = rscv

all: $(TARGET)
	$(AR) $(MAKEOPTION) $(TARGET) $(OBJS)

$(TARGET): $(OBJS)

$(OBJS): $(SRC)
	$(CC) -c $(SRC) `pkg-config --cflags --libs libhangul`

clean:
	rm -f *.o *.a