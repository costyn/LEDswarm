#ifndef ANIMATION_CONTROLLER_H
#define ANIMATION_CONTROLLER_H

#include <FastLED.h>
#include "FX.h"

class AnimationController
{
public:
    AnimationController();
    void init();
    void updatePattern();
    void nextPattern();
    void setBPM(uint32_t bpm);
    uint32_t getBPM() const { return currentBPM; }
    uint8_t getCurrentPattern() const { return currentPattern; }
    void setMeshNumLeds(uint16_t numLeds);

private:
    CRGB _localLeds[LEDS_PER_NODE + 1];
    CRGB _meshleds[MAX_MESH_LEDS + 1];
    uint8_t currentPattern;
    uint8_t nextPattern;
    uint32_t currentBPM;
    FX fx;

    void runPattern();
    void copyLedsToLocal(uint8_t nodePos);
};

#endif