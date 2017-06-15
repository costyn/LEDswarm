#include <arduino.h>

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



// Custom mod which always returns a positive number
int mod(int x, int m) {
  return (x % m + m) % m;
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
