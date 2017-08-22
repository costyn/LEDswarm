#define STEPS       3   // How wide the bands of color are.  1 = more like a gradient, 10 = more like stripes

void FillLEDsFromPaletteColors(uint8_t paletteIndex ) {
  static uint8_t startIndex = 1;  // initialize at start
  const int8_t flowDir = -1 ;

  if( firstPatternIteration ) {
    startIndex = 1 ;
    firstPatternIteration = false ;   // reset flag
  }

  const CRGBPalette16 palettes[] = { RainbowStripeColors_p,
    #ifdef RT_P_RB
    RainbowColors_p,
    #endif
    #ifdef RT_P_OCEAN
    OceanColors_p,
    #endif
    #ifdef RT_P_HEAT
    HeatColors_p,
    #endif
    #ifdef RT_P_LAVA
    LavaColors_p,
    #endif
    #ifdef RT_P_PARTY
    PartyColors_p,
    #endif
    #ifdef RT_P_CLOUD
    CloudColors_p,
    #endif
    #ifdef RT_P_FOREST
    ForestColors_p
    #endif
  } ;
  startIndex += flowDir ;

  uint8_t colorIndex = startIndex ;

  for ( uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( palettes[paletteIndex], colorIndex, maxBright, LINEARBLEND );
    colorIndex += STEPS;
  }

  // add extra glitter during "fast"
  if ( taskCurrentPatternRun.getInterval() < 10 ) {
    addGlitter(250);
  } else {
    addGlitter(25);
  }


FastLED.setBrightness( maxBright ) ;
FastLED.show();

//  taskCurrentPatternRun.setInterval( beatsin16( tapTempo.getBPM(), 1500, 50000) ) ; // microseconds
taskCurrentPatternRun.setInterval( beatsin16( tapTempo.getBPM(), 5, 50 ) ) ;
}



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
  addGlitter(240);
  FastLED.setBrightness( maxBright ) ;
  FastLED.show();
}
#endif




#ifdef RT_STROBE1
void strobe1() {
  if ( tapTempo.beatProgress() > 0.95 ) {
    fill_solid(leds, NUM_LEDS, CRGB::White ); // yaw for color
  } else if ( tapTempo.beatProgress() > 0.80 and tapTempo.beatProgress() < 0.85 ) {
    //    fill_solid(leds, NUM_LEDS, CRGB::White );
  } else {
    fill_solid(leds, NUM_LEDS, CRGB::Black); // black
  }
  FastLED.setBrightness( maxBright ) ;
  FastLED.show();
}
#endif



#ifdef RT_FIRE2012
#define COOLING  55
#define SPARKING 120
#define FIRELEDS round( NUM_LEDS / 2 )

// TODO: replace with original Fire2012 for LED strips

// Adapted Fire2012. This version starts in the middle and mirrors the fire going down to both ends.
// Works well with the Adafruit glow fur scarf.
// FIRELEDS defines the position of the middle LED.

void Fire2012()
{
  // Array of temperature readings at each simulation cell
  static byte heat[FIRELEDS];

  // Step 1.  Cool down every cell a little
  for ( uint8_t i = 0; i < FIRELEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / FIRELEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = FIRELEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = FIRELEDS; j < NUM_LEDS; j++) {
    int heatIndex = j - FIRELEDS ;
    CRGB color = HeatColor( heat[heatIndex]);
    leds[j] = color;
  }

  //  "Reverse" Mapping needed:
  //    ledindex 44 = heat[0]
  //    ledindex 43 = heat[1]
  //    ledindex 42 = heat[2]
  //    ...
  //    ledindex 1 = heat[43]
  //    ledindex 0 = heat[44]

  for ( int j = 0; j <= FIRELEDS; j++) {
    int ledIndex = FIRELEDS - j ;
    CRGB color = HeatColor( heat[j]);
    leds[ledIndex] = color;
  }
  FastLED.setBrightness( maxBright ) ;
  FastLED.show();

} // end Fire2012
#endif




