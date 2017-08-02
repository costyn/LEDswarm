
// Routine Palette Rainbow is always included - a safe routine
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
  #ifdef RT_BLACK
  "black",
  #endif

};
#define NUM_PATTERNS  (sizeof(routines)/sizeof(char *)) //array size


void currentPatternRun() {
// Serial.print(".");

  if( currentPattern != nextPattern ) {
    firstPatternIteration = true ;
  }

  if ( strcmp(routines[currentPattern], "p_rb_stripe") == 0  ) {
    FillLEDsFromPaletteColors(0) ;

    #ifdef RT_P_RB
  } else if ( strcmp(routines[currentPattern], "p_rb") == 0 ) {
    FillLEDsFromPaletteColors(1) ;
    #endif

    #ifdef RT_P_OCEAN
  } else if ( strcmp(routines[currentPattern], "p_ocean") == 0 ) {
    FillLEDsFromPaletteColors(2) ;
    #endif

    #ifdef RT_P_HEAT
  } else if ( strcmp(routines[currentPattern], "p_heat") == 0 ) {
    FillLEDsFromPaletteColors(3) ;
    #endif

    #ifdef RT_P_LAVA
  } else if ( strcmp(routines[currentPattern], "p_lava") == 0 ) {
    FillLEDsFromPaletteColors(4) ;
    #endif

    #ifdef RT_P_PARTY
  } else if ( strcmp(routines[currentPattern], "p_party") == 0 ) {
    FillLEDsFromPaletteColors(5) ;
    #endif

    #ifdef RT_P_CLOUD
  } else if ( strcmp(routines[currentPattern], "p_cloud") == 0 ) {
    FillLEDsFromPaletteColors(6) ;
    #endif

    #ifdef RT_P_FOREST
  } else if ( strcmp(routines[currentPattern], "p_forest") == 0 ) {
    FillLEDsFromPaletteColors(7) ;
    #endif

    #ifdef RT_TWIRL1
  } else if ( strcmp(routines[currentPattern], "twirl1") == 0 ) {
    twirlers( 1, false ) ;
    #endif

    #ifdef RT_TWIRL2
  } else if ( strcmp(routines[currentPattern], "twirl2") == 0 ) {
    twirlers( 2, false ) ;
    #endif

    #ifdef RT_TWIRL4
  } else if ( strcmp(routines[currentPattern], "twirl4") == 0 ) {
    twirlers( 4, false ) ;
    #endif

    #ifdef RT_TWIRL6
  } else if ( strcmp(routines[currentPattern], "twirl6") == 0 ) {
    twirlers( 6, false ) ;
    #endif

    #ifdef RT_TWIRL2_O
  } else if ( strcmp(routines[currentPattern], "twirl2o") == 0 ) {
    twirlers( 2, true ) ;
    #endif

    #ifdef RT_TWIRL4_O
  } else if ( strcmp(routines[currentPattern], "twirl4o") == 0 ) {
    twirlers( 4, true ) ;
    #endif

    #ifdef RT_TWIRL6_O
  } else if ( strcmp(routines[currentPattern], "twirl6o") == 0 ) {
    twirlers( 6, true ) ;
    #endif

    #ifdef RT_FADE_GLITTER
  } else if ( strcmp(routines[currentPattern], "fglitter") == 0 ) {
    fadeGlitter() ;
    taskCurrentPatternRun.setInterval( map( constrain( activityLevel(), 0, 4000), 0, 4000, 20, 5 ) * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_DISCO_GLITTER
  } else if ( strcmp(routines[currentPattern], "dglitter") == 0 ) {
    discoGlitter() ;
    taskCurrentPatternRun.setInterval( map( constrain( activityLevel(), 0, 2500), 0, 2500, 40, 2 ) * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_GLED
    // Gravity LED
  } else if ( strcmp(routines[currentPattern], "gled") == 0 ) {
    gLed() ;
    taskCurrentPatternRun.setInterval( 5 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_BLACK
  } else if ( strcmp(routines[currentPattern], "black") == 0 ) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    taskCurrentPatternRun.setInterval( 500 * TASK_MILLISECOND ) ;  // long because nothing is going on anyways.
    #endif

    #ifdef RT_RACERS
  } else if ( strcmp(routines[currentPattern], "racers") == 0 ) {
    racingLeds() ;
    taskCurrentPatternRun.setInterval( 8 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_WAVE
  } else if ( strcmp(routines[currentPattern], "wave") == 0 ) {
    waveYourArms() ;
    taskCurrentPatternRun.setInterval( 15 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_SHAKE_IT
  } else if ( strcmp(routines[currentPattern], "shakeit") == 0 ) {
    shakeIt() ;
    taskCurrentPatternRun.setInterval( 8 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_STROBE1
  } else if ( strcmp(routines[currentPattern], "strobe1") == 0 ) {
    strobe1() ;
    taskCurrentPatternRun.setInterval( 1 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_STROBE2
  } else if ( strcmp(routines[currentPattern], "strobe2") == 0 ) {
    strobe2() ;
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_HEARTBEAT
  } else if ( strcmp(routines[currentPattern], "heartbeat") == 0 ) {
    heartbeat() ;
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_VUMETER
  } else if ( strcmp(routines[currentPattern], "vumeter") == 0 ) {
    vuMeter() ;
    taskCurrentPatternRun.setInterval( 8 * TASK_MILLISECOND) ;
    #endif

    #ifdef RT_FASTLOOP
  } else if ( strcmp(routines[currentPattern], "fastloop") == 0 ) {
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND) ;
    fastLoop( false ) ;
    #endif

    #ifdef RT_FASTLOOP2
  } else if ( strcmp(routines[currentPattern], "fastloop2") == 0 ) {
    fastLoop( true ) ;
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND) ;
    #endif

    #ifdef RT_PENDULUM
  } else if ( strcmp(routines[currentPattern], "pendulum") == 0 ) {
    pendulum() ;
    //    taskCurrentPatternRun.setInterval( 1500 ) ; // needs a fast refresh rate - optimal in microseconds
    taskCurrentPatternRun.setInterval( 2 ) ; // needs a fast refresh rate
    #endif

    #ifdef RT_BOUNCEBLEND
  } else if ( strcmp(routines[currentPattern], "bounceblend") == 0 ) {
    bounceBlend() ;
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_JUGGLE_PAL
  } else if ( strcmp(routines[currentPattern], "jugglepal") == 0 ) {
    jugglePal() ;
    //    taskCurrentPatternRun.setInterval( 850 ) ; // optimal refresh in microseconds
    taskCurrentPatternRun.setInterval( 1 ) ; // fast refresh rate needed to not skip any LEDs
    #endif

    #ifdef RT_NOISE_LAVA
  } else if ( strcmp(routines[currentPattern], "noise_lava") == 0 ) {
    fillnoise8( 0, beatsin8( tapTempo.getBPM(), 1, 25), 30, 1); // pallette, speed, scale, loop
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_NOISE_PARTY
  } else if ( strcmp(routines[currentPattern], "noise_party") == 0 ) {
    fillnoise8( 1, beatsin8( tapTempo.getBPM(), 1, 25), 30, 1); // pallette, speed, scale, loop
    //    taskCurrentPatternRun.setInterval( beatsin16( tapTempo.getBPM(), 2000, 50000) ) ;
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_QUAD_STROBE
  } else if ( strcmp(routines[currentPattern], "quadstrobe") == 0 ) {
    quadStrobe();
    taskCurrentPatternRun.setInterval( (60000 / (tapTempo.getBPM() * 4)) * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_PULSE_3
  } else if ( strcmp(routines[currentPattern], "pulse3") == 0 ) {
    pulse3();
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_PULSE_5
  } else if ( strcmp(routines[currentPattern], "pulse5") == 0 ) {
    pulse5(3, false);
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_THREE_SIN_PAL
  } else if ( strcmp(routines[currentPattern], "tsp") == 0 ) {
    threeSinPal() ;
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_CYLON
  } else if ( strcmp(routines[currentPattern], "cylon") == 0 ) {
    cylon() ;
    taskCurrentPatternRun.setInterval( 1 * TASK_MILLISECOND ) ;
    #endif

  }
}


void selectNextPattern() {
  nextPattern = currentPattern + 1 ;

  if (nextPattern >= NUM_PATTERNS ) {
    nextPattern = 0;
  }

  if( role == "MASTER" ) {
     taskSendMessage.forceNextIteration(); // Schedule next iteration immediately, for sending a new pattern msg to slaves
  }
}
