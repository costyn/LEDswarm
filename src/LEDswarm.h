#ifndef LEDSWARM_H
#define LEDSWARM_H

// ************************************************************************
//                         DEFINES FOR LIBRARIES
// ************************************************************************

// #define FASTLED_ALLOW_INTERRUPTS 1    // Allow interrupts, to prevent wifi weirdness ; https://github.com/FastLED/FastLED/wiki/Interrupt-problems
// #define INTERRUPT_THRESHOLD 1   // also see https://github.com/FastLED/FastLED/issues/367
#define USE_GET_MILLISECOND_TIMER // Define our own millis() source for FastLED beat functions: see get_millisecond_timer()
#define TASK_RES_MULTIPLIER 1

// ************************************************************************
//                              INCLUDES
// ************************************************************************

#include <Arduino.h>

#include <FastLED.h>
#include <painlessMesh.h>
#include <ArduinoTapTempo.h>
#include <JC_Button.h>
#include <TaskScheduler.h>

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

#define MESH_PREFIX "LEDforge"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

#if defined(NEO_PIXEL)
#define CHIPSET WS2812B
#define COLOR_ORDER GRB
#endif

#define LEADER "LEADER"
#define FOLLOWER "FOLLOWER"

#if defined(APA_102) || defined(APA_102_SLOW)
#include <SPI.h>
#define CHIPSET APA102
#define COLOR_ORDER BGR
#endif

#include "MeshController.h"
#include "AnimationController.h"
#include "UiController.h"
#include "TaskController.h"

// Modified LEDswarm.cpp
void setup()
{
    Serial.begin(115200);
    delay(1000);

    AnimationController animController;
    MeshController meshController(animController);
    UIController uiController(animController);
    TaskController taskController(meshController, animController, uiController);

    animController.init();
    meshController.init();
    uiController.init();
    taskController.init();
}

void loop()
{
    TaskController::scheduler.execute();
    mesh.update();
}

#endif