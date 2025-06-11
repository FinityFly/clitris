CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

#SRC = src/main.cpp src/Game.cpp src/Menu.cpp src/Settings.cpp src/Tetris.cpp src/Tetromino.cpp
SRC = src/main.cpp src/Game.cpp src/Menu.cpp src/Settings.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = tetris

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
