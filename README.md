# LEDswarm
ESP8266/ESP32 based mesh to drive and synchronize a swarm of mobile LEDs

Using these awesome libraries:
* painlessMesh
* FastLED
* TaskScheduler

## Features
* Mesh master election
* Automatic synchronisation of pattern, pattern timing and BPM (for bpm based patterns).
* Each node will work standalone when out of range of the rest (it will become a local master)

## Usage
* Make a new src/headers/<project>.h file with your preferences.
* Edit platformio.ini with your board details and point to the header file
* Compile & upload
