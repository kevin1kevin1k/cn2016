TARGET = sender agent receiver
CFLAGS = -O2 -Wall

%: %.c UDP.h
	$(CC) $(CFLAGS) $< -o $@

all: $(TARGET)

clean:
	$(RM) $(TARGET)