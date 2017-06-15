#include <arduino.h>

#ifdef RT_FADE_GLITTER
void fadeGlitter() {
  addGlitter(90);
  FastLED.setBrightness( maxBright ) ;
  FastLED.show();
  fadeToBlackBy(leds, NUM_LEDS, 200);
}
#endif


#ifdef RT_DISCO_GLITTER
void discoGlitter() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  addGlitter(map( constrain( activityLevel(), 0, 3000), 0, 3000, 100, 255 ));
  FastLED.setBrightness( maxBright ) ;
  FastLED.show();
}
#endif


#ifdef RT_STROBE1
#define FLASHLENGTH 20
void strobe1() {
  if ( tapTempo.beatProgress() > 0.95 ) {
    fill_solid(leds, NUM_LEDS, CHSV( map( yprX, 0, 360, 0, 255 ), 255, 255)); // yaw for color
  } else if ( tapTempo.beatProgress() > 0.80 and tapTempo.beatProgress() < 0.85 ) {
    fill_solid(leds, NUM_LEDS, CRGB::White );
  } else {
    fill_solid(leds, NUM_LEDS, CRGB::Black); // black
  }
  FastLED.setBrightness( maxBright ) ;
  FastLED.show();
}
#endif
