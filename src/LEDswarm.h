// Enabled routines. Uncomment to enable
#define RT_P_RB
// #define RT_P_OCEAN
// #define RT_P_HEAT
// #define RT_P_LAVA
// #define RT_P_PARTY
// #define RT_P_FOREST
//#define RT_TWIRL1
//#define RT_TWIRL2
//#define RT_TWIRL4
//#define RT_TWIRL6
// #define RT_TWIRL2_O
// #define RT_TWIRL4_O
// #define RT_TWIRL6_O
// #define RT_FADE_GLITTER
// #define RT_DISCO_GLITTER
// #define RT_FIRE2012
//#define RT_HEARTBEAT
// #define RT_FASTLOOP
// #define RT_FASTLOOP2
//#define RT_STROBE1
// #define RT_BOUNCEBLEND
#define RT_JUGGLE_PAL
#define RT_NOISE_LAVA
#define RT_NOISE_PARTY
//#define RT_PULSE_3
//#define RT_PULSE_5
#define RT_THREE_SIN_PAL
// #define WHITESTRIPE
#define RT_CYLON



#ifdef DEBUG
#define DEBUG_PRINT(x)       Serial.printf(x,)
#define DEBUG_PRINT(x)       Serial.print (x)
#define DEBUG_PRINTDEC(x)    Serial.print (x, DEC)
#define DEBUG_PRINTLN(x)     Serial.println (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x)
#endif