#ifdef WHITESTRIPE
#define STRIPE_LENGTH 5
// This routines goes "over" other patterns, remembering/copying the
// pattern it is writing over and writing it back behind it.

void whiteStripe() {
  static CRGB patternCopy[STRIPE_LENGTH] ;
  static int startLed = 0 ;

  if ( taskWhiteStripe.getInterval() != WHITESTRIPE_SPEED ) {
    taskWhiteStripe.setInterval( WHITESTRIPE_SPEED ) ;
  }

  if ( startLed == 0 ) {
    for (uint8_t i = 0; i < STRIPE_LENGTH ; i++ ) {
      patternCopy[i] = leds[i];
    }
  }

  // 36 40   44 48 52 56   60

  leds[startLed] = patternCopy[0] ;
  for (uint8_t i = 0; i < STRIPE_LENGTH - 2; i++ ) {
    patternCopy[i] = patternCopy[i + 1] ;
  }
  patternCopy[STRIPE_LENGTH - 1] = leds[startLed + STRIPE_LENGTH] ;

  fill_gradient(leds, startLed + 1, CHSV(0, 0, 255), startLed + STRIPE_LENGTH, CHSV(0, 0, 255), SHORTEST_HUES);

  startLed++ ;

  if ( startLed + STRIPE_LENGTH == NUM_LEDS - 1) { // LED nr 90 is index 89
    for (uint8_t i = startLed; i < startLed + STRIPE_LENGTH; i++ ) {
      leds[i] = patternCopy[i];
    }

    startLed = 0 ;
    taskWhiteStripe.setInterval(random16(4000, 10000)) ;
  }

  FastLED.setBrightness( maxBright ) ;
  FastLED.show();
}
#endif





#if defined(RT_TWIRL1) || defined(RT_TWIRL2) || defined(RT_TWIRL4) || defined(RT_TWIRL6) || defined(RT_TWIRL2_O) || defined(RT_TWIRL4_O) || defined(RT_TWIRL6_O)
// Counter rotating twirlers with blending
// 1 twirler - 1 white = 120/1
// 2 twirler - 1 white = 120/1
// 4 twirler - 2 white = 120/2
// 6 twirler - 3 white = 120/3

void twirlers(uint8_t numTwirlers, bool opposing ) {
  uint8_t pos = 0 ;
  uint8_t speedCorrection = 0 ;

  if ( numTwirlers == 1 ) {
    speedCorrection = 1 ;
  } else {
    speedCorrection = numTwirlers / 2 ;
  }
  uint8_t clockwiseFirst = lerp8by8( 0, NUM_LEDS, beat8( tapTempo.getBPM() / speedCorrection )) ;
  const CRGB clockwiseColor = CRGB::White ;
  const CRGB antiClockwiseColor = CRGB::Red ;

  if ( opposing ) {
    fadeall(map( numTwirlers, 1, 6, 240, 180 ));
  } else {
    fadeall(map( numTwirlers, 1, 6, 240, 180 ));
  }

  for (uint8_t i = 0 ; i < numTwirlers ; i++) {
    if ( (i % 2) == 0 ) {
      pos = (clockwiseFirst + round( NUM_LEDS / numTwirlers ) * i) % NUM_LEDS ;
      if ( leds[pos] ) { // FALSE if currently BLACK - don't blend with black
        leds[pos] = blend( leds[pos], clockwiseColor, 128 ) ;
      } else {
        leds[pos] = clockwiseColor ;
      }

    } else {

      if ( opposing ) {
        uint8_t antiClockwiseFirst = NUM_LEDS - (lerp8by8( 0, NUM_LEDS, beat8( tapTempo.getBPM() / speedCorrection ))) % NUM_LEDS ;
        pos = (antiClockwiseFirst + round( NUM_LEDS / numTwirlers ) * i) % NUM_LEDS ;
      } else {
        pos = (clockwiseFirst + round( NUM_LEDS / numTwirlers ) * i) % NUM_LEDS ;
      }
      if ( leds[pos] ) { // FALSE if currently BLACK - don't blend with black
        leds[pos] = blend( leds[pos], antiClockwiseColor, 128 ) ;
      } else {
        leds[pos] = antiClockwiseColor ;
      }
    }
  }
  FastLED.setBrightness( maxBright ) ;
  FastLED.show();
  taskCurrentPatternRun.setInterval( 1 * TASK_MILLISECOND ) ;
} // end twirlers()
#endif



