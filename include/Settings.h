#ifndef SETTINGS_H
#define SETTINGS_H

class Settings {
public:
    void configure();

private:
    float arr; // auto repeat rate
    float das; // delayed auto shift
    float dcd; // das cut delay
    float sdf; // soft drop factor
};

#endif

