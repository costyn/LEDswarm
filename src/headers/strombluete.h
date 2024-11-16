// #define ATOMMATRIX

// ---- LED stuff ----
#define NEO_PIXEL
#define LEDS_PER_NODE 8
#define NUM_LEDS_PER_STRIP LEDS_PER_NODE
#define LED_PIN_1 18

#define MAX_NODES 5
#define MAX_MESH_LEDS MAX_NODES *LEDS_PER_NODE

// There are only 8 leds
#define DEFAULT_BRIGHTNESS 254
// #define MAX_BRIGHTNESS 254 // not used yet

// ---- Buttons ----
#define BUTTON_PIN 23
#define BPM_BUTTON_PIN 25

// ---- Misc ----
#define DEFAULT_BPM 130
#define DEFAULT_PATTERN 1
// Should the pattern auto advance? And how long to display each pattern?
#define AUTOADVANCE
#define AUTO_ADVANCE_DELAY 30

// ---- Which Patterns To Include? ----
// #define RT_P_OCEAN
#define RT_P_HEAT
#define RT_P_LAVA
#define RT_P_PARTY
#define RT_P_FOREST
// #define RT_TWIRL1
// #define RT_TWIRL2
// #define RT_TWIRL4
// #define RT_TWIRL6
// #define RT_TWIRL2_O
// #define RT_TWIRL4_O
// #define RT_TWIRL6_O
// #define RT_FADE_GLITTER
// #define RT_DISCO_GLITTER
// #define RT_FASTLOOP
// #define RT_FASTLOOP2
// #define RT_PENDULUM
// #define RT_BOUNCEBLEND
// #define RT_JUGGLE_PAL
// #define RT_NOISE_LAVA
// #define RT_NOISE_PARTY
// #define RT_PULSE_5_1
// #define RT_PULSE_5_2
// #define RT_PULSE_5_3
// #define RT_THREE_SIN_PAL
// #define RT_COLOR_GLOW
