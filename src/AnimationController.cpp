#include "AnimationController.h"

AnimationController::AnimationController()
    : currentPattern(DEFAULT_PATTERN), nextPattern(DEFAULT_PATTERN), currentBPM(DEFAULT_BPM),
      fx(_meshleds, LEDS_PER_NODE)
{
}

void AnimationController::init()
{
    FastLED.setBrightness(DEFAULT_BRIGHTNESS);
    fx.setTempo(DEFAULT_BPM);
}

void AnimationController::updatePattern()
{
    runPattern();
}