#ifdef RT_HEARTBEAT
void heartbeat() {
  const uint8_t hbTable[] = {
    25,
    61,
    105,
    153,
    197,
    233,
    253,
    255,
    252,
    243,
    230,
    213,
    194,
    149,
    101,
    105,
    153,
    197,
    216,
    233,
    244,
    253,
    255,
    255,
    252,
    249,
    243,
    237,
    230,
    223,
    213,
    206,
    194,
    184,
    174,
    162,
    149,
    138,
    126,
    112,
    101,
    91,
    78,
    69,
    62,
    58,
    51,
    47,
    43,
    39,
    37,
    35,
    29,
    25,
    22,
    20,
    19,
    15,
    12,
    9,
    8,
    6,
    5,
    3,
  };

  #define NUM_STEPS (sizeof(hbTable)/sizeof(uint8_t *)) //array size

  fill_solid(leds, NUM_LEDS, CRGB::Red);
  // beat8 generates index 0-255 (fract8) as per getBPM(). lerp8by8 interpolates that to array index:
  uint8_t hbIndex = lerp8by8( 0, NUM_STEPS, beat8( tapTempo.getBPM() )) ;
  uint8_t brightness = lerp8by8( 0, maxBright, hbTable[hbIndex] ) ;
  FastLED.setBrightness( brightness );
  FastLED.show();
}
#endif


#if defined(RT_FASTLOOP) || defined(RT_FASTLOOP2)

#define FL_LENGHT 20   // how many LEDs should be in the "stripe"
#define FL_MIDPOINT FL_LENGHT / 2
#define MAX_LOOP_SPEED 5

void fastLoop(bool reverse) {
  static int16_t startP = 0 ;
  static uint8_t hue = 0 ;

  if ( ! reverse ) {
    startP = lerp8by8( 0, NUM_LEDS, beat8( tapTempo.getBPM() )) ;  // start position
  } else {
    startP += map( sin8( beat8( tapTempo.getBPM() / 4 )), 0, 255, -MAX_LOOP_SPEED, MAX_LOOP_SPEED + 1 ) ; // it was hard to write, it should be hard to undestand :grimacing:
  }

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  fillGradientRing(startP, CHSV(hue, 255, 0), startP + FL_MIDPOINT, CHSV(hue, 255, 255));
  fillGradientRing(startP + FL_MIDPOINT + 1, CHSV(hue, 255, 255), startP + FL_LENGHT, CHSV(hue, 255, 0));

  FastLED.setBrightness( maxBright ) ;
  FastLED.show();
  hue++  ;

}
#endif


#if defined(RT_NOISE_LAVA) || defined(RT_NOISE_PARTY)
// FastLED library NoisePlusPalette routine rewritten for 1 dimensional LED strip
// - speed determines how fast time moves forward.  Try  1 for a very slow moving effect,
// or 60 for something that ends up looking like water.

// - Scale determines how far apart the pixels in our noise array are.  Try
// changing these values around to see how it affects the motion of the display.  The
// higher the value of scale, the more "zoomed out" the noise will be.  A value
// of 1 will be so zoomed in, you'll mostly see solid colors.

// if current palette is a 'loop', add a slowly-changing base value

