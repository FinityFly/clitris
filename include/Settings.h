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

private:
    float ARR; // auto repeat rate
    float DAS; // delayed auto shift
    float DCD; // das cut delay
    float SDF; // soft drop factor

    std::unordered_map<std::string, std::vector<int>> keyBindings;
};

#endif