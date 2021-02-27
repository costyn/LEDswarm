// TODO: convert Serial.printfs and println's to DEBUG statements we can turn on/off

#include <FX.h>
#include "LEDswarm.h"

// ************************************************************************
//                              SETUP
// ************************************************************************

void setup() {
  Serial.begin(115200);
  delay(1000); // Startup delay; let things settle down

  // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on; WARNING, buggy!!
  // mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION | DEBUG );  // set before init() so that you can see startup messages
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION | SYNC );  // set before init() so that you can see startup messages
  // mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

#ifdef APA_102
  // FastLED.addLeds<CHIPSET, MY_DATA_PIN, MY_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(12)>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
#else
  // FastLED.addLeds<CHIPSET, LED_PIN_1, COLOR_ORDER>(leds, NUM_NODE_LEDS);
#endif

#ifdef ATOMMATRIX 
  // FastLED.addLeds<CHIPSET, ATOM_LEDPIN, COLOR_ORDER>(matrix_leds, ATOM_NUM_LED);
#endif

  int pins[1]={27};
  _driver.initled((uint8_t*)_localLeds,pins,1,LEDS_PER_NODE,ORDER_GRB);
  _driver.setBrightness(100);

  userScheduler.addTask( taskSendMessage );
  userScheduler.addTask( taskCheckButtonPress );
  userScheduler.addTask( taskCurrentPatternRun );
  taskCheckButtonPress.enable() ;

#ifdef AUTOADVANCE
  userScheduler.addTask( taskSelectNextPattern );
  taskSelectNextPattern.enable() ;
#endif

  // pinMode(BPM_BUTTON_PIN, INPUT_PULLUP);
  bpmButton.begin();
  nextPatternButton.begin();

  tapTempo.setBPM(DEFAULT_BPM);

  Serial.print("Starting up... my Node ID is: ");
  Serial.println(mesh.getNodeId()) ;
  checkMastership() ;

  taskCurrentPatternRun.enable() ;
} // end setup()


// ************************************************************************
//                              LOOP
// ************************************************************************

void loop() {
  mesh.update();
}


// ************************************************************************
//                           MESH FUNCTIONS
// ************************************************************************

// Better to have static and keep the memory allocated or not??
void sendMessage() {
  if( ! tapTempo.isChainActive() or (currentPattern != nextPattern) ) {
    static StaticJsonDocument<256> msg;

    currentPattern = nextPattern ;       // update our own running pattern
    currentBPM     = tapTempo.getBPM() ; // update our BPM with (possibly new) BPM
    newBPMSet      = false ;             // reset the flag

    msg["currentBPM"]     = currentBPM;
    msg["currentPattern"] = currentPattern ;

    String str;
    serializeJson(msg, str);
    mesh.sendBroadcast(str);

    Serial.printf("%s (%u) %u: Sent broadcast message: ", role.c_str(), _nodePos, mesh.getNodeTime() );
    Serial.println(str);
    // Serial.printf("Brightness: %i\n", maxBright);
  } else {
    Serial.printf("%s %u: No msg to send.\tBPM: %u\tPattern: %u\n", role.c_str(), mesh.getNodeTime(), currentBPM, currentPattern );
  }
} // end sendMessage()



void checkMastership() {
  uint32_t masterNodeId = UINT32_MAX ;
  nodes = mesh.getNodeList(true);
  _numNodes = mesh.getNodeList().size();

  nodes.sort();

  SimpleList<uint32_t>::iterator nodeIterator = nodes.begin();   // It's a std::list

  // Print out a list of nodes and check which has the lowest Node ID:
  Serial.printf("Node list: ");

  // https://www.techiedelight.com/convert-list-to-array-cpp/
  int k = 0;
  for (int const &i: nodes) {
      if( i == mesh.getNodeId() ) {
        _nodePos = k;
      }
      if( i < masterNodeId ) {
        masterNodeId = i;
      }
      Serial.printf("%u ", i);
      k++;
  }
  Serial.printf("\n");

  _meshNumLeds = _numNodes * LEDS_PER_NODE;
  fx.setMeshNumLeds(_meshNumLeds);

  if( mesh.getNodeId() == masterNodeId ) {
    role = "MASTER";
    taskSendMessage.enableIfNot() ;
    taskCheckButtonPress.enableIfNot() ;
    taskSelectNextPattern.enableIfNot() ;
    Serial.printf("%s %u: checkMastership(); I am MASTER\n", role.c_str(), mesh.getNodeTime() );
  } else {
    role = "SLAVE" ;
    taskSendMessage.disable() ;         // Only MASTER sends broadcast
    taskCheckButtonPress.disable() ;    // Slaves can't set BPM
    taskSelectNextPattern.disable() ;   // Slaves wait for instructions from the MASTER
    Serial.printf("%s %u: checkMastership(); I am SLAVE\n", role.c_str(), mesh.getNodeTime() );
  }

  Serial.printf("\n%s %u: %d nodes. Mesh Leds: %d. NodePos: %u root: %d\n", role.c_str(), mesh.getNodeTime(), _numNodes, _meshNumLeds, _nodePos, mesh.isRoot());

} // end checkMastership()

