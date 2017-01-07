TARGET = sender agent receiver
CFLAGS = -O2 -Wall

%: %.c
	$(CC) $(CFLAGS) $< -o $@

all: $(TARGET)

clean:
	$(RM) $(TARGET)