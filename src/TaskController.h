// TaskController.h
#ifndef TASK_CONTROLLER_H
#define TASK_CONTROLLER_H

#include <TaskScheduler.h>

extern Scheduler userScheduler;

// Forward declarations
class MeshController;
class AnimationController;
class UIController;

class TaskController
{
public:
    TaskController(MeshController &meshCtrl, AnimationController &animCtrl, UIController &uiCtrl);
    void init();

    static Scheduler scheduler;
    static TaskController *instance;

private:
    // Controller references first
    MeshController &_meshController;
    AnimationController &_animController;
    UIController &_uiController;

    // Then task members
    Task taskCheckButtonPress;
    Task taskCurrentPatternRun;
    Task taskSendMessage;
    Task taskSelectNextPattern;

    // Member functions
    void checkButtonPress();
    void currentPatternRun();
    void sendMessage();
    void selectNextPattern();

    // Static callbacks
    static void checkButtonPressCallback();
    static void currentPatternRunCallback();
    static void sendMessageCallback();
    static void selectNextPatternCallback();
};
#endif // TASK_CONTROLLER_H