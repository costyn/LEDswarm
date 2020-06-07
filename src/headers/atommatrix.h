#define ATOMMATRIX

// ---- LED stuff ----
#define NEO_PIXEL
#define NUM_LEDS    60
#define LED_PIN_1    26
#define DEFAULT_BRIGHTNESS 120
#define MAX_BRIGHTNESS 120

#define ATOM_LEDPIN  27
#define ATOM_MAX_BRIGHTNESS  100
#define ATOM_NUM_LED 25

// Offically should be 150/255 = x/150.

// Onboard:
// #define NEO_PIXEL
// #define LED_PIN     27   // which pin your Neopixels are connected to
// #define NUM_LEDS 25
// #define DEFAULT_BRIGHTNESS 20
// #define MAX_BRIGHTNESS 20


// ---- Buttons ----
#define BUTTON_PIN 39
// #define BUTTON_LED_PIN 3
#define BPM_BUTTON_PIN 25
// #define BUTTON_GND_PIN 15  // is the button connected to another PIN?

// ---- Misc ----
#define DEFAULT_BPM 130
#define DEFAULT_PATTERN   1
#define AUTOADVANCE

// ---- Patterns ----
#define RT_P_RB_STRIPE
#define RT_P_OCEAN
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
#define RT_FADE_GLITTER
#define RT_DISCO_GLITTER
// #define RT_HEARTBEAT
#define RT_FASTLOOP
#define RT_FASTLOOP2
// #define RT_PENDULUM
#define RT_BOUNCEBLEND
#define RT_JUGGLE_PAL
#define RT_NOISE_LAVA
#define RT_NOISE_PARTY
#define RT_PULSE_5_1
#define RT_PULSE_5_2
#define RT_PULSE_5_3
#define RT_THREE_SIN_PAL
#define RT_COLOR_GLOW
