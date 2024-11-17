// TODO: convert Serial.printfs and println's to DEBUG statements we can turn on/off

#include <FX.h>
#include "LEDswarm.h"
#include "AnimationController.h"

#define TASK_CHECK_BUTTON_PRESS_INTERVAL 10
#define CURRENTPATTERN_SELECT_DEFAULT_INTERVAL 5 // in milliseconds

void checkButtonPressCallback();
void currentPatternRunCallback();
void sendMessageCallback();
void selectNextPatternCallback();

Task taskCheckButtonPress(TASK_CHECK_BUTTON_PRESS_INTERVAL, TASK_FOREVER, &checkButtonPressCallback);
Task taskCurrentPatternRun(CURRENTPATTERN_SELECT_DEFAULT_INTERVAL, TASK_FOREVER, &currentPatternRunCallback);
Task taskSendMessage(TASK_SECOND * 5, TASK_FOREVER, &sendMessageCallback);
Task taskSelectNextPattern(TASK_SECOND *AUTO_ADVANCE_DELAY, TASK_FOREVER, &selectNextPatternCallback);

AnimationController animationController;            // Create global instance
MeshController meshController(animationController); // Create global instance
UIController uiController(animationController, meshController);
TaskController taskController(meshController, animationController, uiController);

// ************************************************************************
//                              SETUP
// ************************************************************************

void setup()
{
  Serial.begin(115200);
  delay(1000); // Startup delay; let things settle down
  Serial.println("after Serial begin");

  animationController.init();

  meshController.init();

  uiController.init();

  taskController.init();
} // end setup()

// ************************************************************************
//                              LOOP
// ************************************************************************

void loop()
{
  meshController.update();
}

// ************************************************************************
//                       LED PATTERN FUNCTIONS
// ************************************************************************

// This is where the magic happens!
// @Override This function is called by FastLED inside lib8tion.h. Requests it to use mesg.getNodeTime instead of internal millis() timer.
// Makes every pattern on each node synced!! So AWESOME!
uint32_t get_millisecond_timer()
{
  return meshController.getNodeTime() / 1000;
}

// Task callbacks
void currentPatternRun()
{
  animationController.currentPatternRun();
}

void selectNextPattern()
{
  animationController.selectNextPattern();
}

void sendMessage()
{
  meshController.sendMessage();
}
