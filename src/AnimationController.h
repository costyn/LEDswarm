#ifndef ANIMATION_CONTROLLER_H
#define ANIMATION_CONTROLLER_H

#include <FastLED.h>
#include "FX.h"
#include "ArduinoTapTempo.h"

#define LEADER "LEADER"
#define FOLLOWER "FOLLOWER"

#if defined(NEO_PIXEL)
#define CHIPSET WS2812B
#define COLOR_ORDER GRB
#endif

#if defined(APA_102) || defined(APA_102_SLOW)
#include <SPI.h>
#define CHIPSET APA102
#define COLOR_ORDER BGR
#endif

class AnimationController
{
public:
    AnimationController();
    void init();
    void currentPatternRun();
    void selectNextPattern();
    void setBPM(uint32_t bpm);
    void setMeshNumLeds(uint16_t numLeds);
    void setAlone(bool alone);

    // Getters for LED arrays
    CRGB *getLocalLeds() { return localLeds; }
    CRGB *getMeshLeds() { return meshLeds; }
    CRGB *getMatrixLeds() { return _matrixLeds; }

    uint8_t getCurrentPattern() const { return currentPattern; }
    uint8_t getNextPattern() const { return nextPattern; }
    void setNextPattern(uint8_t pattern) { nextPattern = pattern; }

private:
    uint8_t currentPattern;
    uint8_t nextPattern;
    uint16_t meshNumLeds;
    CRGB localLeds[LEDS_PER_NODE + 1];
    CRGB meshLeds[MAX_MESH_LEDS + 1];
#ifdef ATOMMATRIX
    CRGB _matrixLeds[ATOM_NUM_LED + 1];
#endif

    FX fx;

    void copyToLocalLeds(uint8_t nodePos);
};

#endif