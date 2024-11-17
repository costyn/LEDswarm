#ifndef UI_CONTROLLER_H
#define UI_CONTROLLER_H

#include <JC_Button.h>
#include <ArduinoTapTempo.h>
#include "AnimationController.h"
#include "MeshController.h"

class UIController
{
public:
    UIController(AnimationController &animController, MeshController &meshController);
    void init();
    void checkButtonPress();
    ArduinoTapTempo &getTapTempo() { return tapTempo; }
    uint32_t getCurrentBPM() const { return currentBPM; }

private:
    Button bpmButton;
    Button nextPatternButton;
    ArduinoTapTempo tapTempo;
    bool newBPMSet;
    uint32_t currentBPM;

    AnimationController &_animController;
    MeshController &_meshController;
};

#endif
