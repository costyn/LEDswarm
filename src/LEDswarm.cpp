
#include "painlessMesh.h"
#include "ArduinoTapTempo.h"  // pio lib [--global] install https://github.com/dxinteractive/ArduinoTapTempo.git


#ifdef INCLUDE_LEDS
#include "FastLED.h"
#endif

#define   MESH_PREFIX     "LEDforge.com LEDswarm"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

// #define TIME_SYNC_INTERVAL  6000000  // Time resync period, in us. 600 sec = 10 min

#define   DEFAULT_BPM       120
#define   DEFAULT_PATTERN   0

#ifdef INCLUDE_LEDS
#define    NUM_LEDS 5
#define   DATA_PIN 2
CRGB leds[NUM_LEDS];
#endif

painlessMesh  mesh;
SimpleList<uint32_t> nodes;
uint32_t sendMessageTime = 0; // how often to send broadcast messages
String role ;

// prototype methods:
void receivedCallback( uint32_t from, String &msg );
void newConnectionCallback(uint32_t nodeId) ;
void changedConnectionCallback() ;
void nodeTimeAdjustedCallback(int32_t offset) ;
void delayReceivedCallback(uint32_t from, int32_t delay) ;
void setTapTempo() ;

uint8_t currentPatternId = DEFAULT_PATTERN ;
uint32_t currentBeatLength = 60000 / DEFAULT_BPM ;

ArduinoTapTempo tapTempo;
uint32_t tapTimer = 0 ;


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

#ifdef INCLUDE_LEDS
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
#endif

  Serial.print("Starting up... I am: ");
  Serial.println(mesh.getNodeId()) ;
}


uint32_t getMasterNodeId() {
  uint32_t lowestId = UINT32_MAX ;
  SimpleList<uint32_t>::iterator node = nodes.begin();
  while ( node != nodes.end() ) {
    if (  *node < lowestId ) {
      lowestId = *node ;
    }
    node++;
  }
  return lowestId ;
}

bool thisNodeMaster() {
  return mesh.getNodeId() == getMasterNodeId() ;
}


void loop() {
  mesh.update();

  if( tapTimer < mesh.getNodeTime() ) {
    tapTempo.update(true);
    // Rounds it up to the next whole 1
    tapTimer = mesh.getNodeTime() - (mesh.getNodeTime() % (currentBeatLength*1000)) + (currentBeatLength*1000) ;
    Serial.printf("%s %u: TAP. New tapTime: %u. BPM: ", role.c_str(), mesh.getNodeTime(), tapTimer );
    Serial.println(tapTempo.getBPM() );
  } else {
    tapTempo.update(false);
  }

  if ( thisNodeMaster() ) {
    role = "MASTER" ;
    // send broadcast messages with BPM and pattern-id and next patternstarttime
    if (sendMessageTime == 0) {
      sendMessageTime = mesh.getNodeTime() + 5000000 ; // send a message every 200 ms
    }

    if (sendMessageTime != 0 && (int)sendMessageTime - (int)mesh.getNodeTime() < 0) { // Cast to int in case of time rollover
      DynamicJsonBuffer jsonBuffer;
      JsonObject& msg = jsonBuffer.createObject();

      msg["currentPatternId"]  = currentPatternId;
      msg["currentBeatLength"] = currentBeatLength;
      msg["taptimer"]          = tapTimer ;

      String str;
      msg.printTo(str);
      mesh.sendBroadcast(str);
      sendMessageTime = 0;

      Serial.printf("%s %u: Sent broadcast message: ", role.c_str(), mesh.getNodeTime() );
      Serial.println(str);
    }
  } else {
    role = "SLAVE" ;
  }
}

void receivedCallback( uint32_t from, String &msg ) {
  //  Serial.printf("Received msg from %u: %s\n", from, msg.c_str());
  if ( ! thisNodeMaster() ) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root  = jsonBuffer.parseObject(msg);
    currentPatternId  = root["currentPatternId"].as<uint8_t>() ;
    currentBeatLength = root["currentBeatLength"].as<uint32_t>() ;
    Serial.printf("SLAVE %u: from: %u \tpattern: %u \tBL: %u\n", mesh.getNodeTime(), from, currentPatternId, currentBeatLength  );

    if( tapTimer != root["taptimer"].as<uint32_t>() ) {
      tapTimer = root["taptimer"].as<uint32_t>() ;
      Serial.printf("SLAVE %u: Unsynced with MASTER. Next tapTime (from master): %u\n", mesh.getNodeTime(), tapTimer ) ;
    } else {
      Serial.printf("SLAVE %u: In SYNC with MASTER. Next tapTime: %u\n", mesh.getNodeTime(), tapTimer );
    }

  } else {
    Serial.printf("received msg from %u but I'm the master\n", from) ;
  }

  Serial.println(); // whitespace for easier reading

}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("-- > startHere: New Connection, nodeId = %u\n", nodeId);
  nodes = mesh.getNodeList();
  nodes.push_back(mesh.getNodeId()); // add ourselves to the nodes list
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());

  nodes = mesh.getNodeList();
  nodes.push_back(mesh.getNodeId()); // add ourselves to the nodes list

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list: ");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}
