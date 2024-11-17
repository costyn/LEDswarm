// LEDswarm.h
#ifndef LEDSWARM_H
#define LEDSWARM_H

#define LEADER "LEADER"
#define FOLLOWER "FOLLOWER"

// Task timing defines

const char *routines[] = {
    "p_rb_stripe",
#ifdef RT_P_RB
    "p_rb",
#endif
#ifdef RT_P_OCEAN
    "p_ocean",
#endif
#ifdef RT_P_HEAT
    "p_heat",
#endif
#ifdef RT_P_LAVA
    "p_lava",
#endif
#ifdef RT_P_PARTY
    "p_party",
#endif
#ifdef RT_TWIRL1
    "twirl1",
#endif
#ifdef RT_TWIRL2
    "twirl2",
#endif
#ifdef RT_TWIRL4
    "twirl4",
#endif
#ifdef RT_TWIRL6
    "twirl6",
#endif
#ifdef RT_TWIRL2_O
    "twirl2o",
#endif
#ifdef RT_TWIRL4_O
    "twirl4o",
#endif
#ifdef RT_TWIRL6_O
    "twirl6o",
#endif
#ifdef RT_FADE_GLITTER
    "fglitter",
#endif
#ifdef RT_DISCO_GLITTER
    "dglitter",
#endif
#ifdef RT_RACERS
    "racers",
#endif
#ifdef RT_WAVE
    "wave",
#endif
#ifdef RT_SHAKE_IT
    "shakeit",
#endif
#ifdef RT_STROBE1
    "strobe1",
#endif
#ifdef RT_STROBE2
    "strobe2",
#endif
#ifdef RT_GLED
    "gled",
#endif
#ifdef RT_HEARTBEAT
    "heartbeat",
#endif
#ifdef RT_FASTLOOP
    "fastloop",
#endif
#ifdef RT_FASTLOOP2
    "fastloop2",
#endif
#ifdef RT_PENDULUM
    "pendulum",
#endif
#ifdef RT_VUMETER
    "vumeter",
#endif
#ifdef RT_NOISE_LAVA
    "noise_lava",
#endif
#ifdef RT_NOISE_PARTY
    "noise_party",
#endif
#ifdef RT_BOUNCEBLEND
    "bounceblend",
#endif
#ifdef RT_JUGGLE_PAL
    "jugglepal",
#endif
#ifdef RT_QUAD_STROBE
    "quadstrobe",
#endif
#ifdef RT_PULSE_3
    "pulse3",
#endif
#ifdef RT_PULSE_5
    "pulse5",
#endif
#ifdef RT_THREE_SIN_PAL
    "tsp",
#endif
#ifdef RT_CYLON
    "cylon",
#endif
#ifdef RT_FIRE_STRIPE
    "firestripe",
#endif
#ifdef RT_BLACK
    "black",
#endif

};

// array size
#define NUMROUTINES (sizeof(routines) / sizeof(char *)) // array size

#include <Arduino.h>
#include "AnimationController.h"
#include "MeshController.h"
#include "UiController.h"
#include "TaskController.h"

#define LEDSWARM_DEBUG

#ifdef LEDSWARM_DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTDEC(x) Serial.print(x, DEC)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x)
#endif

// Keep all the necessary defines

// Task declarations
extern Task taskCheckButtonPress;
extern Task taskCurrentPatternRun;
extern Task taskSendMessage;
extern Task taskSelectNextPattern;

// Global pattern variables (we'll encapsulate these later)
extern uint8_t currentPattern;
extern uint8_t nextPattern;
extern uint8_t _maxBright;

#endif