boolean alone(){
  return mesh.getNodeList().size() <= 1 ;
}

void receivedCallback( uint32_t from, String &msg ) {
  // Serial.printf("Received msg from %u: %s\n", from, msg.c_str());
  if( role == "SLAVE" ) {
    static StaticJsonDocument<256> root;
    deserializeJson(root, msg);

    if( root["currentBPM"] ) {
      currentBPM      = root["currentBPM"].as<uint32_t>() ;
      nextPattern     = root["currentPattern"].as<uint8_t>() ;
      Serial.printf("%s (%u) %u: \tBPM: %u\t Pattern: %u\n", role.c_str(), _nodePos, mesh.getNodeTime(), currentBPM, currentPattern );
      Serial.printf("  (%d nodes). Mesh led count: %d\n", _numNodes, _meshNumLeds);
      // Serial.printf("%s %u: \t taskSendMessage: %d\t taskCheckButtonPress: %d\t taskSelectNextPattern: %d\n", role.c_str(),
      //   mesh.getNodeTime(), taskSendMessage.isEnabled(), taskCheckButtonPress.isEnabled(), taskSelectNextPattern.isEnabled() );
      tapTempo.setBPM(currentBPM);
    }

    // if( root["runOnce"] ) {
    //   executeOneCycle = true ;
    // }

  } else if( role == "MASTER") {
    static StaticJsonDocument<256> root;
    deserializeJson(root, msg);

    uint32_t patternRunTime = root["patternRunTime"].as<uint32_t>() ;
    Serial.printf("%s %u (slave end time): \tBPM: %u\t Pattern: %u\tRunTime: %u\n", role.c_str(), mesh.getNodeTime(), currentBPM, currentPattern, patternRunTime );
    //      taskRunPatternOnNode.forceNextIteration(); // Send message to next SLAVE
  }
  // Serial.printf("Copying %d leds from position %d\n", LEDS_PER_NODE, _nodePos*LEDS_PER_NODE );
  Serial.println(); // whitespace for easier reading
} // end receivedCallback()



void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("%s %u: New Connection from nodeId = %u\n", role.c_str(), mesh.getNodeTime(), nodeId);
  checkMastership() ;
}

