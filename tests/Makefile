CC = gcc
CFLAGS = -I../external/unity/src -I../modules/median -I./mocks -DUNIT_TEST
SRC = ../external/unity/src/unity.c ../modules/median/median.c src/test_median.c
TARGET = test_median.exe

all: $(TARGET)
	./$(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
