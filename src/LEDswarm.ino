// #define TIME_SYNC_INTERVAL  6000000  // Time resync period, in us. 600 sec = 10 min
#define _TASK_MICRO_RES     // Turn on microsecond timing

#include "LEDswarm.h"
#include "painlessMesh.h"
#include "ArduinoTapTempo.h"  // pio lib [--global] install https://github.com/dxinteractive/ArduinoTapTempo.git
#include "FastLED.h"

#define   BUTTON_PIN        0

#define   MESH_PREFIX       "LEDforge.com LEDswarm"
#define   MESH_PASSWORD     "somethingSneaky"
#define   MESH_PORT         5555

#define   DEFAULT_BPM       120
#define   DEFAULT_PATTERN   0

#define   DEFAULT_BRIGHTNESS  100  // 0-255, higher number is brighter.
#define   NUM_LEDS          30
#define   DATA_PIN          5
CRGB leds[NUM_LEDS];
uint8_t maxBright = DEFAULT_BRIGHTNESS ;


painlessMesh  mesh;
SimpleList<uint32_t> nodes;
uint32_t sendMessageTime = 0; // how often to send broadcast messages
String role ;

uint8_t currentPatternId = DEFAULT_PATTERN ;
uint32_t currentBeatLength = 60000 / DEFAULT_BPM ;

ArduinoTapTempo tapTempo;
uint32_t tapTimer = 0 ;
#define HOLD_TIME 1000000   // wait time before tapping in new BPM, in uS
uint32_t holdTimer = 0 ;

Task taskSendBeatSync( 5000 * 1000 , TASK_FOREVER, &sendBeatSync ); // send a Beat sync message every 5 seconds

void setup() {
  Serial.begin(115200);

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

  nodes.push_back( mesh.getNodeId() ) ;

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  mesh.scheduler.addTask( taskSendBeatSync );

  Serial.print("Starting up... I am: ");
  Serial.println(mesh.getNodeId()) ;

  pinMode(BUTTON_PIN, INPUT_PULLUP);
}



#define SHORT_PRESS_MIN_TIME 50   // minimum time for a short press - debounce

void loop() {
  mesh.update();
  checkButtonPress(); // TODO: taskify this

  // TODO: taskify this:
  if( tapTimer < mesh.getNodeTime() and holdTimer < mesh.getNodeTime() ) {
    tapTempo.update(true);
    // Rounds it up to the next whole 1
    tapTimer = mesh.getNodeTime() - (mesh.getNodeTime() % (currentBeatLength*1000)) + (currentBeatLength*1000) ;
    Serial.printf("%s %u: Auto TAP. New tapTime: %u. BPM: ", role.c_str(), mesh.getNodeTime(), tapTimer );
    Serial.println(tapTempo.getBPM() );
  } else {
    tapTempo.update(false);
  }

  if ( thisNodeMaster() ) {
    role = "MASTER" ;
    // send broadcast messages with BPM and pattern-id and next patternstarttime
    taskSendBeatSync.enableIfNot() ;
  } else {
    role = "SLAVE" ;
    taskSendBeatSync.disable() ;
  }
}

// Better to have static and keep the memory allocated or not??
void sendBeatSync() {
  static DynamicJsonBuffer jsonBuffer;
  static JsonObject& msg = jsonBuffer.createObject();

  msg["currentBeatLength"] = currentBeatLength;
  msg["taptimer"]          = tapTimer ;

  String str;
  msg.printTo(str);
  mesh.sendBroadcast(str);

  Serial.printf("%s %u: Sent broadcast message: ", role.c_str(), mesh.getNodeTime() );
  Serial.println(str);
}


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
        holdTimer = mesh.getNodeTime() + HOLD_TIME ;
      }
    }
  }
}
