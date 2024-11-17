#ifndef UI_CONTROLLER_H
#define UI_CONTROLLER_H

#include <JC_Button.h>
#include <ArduinoTapTempo.h>
#include "AnimationController.h"

class UIController
{
public:
    UIController(AnimationController &animController);
    void init();
    void update();

private:
    Button bpmButton;
    Button nextPatternButton;
    ArduinoTapTempo tapTempo;
    AnimationController &_animController;

    void checkButtonPress();
};

#endif
