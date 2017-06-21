// TODO: convert Serial.printfs and println's to DEBUG statements we can turn on/off

// #define TIME_SYNC_INTERVAL  6000000  // Mesh time resync period, in us. 600 sec = 10 min
//#define _TASK_MICRO_RES     // Turn on microsecond timing - painlessMesh does not support it (yet)
#define FASTLED_ALLOW_INTERRUPTS 0
#define USE_GET_MILLISECOND_TIMER   // define our own millis() source for FastLED beat functions, in this case from mesh.getNodeTime

#include "LEDswarm.h"
#include "painlessMesh.h"
#include "ArduinoTapTempo.h"  // pio lib [--global] install https://github.com/dxinteractive/ArduinoTapTempo.git
#include "FastLED.h"

#ifdef    _TASK_MICRO_RES
#define   TASK_RES_MULTIPLIER   1000
#else
#define   TASK_RES_MULTIPLIER   1
#endif

#define   BUTTON_PIN        0

#define   MESH_PREFIX       "LEDforge.com LEDswarm"
#define   MESH_PASSWORD     "somethingSneaky"
#define   MESH_PORT         5555

#define   DEFAULT_BPM       120
#define   DEFAULT_PATTERN   0

#define   DEFAULT_BRIGHTNESS  80  // 0-255, higher number is brighter.
#define   NUM_LEDS          30
#define   DATA_PIN          2

CRGB leds[NUM_LEDS];
uint8_t maxBright = DEFAULT_BRIGHTNESS ;

uint8_t  currentPattern = DEFAULT_PATTERN ; // Which mode do we start with
uint8_t  nextPattern    = currentPattern ; // Which pattern are we going to do next
bool     firstPatternIteration = true ;    // if this pattern is being run for the first time

painlessMesh  mesh;
SimpleList<uint32_t> nodes;
String role = "MASTER" ; // default start out as master unless told otherwise

uint32_t currentBeatLength = 60000 / DEFAULT_BPM ;

ArduinoTapTempo tapTempo;
uint32_t tapTimer = 0 ;
#define HOLD_TIME 1000000   // wait time before auto tapping in new BPM, in uS
uint32_t holdTimer = 0 ;


#define TASK_CHECK_BUTTON_PRESS_INTERVAL    10   // in milliseconds
#define CURRENTPATTERN_SELECT_DEFAULT_INTERVAL     50   // default scheduling time for currentPatternSELECT, in milliseconds
Task taskCheckButtonPress( TASK_CHECK_BUTTON_PRESS_INTERVAL, TASK_FOREVER, &checkButtonPress);
Task taskSendBeatSync( TASK_SECOND * 5 , TASK_FOREVER, &sendBeatSync ); // send a Beat sync message every 5 seconds
Task taskCurrentPatternSelect( CURRENTPATTERN_SELECT_DEFAULT_INTERVAL, TASK_FOREVER, &currentPatternSelect);
Task taskSelectNextPattern( TASK_SECOND * 15, TASK_FOREVER, &selectNextPattern);  // switch to next pattern every 15 seconds

void setup() {
  Serial.begin(115200);
  delay(1000); // Startup delay; let things settle down

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

  nodes.push_back( mesh.getNodeId() ) ; // add our own ID to the list of nodes

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  mesh.scheduler.addTask( taskSendBeatSync );
  mesh.scheduler.addTask( taskCheckButtonPress );
  mesh.scheduler.addTask( taskCurrentPatternSelect );
  mesh.scheduler.addTask( taskSelectNextPattern );
  taskCheckButtonPress.enable() ;
  taskCurrentPatternSelect.enable() ;
  taskSelectNextPattern.enable() ;


  Serial.print("Starting up... I am: ");
  Serial.println(mesh.getNodeId()) ;
}



#define SHORT_PRESS_MIN_TIME 50   // minimum time for a short press - debounce

void loop() {
  mesh.update();

  // Can't taskify this because tapTimer is a mesh Node Time time stamp that's sent to other nodes, not a delay or interval time.
  if( tapTimer < mesh.getNodeTime() and holdTimer < mesh.getNodeTime() ) {
    tapTempo.update(true);
    // Rounds it up... or it should. For some reason it likes a lot of 9's at the end.
    tapTimer = mesh.getNodeTime() - (mesh.getNodeTime() % (currentBeatLength*1000)) + (currentBeatLength*1000) ;
    Serial.printf("%s %u: Auto TAP. New tapTime: %u. BPM: ", role.c_str(), mesh.getNodeTime(), tapTimer );
    Serial.println( tapTempo.getBPM() );

    if( currentPattern != nextPattern ) {
      currentPattern = nextPattern ;
      taskCurrentPatternSelect.forceNextIteration(); // schedule it immediately
      firstPatternIteration = true ;
      Serial.printf("%s %u: Going to next pattern: %u\n", role.c_str(), mesh.getNodeTime(), currentPattern );
    }
  } else {
    tapTempo.update(false);
  }
}
// Better to have static and keep the memory allocated or not??
void sendBeatSync() {
  static DynamicJsonBuffer jsonBuffer;
  static JsonObject& msg = jsonBuffer.createObject();

  msg["currentBeatLength"] = currentBeatLength;
  msg["taptimer"]          = tapTimer ;
  msg["nextPattern"]       = nextPattern ;

  String str;
  msg.printTo(str);
  mesh.sendBroadcast(str);

  Serial.printf("%s %u: Sent broadcast message: ", role.c_str(), mesh.getNodeTime() );
  Serial.println(str);
}

// TODO: broadcast new BPM when tapChain ends
void checkButtonPress() {
  static unsigned long buttonTimer = 0;
  static bool buttonActive = false;

  if( digitalRead(BUTTON_PIN) == LOW ) {
    if (buttonActive == false) {
      buttonActive = true;
      buttonTimer = millis();
    }
  } else {
    if (buttonActive == true) {
      buttonActive = false; // reset
      if ( millis() - buttonTimer > SHORT_PRESS_MIN_TIME ) {    // test if debounce is reached
        tapTempo.update(true); // update ArduinoTapTempo
        currentBeatLength = tapTempo.getBeatLength();
        Serial.printf("%s %u: Button TAP. %u. BPM: ", role.c_str(), mesh.getNodeTime(), currentBeatLength );
        Serial.println(tapTempo.getBPM() );
        holdTimer = mesh.getNodeTime() + HOLD_TIME ; // replace holddtimer with task.delay
      }
    }
  }
}

uint32_t get_millisecond_timer() {
   return mesh.getNodeTime()/1000 ;
}
