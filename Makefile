CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

SRC = src/main.cpp src/Game.cpp src/GameUtils.cpp src/UI.cpp src/Menu.cpp src/Settings.cpp src/Tetromino.cpp src/SRS.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = clitris
LDFLAGS = -lcurses

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
