CXX = g++

LDFLAGS = -lncurses

TARGET = cless.out

SRCS = src/main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) -o $(TARGET) $(SRCS) $(LDFLAGS)

run:
	$(CXX) -o $(TARGET) $(SRCS) $(LDFLAGS) && ./$(TARGET)

clean:
	rm -f $(TARGET)