void fillnoise8(uint8_t currentPalette, uint8_t speed, uint8_t scale, boolean colorLoop ) {
  static uint8_t noise[NUM_LEDS];

  const CRGBPalette16 palettes[] = { LavaColors_p, PartyColors_p } ;

  static uint16_t x = random16();
  static uint16_t y = random16();
  static uint16_t z = random16();

  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
  // The amount of data smoothing we're doing depends on "speed".
  uint8_t dataSmoothing = 0;

  if ( speed < 50) {
    dataSmoothing = 200 - (speed * 4);
  }

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    int ioffset = scale * i;

    uint8_t data = inoise8(x + ioffset, y, z);

    // The range of the inoise8 function is roughly 16-238.
    // These two operations expand those values out to roughly 0..255
    // You can comment them out if you want the raw noise data.
    data = qsub8(data, 16);
    data = qadd8(data, scale8(data, 39));

    if ( dataSmoothing ) {
      uint8_t olddata = noise[i];
      uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
      data = newdata;
    }

    noise[i] = data;
  }

  z += speed;

  // apply slow drift to X and Y, just for visual variation.
  x += speed / 8;
  y -= speed / 16;

  static uint8_t ihue = 0;

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    // We use the value at the i coordinate in the noise
    // array for our brightness, and a 'random' value from NUM_LEDS - 1
    // for our pixel's index into the color palette.

    uint8_t index = noise[i];
    uint8_t bri =   noise[NUM_LEDS - i];
    // uint8_t bri =  sin(noise[NUM_LEDS - i]);  // more light/dark variation

    // if this palette is a 'loop', add a slowly-changing base value
    if ( colorLoop) {
      index += ihue;
    }

    // brighten up, as the color palette itself often contains the
    // light/dark dynamic range desired
    if ( bri > 127 ) {
      bri = 255;
    } else {
      bri = dim8_raw( bri * 2);
    }

    CRGB color = ColorFromPalette( palettes[currentPalette], index, bri);
    leds[i] = color;
  }
  ihue += 1;

  FastLED.setBrightness( maxBright ) ;
  FastLED.show();
}
#endif




#ifdef RT_BOUNCEBLEND
void bounceBlend() {
  uint8_t speed = beatsin8( tapTempo.getBPM(), 0, 255);
  static uint8_t startLed = 1 ;
  CHSV endclr = blend(CHSV(0, 255, 255), CHSV(160, 255, 255) , speed);
  CHSV midclr = blend(CHSV(160, 255, 255) , CHSV(0, 255, 255) , speed);
  fillGradientRing(startLed, endclr, startLed + NUM_LEDS / 2, midclr);
  fillGradientRing(startLed + NUM_LEDS / 2 + 1, midclr, startLed + NUM_LEDS, endclr);

  FastLED.setBrightness( maxBright ) ;
  FastLED.show();

  if ( (taskCurrentPatternRun.getRunCounter() % 10 ) == 0 ) {
    startLed++ ;
    if ( startLed + 1 == NUM_LEDS ) startLed = 0  ;
  }
} // end bounceBlend()
#endif




/* juggle_pal
Originally by: Mark Kriegsman
Modified by: Andrew Tuline
Modified further by: Costyn van Dongen
Date: May, 2017
*/

