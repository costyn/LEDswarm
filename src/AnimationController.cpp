#include "AnimationController.h"
#include "TaskScheduler.h" // for TASK_MILLISECOND

extern Task taskCurrentPatternRun; // Still need this for interval setting
extern Task taskSendMessage;       // Still need this for pattern changes

AnimationController::AnimationController()
    : currentPattern(DEFAULT_PATTERN), nextPattern(DEFAULT_PATTERN), meshNumLeds(LEDS_PER_NODE)
#ifdef ATOMMATRIX
      ,
      fx(meshLeds, _matrixLeds, LEDS_PER_NODE)
#else
      ,
      fx(meshleds, LEDS_PER_NODE)
#endif
{
}

void AnimationController::init()
{
    fx.setTempo(DEFAULT_BPM);
    FastLED.setBrightness(DEFAULT_BRIGHTNESS);

#ifdef APA_102
    // FastLED.addLeds<CHIPSET, MY_DATA_PIN, MY_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(12)>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
#else
    FastLED.addLeds<CHIPSET, LED_PIN_1, COLOR_ORDER>(localLeds, LEDS_PER_NODE);
#endif

#ifdef ATOMMATRIX
    FastLED.addLeds<CHIPSET, ATOM_LEDPIN, COLOR_ORDER>(_matrixLeds, ATOM_NUM_LED);
#endif

    Serial.println("after animationController init");
}

void AnimationController::copyToLocalLeds(uint8_t nodePos)
{
    memcpy(&localLeds, &meshLeds[nodePos * LEDS_PER_NODE], sizeof(CRGB) * LEDS_PER_NODE);
}

void AnimationController::selectNextPattern()
{
    nextPattern = currentPattern + 1;

    if (nextPattern >= NUMROUTINES)
    {
        nextPattern = 0;
    }

    if (role == LEADER)
    {
        taskSendMessage.forceNextIteration();
    }
}

void AnimationController::setMeshNumLeds(uint16_t numLeds)
{
    meshNumLeds = numLeds;
    fx.setMeshNumLeds(numLeds);
}

void AnimationController::setBPM(uint32_t bpm)
{
    fx.setTempo(bpm);
}

void AnimationController::setAlone(bool alone)
{
    fx.setAlone(alone);
}

