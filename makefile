CC = gcc

LDFLAGS = -lncurses

TARGET = cless.out

SRCS = src/main.c src/menu.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(LDFLAGS)

run:
	$(CC) -o $(TARGET) $(SRCS) $(LDFLAGS) && ./$(TARGET)

clean:
	rm -f $(TARGET)

