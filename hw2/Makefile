TARGET = sender agent receiver

%: %.c UDP.h
	$(CC) $(CFLAGS) $< -o $@

all: $(TARGET)

clean:
	$(RM) $(TARGET)