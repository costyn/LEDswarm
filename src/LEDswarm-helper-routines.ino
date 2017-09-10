#include <Arduino.h>

void fadeall(uint8_t fade_all_speed) {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(fade_all_speed);
  }
}

void brightall(uint8_t bright_all_speed) {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i] += leds[i].scale8(bright_all_speed) ;
  }
}


void addGlitter( fract8 chanceOfGlitter)
{
  for ( uint8_t i = 0 ; i < 5 ; i++ ) {
    if ( random8() < chanceOfGlitter) {
      leds[ random16(NUM_LEDS) ] += CRGB::White;
    }
  }
}


// Fill a gradient on a LED ring with any possible start positions.
// startLed and endLed may be negative (one or both), may be larger than NUM_LEDS (one or both)
// TODO:
// * startLed cannot (yet) be > endLed
// * remove floating point calculation; replace by linear interpolation?

void fillGradientRing( int startLed, CHSV startColor, int endLed, CHSV endColor ) {
  if ( startLed > endLed ) {
    fill_gradient(leds, endLed, CHSV(0, 255, 255) , startLed, CHSV(0, 255, 255), SHORTEST_HUES ); // show RED for error!
  } else {
    // Determine actual start and actual end (normalize using modulo):
    int actualStart = mod(startLed + NUM_LEDS, NUM_LEDS)  ;
    int actualEnd = mod(endLed + NUM_LEDS, NUM_LEDS) ;

    // If beginning is at say 50, and end at 10, then we split the gradient in 2:
    // * one from 50-59
    // * one from 0-10
    // To determine which color should be at 59 and 0 we use the blend function:
    if ( actualStart > actualEnd ) {
      float ratio = 1.0 - float(actualEnd) / float(endLed - startLed) ; // cast to float otherwise the division won't work
      int normalizedRatio = round( ratio * 255 ) ; // determine what ratio of startColor and endColor we need at LED 0
      CHSV colorAtLEDZero = blend(startColor, endColor, normalizedRatio);

      fill_gradient(leds, actualStart, startColor, NUM_LEDS - 1, colorAtLEDZero, SHORTEST_HUES);
      fill_gradient(leds, 0, colorAtLEDZero, actualEnd, endColor, SHORTEST_HUES);
    } else {
      fill_gradient(leds, actualStart, startColor, actualEnd, endColor, SHORTEST_HUES);
    }
  }
}

// This is a little convoluted and could probably be written better :)
void fillSolidRing( int startLed, int endLed, CHSV color ) {
  if ( startLed > endLed ) {
    fill_gradient(leds, endLed, CHSV(0, 255, 255), startLed, CHSV(0, 255, 255), SHORTEST_HUES ); // show RED for error!
  } else {
    // Determine actual start and actual end (normalize using custom modulo):
    int actualStart = mod(startLed + NUM_LEDS, NUM_LEDS)  ;
    int actualEnd = mod(endLed + NUM_LEDS, NUM_LEDS) ;

    // If beginning is at say 50, and end at 10, then we split the fill in 2:
    // * one from 50-59
    // * one from 0-10
    if ( actualStart > actualEnd ) {
      fill_solid(leds + actualStart, NUM_LEDS - actualStart, color);
      fill_solid(leds, actualEnd, color);
    } else {
      fill_solid(leds + actualStart, actualEnd - actualStart, color);
    }
  }
} // end fillSolidRing()




// Custom mod which always returns a positive number
int mod(int x, int m) {
  return (x % m + m) % m;
}
