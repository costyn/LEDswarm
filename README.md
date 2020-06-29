# LEDswarm
ESP8266/ESP32 based mesh to drive and synchronize a swarm of mobile LEDs

__VIDEO: https://www.youtube.com/watch?v=Uttsf_RPKiI__

Using these awesome libraries:
* painlessMesh: https://gitlab.com/painlessMesh
* FastLED: https://github.com/FastLED/FastLED
* TaskScheduler: https://github.com/arkhipenko/TaskScheduler
* ArduinoTapTempo: https://github.com/dxinteractive/ArduinoTapTempo

## Features
* No central node needed
* Automatic joining of wifi mesh
* Mesh master election
* Automatic synchronisation of pattern, pattern timing and BPM (for bpm based patterns).
* Each node will work standalone when out of range of the rest (it will become it's own master)

## Usage
* Make a new src/headers/<project>.h file with your preferences (see other files for examples)
* Edit platformio.ini with your board details and point to the header file (see other examples)
* Compile & upload

## TODO (Wish) list
* Multi-node sequential patterns: nodes handoff patterns to each other in sequence
* Control via an app or other interface