#ifdef RT_JUGGLE_PAL
void jugglePal() {                                             // A time (rather than loop) based demo sequencer. This gives us full control over the length of each sequence.

  static uint8_t    numdots =   4;                                     // Number of dots in use.
  static uint8_t   thisfade =   2;                                     // How long should the trails be. Very low value = longer trails.
  static uint8_t   thisdiff =  16;                                     // Incremental change in hue between each dot.
  static uint8_t    thishue =   0;                                     // Starting hue.
  static uint8_t     curhue =   0;                                     // The current hue
  static uint8_t   thisbeat =   35;                                     // Higher = faster movement.

  uint8_t secondHand = ( get_millisecond_timer() / 1000) % 60;                // Change '60' to a different value to change duration of the loop (also change timings below)
  static uint8_t lastSecond = 99;                             // This is our 'debounce' variable.

  if (lastSecond != secondHand) {                             // Debounce to make sure we're not repeating an assignment.
    lastSecond = secondHand;
    switch (secondHand) {
      case  0: numdots = 1; thisbeat = tapTempo.getBPM() / 2; thisdiff = 8;  thisfade = 8;  thishue = 0;   break;
      case  7: numdots = 2; thisbeat = tapTempo.getBPM() / 2; thisdiff = 4;  thisfade = 12; thishue = 0;   break;
      case 25: numdots = 4; thisbeat = tapTempo.getBPM() / 2; thisdiff = 24; thisfade = 50; thishue = 128; break;
      case 40: numdots = 2; thisbeat = tapTempo.getBPM() / 2; thisdiff = 16; thisfade = 50; thishue = 0; break;
      case 52: numdots = 4; thisbeat = tapTempo.getBPM() / 2; thisdiff = 24; thisfade = 80; thishue = 160; break;
    }
  }

  curhue = thishue;                                           // Reset the hue values.
  fadeToBlackBy(leds, NUM_LEDS, thisfade);

  for ( uint8_t i = 0; i < numdots; i++) {
    leds[beatsin16(thisbeat + i + numdots, 0, NUM_LEDS - 1)] += ColorFromPalette(RainbowColors_p, curhue, 255, LINEARBLEND); // Munge the values and pick a colour from the palette
    curhue += thisdiff;
  }

  FastLED.setBrightness( maxBright ) ;
  FastLED.show();

} // end jugglePal()
#endif



#ifdef RT_PULSE_3
#define PULSE_WIDTH 10
void pulse3() {
  uint8_t width = beatsin8( constrain( tapTempo.getBPM() * 2, 0, 255), 0, PULSE_WIDTH ) ; // can't use BPM > 255
  uint8_t hue = beatsin8( 1, 0, 255) ;
  static uint8_t middle = 0 ;

  if ( width == 1 ) {
    middle = taskCurrentPatternRun.getRunCounter() % 60 + taskCurrentPatternRun.getRunCounter() % 2;
  }

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  fillGradientRing(middle - width, CHSV(hue, 255, 0), middle, CHSV(hue, 255, 255));
  fillGradientRing(middle, CHSV(hue, 255, 255), middle + width, CHSV(hue, 255, 0));

  FastLED.setBrightness( maxBright ) ;
  FastLED.show() ;
}
#endif

#ifdef RT_PULSE_5
void pulse5( uint8_t numPulses, boolean leadingDot) {
  uint8_t spacing = NUM_LEDS / numPulses ;
  uint8_t pulseWidth = (spacing / 2) - 1 ; // leave 1 led empty at max
  uint8_t middle = beatsin8( 5, 0, NUM_LEDS / 2) ;
  uint8_t width = beatsin8( tapTempo.getBPM(), 0, pulseWidth) ;
  uint8_t hue = beatsin8( tapTempo.getBPM(), 0, 30) ;

  fill_solid(leds, NUM_LEDS, CRGB::Black);

  for ( uint8_t i = 0 ; i < numPulses; i++ ) {
    uint8_t offset = spacing * i ;
    fillGradientRing(middle - width + offset, CHSV(hue, 255, 0), middle + offset, CHSV(hue, 255, 255));
    fillGradientRing(middle + offset, CHSV(hue, 255, 255), middle + width + offset, CHSV(hue, 255, 0));

    if ( leadingDot ) {  // abusing fill gradient since it deals with "ring math"
      fillGradientRing(middle - width + offset, CHSV(0, 255, 255), middle - width + offset, CHSV(0, 255, 255));
      fillGradientRing(middle + width + offset, CHSV(0, 255, 255), middle + width + offset, CHSV(0, 255, 255));
    }
  }

  FastLED.setBrightness( maxBright ) ;
  FastLED.show() ;
}
#endif



#ifdef RT_THREE_SIN_PAL

