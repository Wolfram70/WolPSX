CPP = g++
BUILD_DIR = build
SRC_DIR = src

all:
	$(CPP) $(SRC_DIR)/*.cpp -o $(BUILD_DIR)/emu