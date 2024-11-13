#ifndef LEDSWARM_H
#define LEDSWARM_H

// ************************************************************************
//                         DEFINES FOR LIBRARIES
// ************************************************************************


// #define FASTLED_ALLOW_INTERRUPTS 1    // Allow interrupts, to prevent wifi weirdness ; https://github.com/FastLED/FastLED/wiki/Interrupt-problems
// #define INTERRUPT_THRESHOLD 1   // also see https://github.com/FastLED/FastLED/issues/367
#define USE_GET_MILLISECOND_TIMER     // Define our own millis() source for FastLED beat functions: see get_millisecond_timer()
#define TASK_RES_MULTIPLIER  1

// ************************************************************************
//                              INCLUDES
// ************************************************************************

#include <Arduino.h>
#include <FastLED.h>
#include <painlessMesh.h>
#include <ArduinoTapTempo.h>
#include <JC_Button.h>

#define LEDSWARM_DEBUG

#ifdef LEDSWARM_DEBUG
#define DEBUG_PRINT(x)       Serial.print (x)
#define DEBUG_PRINTDEC(x)    Serial.print (x, DEC)
#define DEBUG_PRINTLN(x)     Serial.println (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x)
#endif

#define   MESH_PREFIX     "LEDforge"
#define   MESH_PASSWORD     "somethingSneaky"
#define   MESH_PORT         5555

#if defined(NEO_PIXEL) || defined(NEO_PIXEL_MULTI)
#define CHIPSET     WS2812B
#define COLOR_ORDER GRB
#endif

#define LEADER "LEADER"
#define FOLLOWER "FOLLOWER"


#if defined(APA_102) || defined(APA_102_SLOW)
#include <SPI.h>
#define CHIPSET     APA102
#define COLOR_ORDER BGR
#endif


// ************************************************************************
//                    GLOBAL VARIABLES
// ************************************************************************

uint8_t  _numNodes = 1; // default
uint8_t  _nodePos = 0; // default
uint16_t _meshNumLeds = LEDS_PER_NODE; // at boot we are alone

CRGB     _localLeds[LEDS_PER_NODE+1];
CRGB     _meshleds[MAX_MESH_LEDS+1];

uint8_t  maxBright = DEFAULT_BRIGHTNESS ;
uint8_t  currentPattern = DEFAULT_PATTERN ; // Which mode do we start with
uint8_t  nextPattern    = currentPattern ;
uint8_t  currentBrightness = maxBright ;

#ifdef ATOMMATRIX
CRGB      matrix_leds[ATOM_NUM_LED];
#endif

#ifdef ATOMMATRIX
FX fx(_localLeds, matrix_leds, LEDS_PER_NODE);
#else
FX fx(_meshleds, LEDS_PER_NODE); // at boot, mesh is just 1 node
#endif

painlessMesh  mesh;
// https://painlessmesh.gitlab.io/painlessMesh/configuration_8hpp.html#a21459eda80c40da63432b0b89793f46d
SimpleList<uint32_t> nodes; // std::list<T>;
String role = LEADER ; // default start out as leader unless told otherwise
uint32_t activeFollower ;

//Scheduler
Scheduler userScheduler; // to control your personal task

// BPM variables
ArduinoTapTempo tapTempo;
bool newBPMSet = true ;     // flag for when new BPM is set by button
uint32_t currentBPM = 120 ; // default BPM of ArduinoTapTempo

Button bpmButton(BPM_BUTTON_PIN);
// Button bpmButton(BPM_BUTTON_PIN, 50, true, true);
Button nextPatternButton(BUTTON_PIN);


// ************************************************************************
//                      C++ FUNCTION PROTOTYPEs
// ************************************************************************

void checkButtonPress();
void sendMessage();
void currentPatternRun();
void selectNextPattern();

uint32_t get_millisecond_timer();

void checkLeadership() ;
boolean alone();
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId) ;
void changedConnectionCallback() ;
void nodeTimeAdjustedCallback(int32_t offset) ;
void delayReceivedCallback(uint32_t from, int32_t delay) ;


// ************************************************************************
//             TASK VARIABLES (need function prototypes)
// ************************************************************************

// Task variables
#define TASK_CHECK_BUTTON_PRESS_INTERVAL    10   // in milliseconds
#define CURRENTPATTERN_SELECT_DEFAULT_INTERVAL     5   // default scheduling time for currentPatternSELECT, in milliseconds
Task taskCheckButtonPress( TASK_CHECK_BUTTON_PRESS_INTERVAL, TASK_FOREVER, &checkButtonPress);
Task taskCurrentPatternRun( CURRENTPATTERN_SELECT_DEFAULT_INTERVAL, TASK_FOREVER, &currentPatternRun);
Task taskSendMessage( TASK_SECOND * 5, TASK_FOREVER, &sendMessage ); // check every 5 second if we have a new BPM / pattern to send
Task taskSelectNextPattern( TASK_SECOND * AUTO_ADVANCE_DELAY, TASK_FOREVER, &selectNextPattern);  // switch to next pattern every AUTO_ADVANCE_DELAY seconds
//Task taskRunPatternOnNode( TASK_IMMEDIATE, TASK_ONCE, &runPatternOnNode );

// TODO: rewrite this garbage
// Routine Palette Rainbow is always included - a safe routine
const char *routines[1024] = {
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

//array size
#define NUMROUTINES (sizeof(routines)/sizeof(char *)) //array size
// const int numRoutines = (sizeof(routines)/sizeof(char *));

#endif
