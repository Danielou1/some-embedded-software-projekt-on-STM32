# Compiler and Flags
CC = gcc
CFLAGS = -I../external/unity/src -I../modules -I./mocks -DUNIT_TEST

# Common sources
UNITY_SRC = ../external/unity/src/unity.c

# OS Detection
ifeq ($(OS),Windows_NT)
    EXT = .exe
    RM = del /F /Q
else
    EXT = 
    RM = rm -f
endif

# Targets
TARGET_MEDIAN = test_median$(EXT)
TARGET_PI = test_pi_controller$(EXT)

all: $(TARGET_MEDIAN) $(TARGET_PI)
	@echo "Running Median Tests..."
	./$(TARGET_MEDIAN)
	@echo "Running PI Controller Tests..."
	./$(TARGET_PI)

$(TARGET_MEDIAN): src/test_median.c ../modules/median/median.c $(UNITY_SRC)
	$(CC) $(CFLAGS) -I../modules/median $^ -o $@

$(TARGET_PI): src/test_pi_controller.c ../modules/pi_controller/pi_controller.c $(UNITY_SRC)
	$(CC) $(CFLAGS) -I../modules/pi_controller $^ -o $@

clean:
	$(RM) *$(EXT)
