// LEDswarm.h
#ifndef LEDSWARM_H
#define LEDSWARM_H

#define LEADER "LEADER"
#define FOLLOWER "FOLLOWER"

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