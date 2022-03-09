CC = g++
CFLAGS = -g -Wall -Wextra -Werror -pedantic
TARGET = hinfosvc
FILES = hinfosvc.cpp
all: $(TARGET)

$(TARGET): $(FILES)
	$(CC) $(CFLAGS) $(FILES) -o $(TARGET)

clean:
	rm -f $(TARGET)