CC = gcc
CFLAGS = -g -Wall -Wextra -Werror
TARGET = hinfosvc
FILES = hinfosvc.cpp
all: $(TARGET)

$(TARGET): $(FILES)
	$(CC) $(CFLAGS) $(FILES) -o $(TARGET)

clean:
	rm -f $(TARGET)