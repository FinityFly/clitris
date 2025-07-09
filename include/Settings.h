#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <unordered_map>
#include <vector>
struct TetrominoStyle {
    char character;
    std::string color;
};

class Settings {
public:
    static void configure();
    static std::unordered_map<std::string, std::vector<int>> getKeyBindings();
    static float getARR() { return ARR; }
    static float getDAS() { return DAS; }
    static float getDCD() { return DCD; }
    static float getSDF() { return SDF; }
    static std::string getMode() { return mode; }

    static void setMode(const std::string& newMode) {
        mode = newMode;
    }

    static void saveConfig();
    static void loadConfig();

    static const std::unordered_map<std::string, TetrominoStyle>& getTetrominoStyles() { return tetrominoStyles; }
    static void setTetrominoStyle(const std::string& tetromino, const TetrominoStyle& style) {
        tetrominoStyles[tetromino] = style;
    }

private:
    static float ARR; // auto repeat rate
    static float DAS; // delayed auto shift
    static float DCD; // das cut delay
    static float SDF; // soft drop factor

    static std::string mode;

    static std::unordered_map<std::string, std::vector<int>> keyBindings;

    static std::unordered_map<std::string, TetrominoStyle> tetrominoStyles;

    static std::string getUserDataPath();
};

#endif