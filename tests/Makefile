# Compiler and Flags
CC = gcc
CFLAGS = -I../external/unity/src -I../modules/median -I./mocks -DUNIT_TEST
SRC = ../external/unity/src/unity.c ../modules/median/median.c src/test_median.c

# OS Detection
ifeq ($(OS),Windows_NT)
    TARGET = test_median.exe
    RM = del /F /Q
    EXEC = $(TARGET)
    FIX_PATH = $(subst /,\,$1)
else
    TARGET = test_median
    RM = rm -f
    EXEC = ./$(TARGET)
    FIX_PATH = $1
endif

all: $(TARGET)
	$(EXEC)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	$(RM) $(TARGET)
