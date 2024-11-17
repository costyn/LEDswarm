#include "UiController.h"

UIController::UIController(AnimationController &animController, MeshController &meshController)
    : bpmButton(BPM_BUTTON_PIN),
      nextPatternButton(BUTTON_PIN),
      _animController(animController),
      _meshController(meshController),
      newBPMSet(false),
      currentBPM(DEFAULT_BPM)
{
}

void UIController::init()
{
    bpmButton.begin();
    nextPatternButton.begin();
    tapTempo.setBPM(DEFAULT_BPM);
}

void UIController::checkButtonPress()
{
    // bpmButton.read();  // Commented out as in original
    nextPatternButton.read();

    // if (bpmButton.wasPressed()) {
    //     tapTempo.update(true);
    //     Serial.printf("%s %u: Button TAP. BPM: ", _meshController.isLeader() ? LEADER : FOLLOWER,
    //                  _meshController.getNodeTime());
    //     Serial.println(tapTempo.getBPM());
    //     newBPMSet = true;
    // } else {
    //     tapTempo.update(false);
    // }

    if (nextPatternButton.wasPressed())
    {
        _animController.selectNextPattern();
        _meshController.broadcastPatternChange();
    }

    // if (nextPatternButton.pressedFor(500)) {
    //     cycleBrightness();
    // }
}