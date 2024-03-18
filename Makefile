CC = cl
CFLAGS = /W4 /DEBUG
LDFLAGS = /link /LIBPATH:. libcrc.lib
#TARGET = main
DEPS = checksum.h
SRCS = sample-cpp-vcp.cpp

main: $(DEPS)
	$(CC) $(CFLAGS) /Fe$(TARGET) $(SRCS) $(LDFLAGS)

clean:
	del $(TARGET).exe
