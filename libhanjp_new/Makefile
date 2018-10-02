CC = gcc
AR = ar
TARGET = libhanjp.a
SRC = hanjp*.c
OBJS = *.o
MAKEOPTION = rscv

all: $(TARGET)
	$(AR) $(MAKEOPTION) $(TARGET) $(OBJS)

$(TARGET): $(OBJS)

$(OBJS): $(SRC)
	$(CC) -c $(SRC) `pkg-config --cflags --libs libhangul`

clean:
	rm -f *.o *.a a.out

test: *.c
	$(CC) -g *.c `pkg-config --cflags --libs libhangul`