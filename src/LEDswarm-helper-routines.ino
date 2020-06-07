// Fill a gradient on a LED ring with any possible start positions.
// startLed and endLed may be negative (one or both), may be larger than NUM_LEDS (one or both)
// TODO:
// * remove floating point calculation; replace by linear interpolation?

void fillGradientRing( int startLed, CHSV startColor, int endLed, CHSV endColor ) {
  // Determine actual start and actual end (normalize using modulo):
  int actualStart = mod(startLed + NUM_LEDS, NUM_LEDS)  ;
  int actualEnd = mod(endLed + NUM_LEDS, NUM_LEDS) ;

  // If beginning is at say 50, and end at 10, then we split the gradient in 2:
  // * one from 50-59
  // * one from 0-10
  // To determine which color should be at 59 and 0 we use the blend function:
  if ( actualStart > actualEnd ) {
    float ratio = 1.0 - float(actualEnd) / float(endLed - startLed) ; // cast to float otherwise the division won't work
    int normalizedRatio = round( ratio * 255 ) ; // determine what ratio of startColor and endColor we need at LED 0
    CHSV colorAtLEDZero = blend(startColor, endColor, normalizedRatio);

    fill_gradient(leds, actualStart, startColor, NUM_LEDS - 1, colorAtLEDZero, SHORTEST_HUES);
    fill_gradient(leds, 0, colorAtLEDZero, actualEnd, endColor, SHORTEST_HUES);
  } else {
    fill_gradient(leds, actualStart, startColor, actualEnd, endColor, SHORTEST_HUES);
  }
}

// This is a little convoluted and could probably be written better :)
void fillSolidRing( int startLed, int endLed, CHSV color ) {
  // Determine actual start and actual end (normalize using custom modulo):
  int actualStart = mod(startLed + NUM_LEDS, NUM_LEDS)  ;
  int actualEnd = mod(endLed + NUM_LEDS, NUM_LEDS) ;

  // If beginning is at say 50, and end at 10, then we split the fill in 2:
  // * one from 50-59
  // * one from 0-10
  if ( actualStart > actualEnd ) {
    fill_solid(leds + actualStart, NUM_LEDS - actualStart, color);
    fill_solid(leds, actualEnd, color);
  } else {
    fill_solid(leds + actualStart, actualEnd - actualStart, color);
  }
} // end fillSolidRing()


#ifdef USING_MPU

// offset for aligning gyro "bottom" with LED "bottom" - depends on orientation of ring led 0 vs gyro - determine experimentally
#define OFFSET -4

// This routine needs pitch/roll information in floats, so we need to retrieve it separately
//  Suggestions how to fix this/clean it up welcome.
int lowestPoint() {
  Quaternion quat;        // [w, x, y, z]         quaternion container
  VectorFloat gravity;    // [x, y, z]            gravity vector
  float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

  mpu.dmpGetQuaternion(&quat, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &quat);
  mpu.dmpGetYawPitchRoll(ypr, &quat, &gravity);

  float myYprP = (ypr[1] * 180 / M_PI) ;  // Convert from radians to degrees:
  float myYprR = (ypr[2] * 180 / M_PI) ;

  int targetLedPos = 0 ;
  static int currentLedPos = 0 ;
  int ratio = 0 ;
  int mySpeed = round( (abs(myYprP) + abs(myYprR) ) * 100 );
  taskLedModeSelect.setInterval( map( mySpeed, 0, 9000, 25, 5) ) ; // Why did I do this? So weird

  if ( myYprR < 0 and myYprP < 0 ) {
    ratio =  (abs( myYprP ) / (abs(myYprP) + abs(myYprR))) * 100 ;
    targetLedPos = map( ratio, 0, 100, 0 , 14 );

  } else if ( myYprR > 0 and myYprP < 0 ) {
    ratio =  (abs( myYprR ) / (abs(myYprP) + abs(myYprR))) * 100 ;
    targetLedPos = map( ratio, 0, 100, 15 , 29 );

  } else if ( myYprR > 0 and myYprP > 0 ) {
    ratio =  (abs( myYprP ) / (abs(myYprP) + abs(myYprR))) * 100 ;
    targetLedPos = map( ratio, 0, 100, 30 , 44 );

  } else if ( myYprR < 0 and myYprP > 0 ) {
    ratio =  (abs( myYprR ) / (abs(myYprP) + abs(myYprR))) * 100 ;
    targetLedPos = map( ratio, 0, 100, 45 , 60 );
  } else {
    DEBUG_PRINTLN(F("\tWTF\t")) ;  // This should never happen
  }
  targetLedPos = mod(targetLedPos + OFFSET, NUM_LEDS) ;

  if ( currentLedPos != targetLedPos ) {
    bool goClockwise = true ;

    // http://stackoverflow.com/questions/7428718/algorithm-or-formula-for-the-shortest-direction-of-travel-between-two-degrees-on

    if ( mod(targetLedPos - currentLedPos + NUM_LEDS, NUM_LEDS) < NUM_LEDS / 2) {  // custom modulo
      goClockwise = true ;
    } else {
      goClockwise = false  ;
    }

    // TODO: add QuadraticEaseInOut for movement
    if ( goClockwise ) {
      currentLedPos++ ;
      if ( currentLedPos > NUM_LEDS - 1 ) {
        currentLedPos = 0 ;
      }
    } else {
      currentLedPos-- ;
      if ( currentLedPos < 0 ) {
        currentLedPos = NUM_LEDS - 1 ;
      }
    }

  }

  return currentLedPos ;
}
#endif

