// TODO: convert Serial.printfs and println's to DEBUG statements we can turn on/off
// UI/UX:
// * Set BPM
// * Set Pattern
// * Set Brightness

#define LEDSWARM_DEBUG

#define TIME_SYNC_INTERVAL  60000000  // Mesh time resync period, in us. 1 minute
// #define FASTLED_ALLOW_INTERRUPTS 1    // Allow interrupts, to prevent wifi weirdness ; https://github.com/FastLED/FastLED/wiki/Interrupt-problems
// #define INTERRUPT_THRESHOLD 1   // also see https://github.com/FastLED/FastLED/issues/367
#define USE_GET_MILLISECOND_TIMER     // Define our own millis() source for FastLED beat functions: see get_millisecond_timer()
#define TASK_RES_MULTIPLIER  1


#include <painlessMesh.h>
#include <ArduinoTapTempo.h>  // pio lib [--global] install ArduinoTapTempo
#include <FastLED.h>
#include <easing.h>
#include <JC_Button.h>          // https://github.com/JChristensen/JC_Button

#ifdef LEDSWARM_DEBUG
#define DEBUG_PRINT(x)       Serial.print (x)
#define DEBUG_PRINTDEC(x)    Serial.print (x, DEC)
#define DEBUG_PRINTLN(x)     Serial.println (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x)
#endif

#define   MESH_SSID       "LEDforge"
#define   MESH_PASSWORD     "somethingSneaky"
#define   MESH_PORT         5555



#if defined(NEO_PIXEL) || defined(NEO_PIXEL_MULTI)
#define CHIPSET     WS2812B
#define COLOR_ORDER GRB
#endif


#if defined(APA_102) || defined(APA_102_SLOW)
#include <SPI.h>
#define CHIPSET     APA102
#define COLOR_ORDER BGR
#endif


// LED variables
CRGB      leds[NUM_LEDS];
CRGB      matrix_leds[ATOM_NUM_LED];
uint8_t   maxBright = DEFAULT_BRIGHTNESS ;
uint8_t  currentPattern = DEFAULT_PATTERN ; // Which mode do we start with
uint8_t  nextPattern    = currentPattern ;
bool     firstPatternIteration = true ;    // if this pattern is being run for the first time
uint8_t   currentBrightness = maxBright ;

// Prototypes
void sendMessage();
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);

// Mesh variables
painlessMesh  mesh;
SimpleList<uint32_t> nodes;
String role = "MASTER" ; // default start out as master unless told otherwise
uint32_t activeSlave ;

//Scheduler
Scheduler userScheduler; // to control your personal task

// BPM variables
ArduinoTapTempo tapTempo;
bool newBPMSet = true ;     // flag for when new BPM is set by button
uint32_t currentBPM = 120 ; // default BPM of ArduinoTapTempo

Button bpmButton(BPM_BUTTON_PIN);
// Button bpmButton(BPM_BUTTON_PIN, 50, true, true);
Button nextPatternButton(BUTTON_PIN);

// Task variables
#define TASK_CHECK_BUTTON_PRESS_INTERVAL    100   // in milliseconds
#define CURRENTPATTERN_SELECT_DEFAULT_INTERVAL     1   // default scheduling time for currentPatternSELECT, in milliseconds
Task taskCheckButtonPress( TASK_CHECK_BUTTON_PRESS_INTERVAL, TASK_FOREVER, &checkButtonPress);
Task taskCurrentPatternRun( CURRENTPATTERN_SELECT_DEFAULT_INTERVAL, TASK_FOREVER, &currentPatternRun);
Task taskSendMessage( TASK_SECOND * 5, TASK_FOREVER, &sendMessage ); // check every second if we have a new BPM / pattern to send
Task taskSelectNextPattern( TASK_SECOND * 15, TASK_FOREVER, &selectNextPattern);  // switch to next pattern every 15 seconds
//Task taskRunPatternOnNode( TASK_IMMEDIATE, TASK_ONCE, &runPatternOnNode );

void setup() {
  Serial.begin(115200);
  delay(1000); // Startup delay; let things settle down

  // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  //mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION | DEBUG );  // set before init() so that you can see startup messages
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION | SYNC );  // set before init() so that you can see startup messages
  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

  // nodes.push_back( mesh.getNodeId() ) ; // add our own ID to the list of nodes

#ifdef APA_102
  FastLED.addLeds<CHIPSET, MY_DATA_PIN, MY_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(12)>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
#else
  FastLED.addLeds<CHIPSET, LED_PIN_1, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.addLeds<CHIPSET, ATOM_LEDPIN, COLOR_ORDER>(matrix_leds, ATOM_NUM_LED);
#endif

  userScheduler.addTask( taskSendMessage );
  userScheduler.addTask( taskCheckButtonPress );
  userScheduler.addTask( taskCurrentPatternRun );
  taskCheckButtonPress.enable() ;
  taskCurrentPatternRun.enable() ;

#ifdef AUTOADVANCE
  userScheduler.addTask( taskSelectNextPattern );
  taskSelectNextPattern.enable() ;
#endif

  // pinMode(BPM_BUTTON_PIN, INPUT_PULLUP);
  bpmButton.begin();
  nextPatternButton.begin();

  Serial.print("Starting up... my Node ID is: ");
  Serial.println(mesh.getNodeId()) ;
  checkMastership() ;
} // end setup()


void loop() {
  // yield() ;
  mesh.update();
} // end loop()


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

    Serial.printf("%s %u: Sent broadcast message: ", role.c_str(), mesh.getNodeTime() );
    Serial.println(str);
    // Serial.printf("Brightness: %i\n", maxBright);
  } else {
    Serial.printf("%s %u: No msg to send.\tBPM: %u\tPattern: %u\n", role.c_str(), mesh.getNodeTime(), currentBPM, currentPattern );
  }
} // end sendMessage()


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



// @Override This function is called by FastLED inside lib8tion.h. Requests it to use mesg.getNodeTime instead of internal millis() timer.
// Makes every pattern on each node synced!! So awesome!
uint32_t get_millisecond_timer() {
   return mesh.getNodeTime()/1000 ;
}


// MASTER sends pattern message to node x direct message
// MASTER shifts node off list and adds it to the end
// Node x sends back timing for pattern
// MASTER keeps running average for pattern, sends out new patterns on time to next node

// uint32_t runPatternOnNode() {
//   static uint32_t sendTime ;
//   auto node = nodes.begin();
//   nodes.pop_front();
//   nodes.push_back(node);
//
//   static DynamicJsonBuffer jsonBuffer;
//   static JsonObject& msg = jsonBuffer.createObject();
//
//   msg["runOnce"] = currentPattern;
//
//   String str;
//   msg.printTo(str);
//   mesh.sendSingle(node, str);
//   sendTime = mesh.getNodeTime() ;
//   Serial.printf("%s %u (slave start time): \tBPM: %u\t Pattern: %u\n", role.c_str(), sendTime, currentBPM, currentPattern );
//   activeSlave = node ;  // keep track of which SLAVE is currently running a pattern
// }


// OPtion B: define CRGB array for the number of LEDS