/* three_sin_pal_demo
By: Andrew Tuline
Date: March, 2015
3 sine waves, one for each colour. I didn't take this far, but you could change the beat frequency and so on. . .
*/
#define MAXCHANGES 24
// Frequency, thus the distance between waves:
#define MUL1 7
#define MUL2 6
#define MUL3 5
void threeSinPal() {
  static int wave1 = 0;                                                // Current phase is calculated.
  static int wave2 = 0;
  static int wave3 = 0;

  static CRGBPalette16 currentPalette(CRGB::Black);
  static CRGBPalette16 targetPalette(PartyColors_p);

  if ( taskCurrentPatternRun.getRunCounter() % 2 == 0 ) {
    nblendPaletteTowardPalette( currentPalette, targetPalette, MAXCHANGES);

    wave1 += beatsin8(10, -4, 4);
    wave2 += beatsin8(15, -2, 2);
    wave3 += beatsin8(12, -3, 3);

    for (int k = 0; k < NUM_LEDS; k++) {
      uint8_t tmp = sin8(MUL1 * k + wave1) + sin8(MUL2 * k + wave2) + sin8(MUL3 * k + wave3);
      leds[k] = ColorFromPalette(currentPalette, tmp, 255);
    }
  }

  uint8_t secondHand = (get_millisecond_timer() / 1000) % 60;
  static uint8_t lastSecond = 99;

  if ( lastSecond != secondHand) {
    lastSecond = secondHand;
    CRGB p = CHSV( HUE_PURPLE, 255, 255);
    CRGB g = CHSV( HUE_GREEN, 255, 255);
    CRGB u = CHSV( HUE_BLUE, 255, 255);
    CRGB b = CRGB::Black;
    CRGB w = CRGB::White;

    switch (secondHand) {
      case  0: targetPalette = RainbowColors_p; break;
      case  5: targetPalette = CRGBPalette16( u, u, b, b, p, p, b, b, u, u, b, b, p, p, b, b); break;
      case 10: targetPalette = OceanColors_p; break;
      case 15: targetPalette = CloudColors_p; break;
      case 20: targetPalette = LavaColors_p; break;
      case 25: targetPalette = ForestColors_p; break;
      case 30: targetPalette = PartyColors_p; break;
      case 35: targetPalette = CRGBPalette16( b, b, b, w, b, b, b, w, b, b, b, w, b, b, b, w); break;
      case 40: targetPalette = CRGBPalette16( u, u, u, w, u, u, u, w, u, u, u, w, u, u, u, w); break;
      case 45: targetPalette = CRGBPalette16( u, p, u, w, p, u, u, w, u, g, u, w, u, p, u, w); break;
      case 50: targetPalette = CloudColors_p; break;
      case 55: targetPalette = CRGBPalette16( u, u, u, w, u, u, p, p, u, p, p, p, u, p, p, w); break;
      case 60: break;
    }
  }

  FastLED.setBrightness( maxBright ) ;
  FastLED.show();

} // threeSinPal()
#endif


#ifdef RT_CYLON
void cylon() {
  //uint8_t ledPos = beatsin8( tapTempo.getBPM(), 0, NUM_LEDS - 1 ) ;
  //uint8_t ledPos = beatsin8( 40, 0, NUM_LEDS - 1 ) ;
  //uint8_t ledPos = lerp8by8( 0, NUM_LEDS-1, ease8InOutQuad beatsin8( 40 ))) ;
  uint8_t ledPos = beatsin8( tapTempo.getBPM()/2, 0, NUM_LEDS - 1 ) ;
  leds[ledPos] = CRGB::Orange ;
  uint8_t ledPos2 = beatsin8( tapTempo.getBPM()/2, 0, NUM_LEDS - 1, 0, 40 ) ;
  leds[ledPos2] = CRGB::Red ;
  FastLED.setBrightness( maxBright ) ;
  FastLED.show();
  fadeToBlackBy(leds, NUM_LEDS, 255);
}
#endif