void AnimationController::currentPatternRun()
{
    // Moved from LEDswarm.cpp, everything inside the currentPatternRun() function
    taskCurrentPatternRun.setInterval(10 * TASK_MILLISECOND);

    if (strcmp(routines[currentPattern], "p_rb_stripe") == 0)
    {
        fx.setCurrentPalette(RainbowStripeColors_p);
        fx.FillLEDsFromPaletteColors();

#ifdef RT_P_RB
    }
    else if (strcmp(routines[currentPattern], "p_rb") == 0)
    {
        fx.setCurrentPalette(RainbowColors_p);
        fx.FillLEDsFromPaletteColors(1);
#endif

#ifdef RT_P_OCEAN
    }
    else if (strcmp(routines[currentPattern], "p_ocean") == 0)
    {
        fx.setCurrentPalette(OceanColors_p);
        fx.FillLEDsFromPaletteColors();
#endif

#ifdef RT_P_HEAT
    }
    else if (strcmp(routines[currentPattern], "p_heat") == 0)
    {
        fx.setCurrentPalette(HeatColors_p);
        fx.FillLEDsFromPaletteColors();
#endif

#ifdef RT_P_LAVA
    }
    else if (strcmp(routines[currentPattern], "p_lava") == 0)
    {
        fx.setCurrentPalette(LavaColors_p);
        fx.FillLEDsFromPaletteColors();
#endif

#ifdef RT_P_PARTY
    }
    else if (strcmp(routines[currentPattern], "p_party") == 0)
    {
        fx.setCurrentPalette(PartyColors_p);
        fx.FillLEDsFromPaletteColors();
#endif

#ifdef RT_P_CLOUD
    }
    else if (strcmp(routines[currentPattern], "p_cloud") == 0)
    {
        fx.setCurrentPalette(CloudColors_p);
        fx.FillLEDsFromPaletteColors(6);
#endif

#ifdef RT_P_FOREST
    }
    else if (strcmp(routines[currentPattern], "p_forest") == 0)
    {
        fx.setCurrentPalette(ForestColors_p);
        fx.FillLEDsFromPaletteColors();
#endif

#ifdef RT_TWIRL1
    }
    else if (strcmp(routines[currentPattern], "twirl1") == 0)
    {
        fx.twirlers(1, false);
        taskCurrentPatternRun.setInterval(1 * TASK_MILLISECOND);
#endif

#ifdef RT_TWIRL2
    }
    else if (strcmp(routines[currentPattern], "twirl2") == 0)
    {
        fx.twirlers(2, false);
        taskCurrentPatternRun.setInterval(1 * TASK_MILLISECOND);
#endif

#ifdef RT_TWIRL4
    }
    else if (strcmp(routines[currentPattern], "twirl4") == 0)
    {
        fx.twirlers(4, false);
        taskCurrentPatternRun.setInterval(1 * TASK_MILLISECOND);
#endif

#ifdef RT_TWIRL6
    }
    else if (strcmp(routines[currentPattern], "twirl6") == 0)
    {
        fx.twirlers(6, false);
        taskCurrentPatternRun.setInterval(1 * TASK_MILLISECOND);
#endif

#ifdef RT_TWIRL2_O
    }
    else if (strcmp(routines[currentPattern], "twirl2o") == 0)
    {
        fx.twirlers(2, true);
        taskCurrentPatternRun.setInterval(1 * TASK_MILLISECOND);
#endif

#ifdef RT_TWIRL4_O
    }
    else if (strcmp(routines[currentPattern], "twirl4o") == 0)
    {
        fx.twirlers(4, true);
        taskCurrentPatternRun.setInterval(1 * TASK_MILLISECOND);
#endif

#ifdef RT_TWIRL6_O
    }
    else if (strcmp(routines[currentPattern], "twirl6o") == 0)
    {
        fx.twirlers(6, true);
        taskCurrentPatternRun.setInterval(1 * TASK_MILLISECOND);
#endif

#ifdef RT_FADE_GLITTER
    }
    else if (strcmp(routines[currentPattern], "fglitter") == 0)
    {
        fx.fadeGlitter();
#ifdef USING_MPU
        taskCurrentPatternRun.setInterval(map(constrain(activityLevel(), 0, 2500), 0, 2500, 40, 2) * TASK_RES_MULTIPLIER);
#else
        taskCurrentPatternRun.setInterval(20 * TASK_RES_MULTIPLIER);
#endif
#endif

#ifdef RT_DISCO_GLITTER
    }
    else if (strcmp(routines[currentPattern], "dglitter") == 0)
    {
        fx.discoGlitter();
#ifdef USING_MPU
        taskCurrentPatternRun.setInterval(map(constrain(activityLevel(), 0, 2500), 0, 2500, 40, 2) * TASK_RES_MULTIPLIER);
#else
        taskCurrentPatternRun.setInterval(10 * TASK_RES_MULTIPLIER);
#endif
#endif

#ifdef RT_GLED
        // Gravity LED
    }
    else if (strcmp(routines[currentPattern], "gled") == 0)
    {
        fx.gLed();
        taskCurrentPatternRun.setInterval(5 * TASK_MILLISECOND);
#endif

#ifdef RT_BLACK
    }
    else if (strcmp(routines[currentPattern], "black") == 0)
    {
        fill_solid(leds, _meshNumLeds, CRGB::Black);
        // FastLED.show();
        taskCurrentPatternRun.setInterval(500 * TASK_MILLISECOND); // long because nothing is going on anyways.
#endif

#ifdef RT_RACERS
    }
    else if (strcmp(routines[currentPattern], "racers") == 0)
    {
        fx.racingLeds();
        taskCurrentPatternRun.setInterval(8 * TASK_MILLISECOND);
#endif

#ifdef RT_WAVE
    }
    else if (strcmp(routines[currentPattern], "wave") == 0)
    {
        fx.waveYourArms();
        taskCurrentPatternRun.setInterval(15 * TASK_MILLISECOND);
#endif

#ifdef RT_SHAKE_IT
    }
    else if (strcmp(routines[currentPattern], "shakeit") == 0)
    {
        fx.shakeIt();
        taskCurrentPatternRun.setInterval(8 * TASK_MILLISECOND);
#endif

#ifdef RT_STROBE1
    }
    else if (strcmp(routines[currentPattern], "strobe1") == 0)
    {
        fx.strobe1();
        taskCurrentPatternRun.setInterval(1 * TASK_MILLISECOND);
#endif

#ifdef RT_STROBE2
    }
    else if (strcmp(routines[currentPattern], "strobe2") == 0)
    {
        fx.strobe2();
        taskCurrentPatternRun.setInterval(10 * TASK_MILLISECOND);
#endif

#ifdef RT_HEARTBEAT
    }
    else if (strcmp(routines[currentPattern], "heartbeat") == 0)
    {
        fx.heartbeat();
        taskCurrentPatternRun.setInterval(10 * TASK_MILLISECOND);
#endif

#ifdef RT_VUMETER
    }
    else if (strcmp(routines[currentPattern], "vumeter") == 0)
    {
        vuMeter();
        taskCurrentPatternRun.setInterval(8 * TASK_MILLISECOND);
#endif

#ifdef RT_FASTLOOP
    }
    else if (strcmp(routines[currentPattern], "fastloop") == 0)
    {
        taskCurrentPatternRun.setInterval(10 * TASK_MILLISECOND);
        fx.fastLoop(false);
#endif

#ifdef RT_FASTLOOP2
    }
    else if (strcmp(routines[currentPattern], "fastloop2") == 0)
    {
        fx.fastLoop(true);
        taskCurrentPatternRun.setInterval(10 * TASK_MILLISECOND);
#endif

#ifdef RT_PENDULUM
    }
    else if (strcmp(routines[currentPattern], "pendulum") == 0)
    {
        fx.pendulum();
        //    taskCurrentPatternRun.setInterval( 1500 ) ; // needs a fast refresh rate - optimal in microseconds
        taskCurrentPatternRun.setInterval(2); // needs a fast refresh rate
#endif

#ifdef RT_BOUNCEBLEND
    }
    else if (strcmp(routines[currentPattern], "bounceblend") == 0)
    {
        fx.bounceBlend();
        taskCurrentPatternRun.setInterval(10 * TASK_MILLISECOND);
#endif

#ifdef RT_JUGGLE_PAL
    }
    else if (strcmp(routines[currentPattern], "jugglepal") == 0)
    {
        jugglePal();
        //    taskCurrentPatternRun.setInterval( 850 ) ; // optimal refresh in microseconds
        taskCurrentPatternRun.setInterval(1); // fast refresh rate needed to not skip any LEDs
#endif

#ifdef RT_NOISE_LAVA
    }
    else if (strcmp(routines[currentPattern], "noise_lava") == 0)
    {
        fx.setCurrentPalette(LavaColors_p);
        fx.fillnoise8(beatsin8(fx.getTempo(), 1, 25), 30, 1); // pallette, speed, scale, loop
        taskCurrentPatternRun.setInterval(10 * TASK_MILLISECOND);
#endif

#ifdef RT_NOISE_PARTY
    }
    else if (strcmp(routines[currentPattern], "noise_party") == 0)
    {
        fx.setCurrentPalette(PartyColors_p);
        fx.fillnoise8(beatsin8(fx.getTempo(), 1, 25), 30, 1);
        //    taskCurrentPatternRun.setInterval( beatsin16( tapTempo.getBPM(), 2000, 50000) ) ;
        taskCurrentPatternRun.setInterval(10 * TASK_MILLISECOND);
#endif

#ifdef RT_QUAD_STROBE
    }
    else if (strcmp(routines[currentPattern], "quadstrobe") == 0)
    {
        fx.quadStrobe();
        taskCurrentPatternRun.setInterval((60000 / (tapTempo.getBPM() * 4)) * TASK_MILLISECOND);
#endif

#ifdef RT_PULSE_3
    }
    else if (strcmp(routines[currentPattern], "pulse3") == 0)
    {
        fx.pulse3();
        taskCurrentPatternRun.setInterval(10 * TASK_MILLISECOND);
#endif

#ifdef RT_PULSE_5
    }
    else if (strcmp(routines[currentPattern], "pulse5") == 0)
    {
        fx.pulse5(3, false);
        taskCurrentPatternRun.setInterval(10 * TASK_MILLISECOND);
#endif

#ifdef RT_THREE_SIN_PAL
    }
    else if (strcmp(routines[currentPattern], "tsp") == 0)
    {
        fx.threeSinPal();
        taskCurrentPatternRun.setInterval(10 * TASK_MILLISECOND);
#endif

#ifdef RT_FIRE_STRIPE
    }
    else if (strcmp(routines[currentPattern], "firestripe") == 0)
    {
        fx.fireStripe();
        taskCurrentPatternRun.setInterval(10 * TASK_MILLISECOND);
#endif

#ifdef RT_CYLON
    }
    else if (strcmp(routines[currentPattern], "cylon") == 0)
    {
        fx.cylon();
        taskCurrentPatternRun.setInterval(1 * TASK_MILLISECOND);
#endif
    }

    // COPY
    memcpy(&localLeds, &meshLeds[_nodePos * LEDS_PER_NODE], sizeof(CRGB) * LEDS_PER_NODE);

    // if( role == LEADER) {
    //   localLeds[_nodePos] = CRGB::Purple;
    // } else {
    //   localLeds[_nodePos] = CRGB::Green;
    // }
    FastLED.show();
}
