#include "UiController.h"

UIController::UIController(AnimationController &animController)
    : bpmButton(BPM_BUTTON_PIN), nextPatternButton(BUTTON_PIN), _animController(animController)
{
}

void UIController::init()
{
    bpmButton.begin();
    nextPatternButton.begin();
    tapTempo.setBPM(DEFAULT_BPM);
}

void UIController::update()
{
    nextPatternButton.read();

    // if( bpmButton.wasPressed() ) {
    //   tapTempo.update(true); // update ArduinoTapTempo
    //   Serial.printf("%s %u: Button TAP. BPM: ", role.c_str(), mesh.getNodeTime() );
    //   Serial.println(tapTempo.getBPM() );
    //   newBPMSet = true ;
    // } else {
    //   tapTempo.update(false);
    // }

    if (nextPatternButton.wasPressed())
    {
        nextPattern = currentPattern + 1;
        // Serial.printf("%s (%u) %u: after upping nextPattern: %i, NUMROUTINES = %i\n", role.c_str(), _nodePos, mesh.getNodeTime(), nextPattern, NUMROUTINES);

        if (nextPattern >= NUMROUTINES)
        {
            nextPattern = 0;
        }

        if (role == LEADER)
        {
            taskSendMessage.forceNextIteration(); // Schedule next iteration immediately, for sending a new pattern msg to Follower
        }
    }