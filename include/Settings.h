#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <unordered_map>
#include <vector>
class Settings {
public:
    void configureDefault();
    void configure();
    std::unordered_map<std::string, std::vector<int>> getKeyBindings() const;
    float getARR() const { return ARR; }
    float getDAS() const { return DAS; }
    float getDCD() const { return DCD; }
    float getSDF() const { return SDF; }

private:
    float ARR; // auto repeat rate
    float DAS; // delayed auto shift
    float DCD; // das cut delay
    float SDF; // soft drop factor

    std::unordered_map<std::string, std::vector<int>> keyBindings;
};

#endif