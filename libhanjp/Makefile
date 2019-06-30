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
	$(CC) -g *.c `pkg-config --cflags --libs libhangul` `pkg-config --cflags --libs glib-2.0`

install:
	cp libhanjp.pc /usr/lib/x86_64-linux-gnu/pkgconfig
	cp libhanjp.a /usr/lib/x84_64-linux-gnu
	cp hanjp.h /usr/include/hanjp-0.1
	cp ./keyboard/* /usr/share/libhangul/keyboards