void fadeall(uint8_t fade_all_speed) {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(fade_all_speed);
  }
}

void brightall(uint8_t bright_all_speed) {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    //leds[i] += leds[i].scale8(bright_all_speed) ;
    leds[i] += brighten8_video(bright_all_speed) ;
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

void addColorGlitter( fract8 chanceOfGlitter)
{
  for ( uint8_t i = 0 ; i < 5 ; i++ ) {
    if ( random8() < chanceOfGlitter) {
      leds[ random16(NUM_LEDS) ] += CHSV(beat8(55), 255, 255);
    }
  }
}


#define LONG_PRESS_MIN_TIME 500  // minimum time for a long press
// #define SHORT_PRESS_MIN_TIME 70   // minimum time for a short press - debounce

// #ifdef BUTTON_PIN
//
// // longPressActive
// // advance ledMode
// // TODO: Replace horror show below with something like https://github.com/mathertel/OneButton
//
// void checkButtonPress() {
//   static unsigned long buttonTimer = 0;
//   static boolean buttonActive = false;
//
//   if (digitalRead(BUTTON_PIN) == LOW) {
//     // Start the timer
//     if (buttonActive == false) {
//       buttonActive = true;
//       buttonTimer = millis();
//     }
//
//     // If timer has passed longPressTime, set longPressActive to true
//     if ((millis() - buttonTimer > LONG_PRESS_MIN_TIME) && (longPressActive == false)) {
//       longPressActive = true;
//     }
//
// #ifndef USING_MPU
//     if( longPressActive == true ) {
//           cycleBrightness() ;
//     }
// #endif
//
//
//   } else {
//     // Reset when button is no longer pressed
//     if (buttonActive == true) {
//       buttonActive = false;
//       if (longPressActive == true) {
//         longPressActive = false;
// //        taskLedModeSelect.enableIfNot() ;
//       } else {
//         if ( millis() - buttonTimer > SHORT_PRESS_MIN_TIME ) {
//           nextLedMode() ;
//         }
//       }
//     }
//   }
//
//   #ifdef BPM_BUTTON_PIN
//   if (digitalRead(BPM_BUTTON_PIN) == LOW) {
//     tapTempo.update(true);
// //    DEBUG_PRINTLN( tapTempo.getBPM() ) ;
//   } else {
//     tapTempo.update(false);
//   }
//   #endif
//
//   serialEvent() ;
//   if (stringComplete) {
//     if( inputString.charAt(0) == 'p' ) {
//       inputString.remove(0,1);
//       uint8_t input = inputString.toInt();
//       if( input < NUMROUTINES ) {
//         ledMode = inputString.toInt();
//       } else {
//         DEBUG_PRINT("Ignoring input; value too high. NUMROUTINES: ");
//         DEBUG_PRINTLN( NUMROUTINES ) ;
//       }
//       DEBUG_PRINT("LedMode: ");
//       DEBUG_PRINTLN( ledMode ) ;
//     }
//     if( inputString.charAt(0) == 'b' ) {
//       inputString.remove(0,1);
//       tapTempo.setBPM(inputString.toInt());
//       DEBUG_PRINT("BPM: ");
//       DEBUG_PRINTLN( inputString.toInt() ) ;
//     }
//     if( inputString.charAt(0) == 'm' ) {
//       inputString.remove(0,1);
//       int requestedBrightness = inputString.toInt() ;
//       if( requestedBrightness > MAX_BRIGHTNESS) {
//         DEBUG_PRINT("Ignoring input; value too high. MAX_BRIGHTNESS: ");
//         DEBUG_PRINTLN( MAX_BRIGHTNESS ) ;
//         currentBrightness = MAX_BRIGHTNESS;
//       }
//       DEBUG_PRINT("currentBrightness: ");
//       DEBUG_PRINTLN( currentBrightness ) ;
//     }
//
//     // clear the string:
//     inputString = "";
//     stringComplete = false;
//   }
//
// }
// #endif

// Only use if no MPU present:
#if !defined(USING_MPU) && defined(BUTTON_PIN)
void cycleBrightness() {
  static uint8_t lerpBrightness ;

  if ( taskCheckButtonPress.getRunCounter() % 100 ) {
    currentBrightness = lerp8by8( 3, MAX_BRIGHTNESS, quadwave8( lerpBrightness )) ;
    lerpBrightness++ ;
    DEBUG_PRINTLN( currentBrightness ) ;
  }

//  taskLedModeSelect.disable() ;
//  fill_solid(leds, NUM_LEDS, CRGB::White );
//  FastLED.show() ;

}
#endif

// Custom modulo which always returns a positive number
int mod(int x, int m) {
  return (x % m + m) % m;
}


/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
// void serialEvent() {
//   while (Serial.available()) {
//     // get the new byte:
//     char inChar = (char)Serial.read();
//     Serial.print(inChar); // echo back what we got
//     // add it to the inputString:
//     inputString += inChar;
//     // if the incoming character is a newline, set a flag so the main loop can
//     // do something about it:
//     if (inChar == '\n') {
//       stringComplete = true;
//     }
//   }
// }

// #ifdef AUTOADVANCE
// void autoAdvanceLedMode() {
//   nextLedMode() ;
//   // if ( strcmp(routines[ledMode], "jugglePal") == 0 ) {
//   //   taskAutoAdvanceLedMode.delay( TASK_SECOND * 10 ) ;
//   // }
// }
// #endif
//
// void nextLedMode() {
//   fill_solid(leds, NUM_LEDS, CRGB::Black);
//   FastLED.show() ; // clear the canvas - prevent power fluctuations if a next pattern has lots of brightness
//   ledMode++;
//   if (ledMode == NUMROUTINES ) {
//     ledMode = 0;
//   }
//
//   DEBUG_PRINT(F("ledMode = ")) ;
//   DEBUG_PRINT( routines[ledMode] ) ;
//   DEBUG_PRINT(F(" mode ")) ;
//   DEBUG_PRINTLN( ledMode ) ;
// }

uint8_t QuadraticEaseIn8( uint8_t p ) {
  int     i_100       = map(p, 0, NUM_LEDS, 0, 100); // Map current led p to percentage between 0 - 100
  AHFloat eased_float = QuadraticEaseInOut( (float)i_100 / (float)100); // Convert to value between 0 - 1
  int     eased_100   = (int)(eased_float * 100); // convert back to percentage
  return  map(eased_100, 0, 100, 0, NUM_LEDS);  // convert back to LED position
}

uint8_t CubicEaseIn8( uint8_t p ) {
  int     i_100       = map(p, 0, NUM_LEDS, 0, 100); // Map current led p to percentage between 0 - 100
  AHFloat eased_float = CubicEaseInOut( (float)i_100 / (float)100); // Convert to value between 0 - 1
  int     eased_100   = (int)(eased_float * 100); // convert back to percentage
  return  map(eased_100, 0, 100, 0, NUM_LEDS);  // convert back to LED position
}

uint8_t mappedEase8InOutQuad( uint8_t p ) {
  int     i_255       = map(p, 0, NUM_LEDS, 0, 255); // Map current led p to percentage between 0 - 100
  int     eased_255   = ease8InOutQuad( i_255 );
  return  map(eased_255, 0, 255, 0, NUM_LEDS);  // convert back to LED position
}