void changedConnectionCallback() {
  Serial.printf("%s %u: Changed connections %s\n", role.c_str(), mesh.getNodeTime(), mesh.subConnectionJson().c_str());
  checkMastership() ;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("%s: Adjusted time %u. Offset = %d\n", role.c_str(), mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}


// ************************************************************************
//                       USER INTERFACE
// ************************************************************************


void checkButtonPress()
{
  bpmButton.read();
  nextPatternButton.read();

  if( bpmButton.wasPressed() ) {
    tapTempo.update(true); // update ArduinoTapTempo
    Serial.printf("%s %u: Button TAP. BPM: ", role.c_str(), mesh.getNodeTime() );
    Serial.println(tapTempo.getBPM() );
    newBPMSet = true ;
  } else {
    tapTempo.update(false);
  }

  if( nextPatternButton.wasPressed() ) {
    selectNextPattern();
  }

  // if( nextPatternButton.pressedFor(500)) {
  //   cycleBrightness();
  // }
}



// ************************************************************************
//                       LED PATTERN FUNCTIONS
// ************************************************************************

// This is where the magic happens!
// @Override This function is called by FastLED inside lib8tion.h. Requests it to use mesg.getNodeTime instead of internal millis() timer.
// Makes every pattern on each node synced!! So AWESOME!
uint32_t get_millisecond_timer() {
   return mesh.getNodeTime()/1000 ;
}


void currentPatternRun() {
  // Serial.print(".");

  // default
  // taskCurrentPatternRun.setInterval( CURRENTPATTERN_SELECT_DEFAULT_INTERVAL ) ;

  #ifdef ATOMMATRIX
    fx.heartbeat();
  #endif

  fx.spin();  
  taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND ) ;

  // if( currentPattern != nextPattern ) {
  //   // Serial.printf("%s %u: \t currentPattern: %i\t nextPattern: %i\n", role.c_str(), mesh.getNodeTime(), currentPattern, nextPattern );
  //   // fx.firstPatternIteration = true ; // FIXME/TODO move to Mesh
  //   currentPattern = nextPattern;
  //   // Serial.printf("%s %u: \t currentPattern: %i\t nextPattern: %i\n", role.c_str(), mesh.getNodeTime(), currentPattern, nextPattern );
  // }

  if ( strcmp(routines[currentPattern], "p_rb_stripe") == 0  ) {
    // fx.setCurrentPalette(RainbowStripeColors_p);
    // fx.FillLEDsFromPaletteColors() ;

    #ifdef RT_P_RB
  } else if ( strcmp(routines[currentPattern], "p_rb") == 0 ) {
    fx.setCurrentPalette(RainbowColors_p);
    fx.FillLEDsFromPaletteColors(1) ;
    #endif

    #ifdef RT_P_OCEAN
  } else if ( strcmp(routines[currentPattern], "p_ocean") == 0 ) {
    fx.setCurrentPalette(OceanColors_p);
    fx.FillLEDsFromPaletteColors();
    #endif

    #ifdef RT_P_HEAT
  } else if ( strcmp(routines[currentPattern], "p_heat") == 0 ) {
    fx.setCurrentPalette(HeatColors_p);
    fx.FillLEDsFromPaletteColors() ;
    #endif

    #ifdef RT_P_LAVA
  } else if ( strcmp(routines[currentPattern], "p_lava") == 0 ) {
    fx.setCurrentPalette(LavaColors_p);
    fx.FillLEDsFromPaletteColors() ;
    #endif

    #ifdef RT_P_PARTY
  } else if ( strcmp(routines[currentPattern], "p_party") == 0 ) {
    fx.setCurrentPalette(PartyColors_p);
    fx.FillLEDsFromPaletteColors() ;
    #endif

    #ifdef RT_P_CLOUD
  } else if ( strcmp(routines[currentPattern], "p_cloud") == 0 ) {
    fx.setCurrentPalette(CloudColors_p);
    fx.FillLEDsFromPaletteColors(6) ;
    #endif

    #ifdef RT_P_FOREST
  } else if ( strcmp(routines[currentPattern], "p_forest") == 0 ) {
    fx.setCurrentPalette(ForestColors_p);
    fx.FillLEDsFromPaletteColors() ;
    #endif

    #ifdef RT_TWIRL1
  } else if ( strcmp(routines[currentPattern], "twirl1") == 0 ) {
    fx.twirlers( 1, false ) ;
    taskCurrentPatternRun.setInterval( 1 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_TWIRL2
  } else if ( strcmp(routines[currentPattern], "twirl2") == 0 ) {
    fx.twirlers( 2, false ) ;
    taskCurrentPatternRun.setInterval( 1 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_TWIRL4
  } else if ( strcmp(routines[currentPattern], "twirl4") == 0 ) {
    fx.twirlers( 4, false ) ;
    taskCurrentPatternRun.setInterval( 1 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_TWIRL6
  } else if ( strcmp(routines[currentPattern], "twirl6") == 0 ) {
    fx.twirlers( 6, false ) ;
    taskCurrentPatternRun.setInterval( 1 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_TWIRL2_O
  } else if ( strcmp(routines[currentPattern], "twirl2o") == 0 ) {
    fx.twirlers( 2, true ) ;
    taskCurrentPatternRun.setInterval( 1 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_TWIRL4_O
  } else if ( strcmp(routines[currentPattern], "twirl4o") == 0 ) {
    fx.twirlers( 4, true ) ;
    taskCurrentPatternRun.setInterval( 1 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_TWIRL6_O
  } else if ( strcmp(routines[currentPattern], "twirl6o") == 0 ) {
    fx.twirlers( 6, true ) ;
    taskCurrentPatternRun.setInterval( 1 * TASK_MILLISECOND ) ;
    #endif

  #ifdef RT_FADE_GLITTER
  } else if ( strcmp(routines[currentPattern], "fglitter") == 0 ) {
    fx.fadeGlitter() ;
    #ifdef USING_MPU
        taskCurrentPatternRun.setInterval( map( constrain( activityLevel(), 0, 2500), 0, 2500, 40, 2 ) * TASK_RES_MULTIPLIER ) ;
    #else
        taskCurrentPatternRun.setInterval( 20 * TASK_RES_MULTIPLIER ) ;
    #endif
  #endif

  #ifdef RT_DISCO_GLITTER
  } else if ( strcmp(routines[currentPattern], "dglitter") == 0 ) {
    fx.discoGlitter() ;
    #ifdef USING_MPU
        taskCurrentPatternRun.setInterval( map( constrain( activityLevel(), 0, 2500), 0, 2500, 40, 2 ) * TASK_RES_MULTIPLIER ) ;
    #else
        taskCurrentPatternRun.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
    #endif
  #endif

  #ifdef RT_GLED
    // Gravity LED
  } else if ( strcmp(routines[currentPattern], "gled") == 0 ) {
    fx.gLed() ;
    taskCurrentPatternRun.setInterval( 5 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_BLACK
  } else if ( strcmp(routines[currentPattern], "black") == 0 ) {
    fill_solid(leds, _meshNumLeds, CRGB::Black);
    // FastLED.show();
    taskCurrentPatternRun.setInterval( 500 * TASK_MILLISECOND ) ;  // long because nothing is going on anyways.
    #endif

    #ifdef RT_RACERS
  } else if ( strcmp(routines[currentPattern], "racers") == 0 ) {
    fx.racingLeds() ;
    taskCurrentPatternRun.setInterval( 8 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_WAVE
  } else if ( strcmp(routines[currentPattern], "wave") == 0 ) {
    fx.waveYourArms() ;
    taskCurrentPatternRun.setInterval( 15 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_SHAKE_IT
  } else if ( strcmp(routines[currentPattern], "shakeit") == 0 ) {
    fx.shakeIt() ;
    taskCurrentPatternRun.setInterval( 8 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_STROBE1
  } else if ( strcmp(routines[currentPattern], "strobe1") == 0 ) {
    fx.strobe1() ;
    taskCurrentPatternRun.setInterval( 1 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_STROBE2
  } else if ( strcmp(routines[currentPattern], "strobe2") == 0 ) {
    fx.strobe2() ;
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_HEARTBEAT
  } else if ( strcmp(routines[currentPattern], "heartbeat") == 0 ) {
    fx.heartbeat() ;
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
    fx.fastLoop( false ) ;
    #endif

    #ifdef RT_FASTLOOP2
  } else if ( strcmp(routines[currentPattern], "fastloop2") == 0 ) {
    fx.fastLoop( true ) ;
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND) ;
    #endif

    #ifdef RT_PENDULUM
  } else if ( strcmp(routines[currentPattern], "pendulum") == 0 ) {
    fx.pendulum() ;
    //    taskCurrentPatternRun.setInterval( 1500 ) ; // needs a fast refresh rate - optimal in microseconds
    taskCurrentPatternRun.setInterval( 2 ) ; // needs a fast refresh rate
    #endif

    #ifdef RT_BOUNCEBLEND
  } else if ( strcmp(routines[currentPattern], "bounceblend") == 0 ) {
    fx.bounceBlend() ;
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
    fx.setCurrentPalette(LavaColors_p);
    fx.fillnoise8( beatsin8( fx.getTempo(), 1, 25), 30, 1); // pallette, speed, scale, loop
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_NOISE_PARTY
  } else if ( strcmp(routines[currentPattern], "noise_party") == 0 ) {
    fx.setCurrentPalette(PartyColors_p);
    fx.fillnoise8( beatsin8( fx.getTempo(), 1, 25), 30, 1); 
    //    taskCurrentPatternRun.setInterval( beatsin16( tapTempo.getBPM(), 2000, 50000) ) ;
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_QUAD_STROBE
  } else if ( strcmp(routines[currentPattern], "quadstrobe") == 0 ) {
    fx.quadStrobe();
    taskCurrentPatternRun.setInterval( (60000 / (tapTempo.getBPM() * 4)) * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_PULSE_3
  } else if ( strcmp(routines[currentPattern], "pulse3") == 0 ) {
    fx.pulse3();
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_PULSE_5
  } else if ( strcmp(routines[currentPattern], "pulse5") == 0 ) {
    fx.pulse5(3, false);
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_THREE_SIN_PAL
  } else if ( strcmp(routines[currentPattern], "tsp") == 0 ) {
    fx.threeSinPal() ;
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_FIRE_STRIPE
  } else if ( strcmp(routines[currentPattern], "firestripe") == 0 ) {
    fx.fireStripe() ;
    taskCurrentPatternRun.setInterval( 10 * TASK_MILLISECOND ) ;
    #endif

    #ifdef RT_CYLON
  } else if ( strcmp(routines[currentPattern], "cylon") == 0 ) {
    fx.cylon() ;
    taskCurrentPatternRun.setInterval( 1 * TASK_MILLISECOND ) ;
    #endif
  }

  // Serial.print(".");

    // COPY 
    memcpy( &_localLeds, &_meshleds[_nodePos*LEDS_PER_NODE], sizeof(CRGB)*LEDS_PER_NODE ); 
    if( role == "MASTER") {
      _localLeds[_nodePos] = CRGB::Purple;
    } else {
      _localLeds[_nodePos] = CRGB::Green;
    }
    _driver.showPixels();
}

void selectNextPattern() {
  nextPattern = currentPattern + 1 ;

  if (nextPattern >= NUMROUTINES ) {
    nextPattern = 0;
  }

  if( role == "MASTER" ) {
     taskSendMessage.forceNextIteration(); // Schedule next iteration immediately, for sending a new pattern msg to slaves
  }
}