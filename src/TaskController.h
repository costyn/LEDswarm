// TaskController.h
#ifndef TASK_CONTROLLER_H
#define TASK_CONTROLLER_H

#include <TaskScheduler.h>

// Forward declarations
class MeshController;
class AnimationController;
class UIController;

class TaskController
{
public:
    TaskController(MeshController &meshCtrl, AnimationController &animCtrl, UIController &uiCtrl);
    void init();
    void selectNextPattern();

    // Public because it needs to be accessible by the mesh library
    static Scheduler scheduler;

    // Make instance accessible for callbacks
    static TaskController *instance;

private:
    // Changed to non-static member functions
    void checkButtonPress();
    void currentPatternRun();
    void sendMessage();

    // Static callback wrappers
    static void checkButtonPressCallback();
    static void currentPatternRunCallback();
    static void sendMessageCallback();
    static void selectNextPatternCallback();

    // Task definitions
    Task taskCheckButtonPress;
    Task taskCurrentPatternRun;
    Task taskSendMessage;
    Task taskSelectNextPattern;

    // Controller references
    MeshController &_meshController;
    AnimationController &_animController;
    UIController &_uiController;
};

#endif // TASK_CONTROLLER_H