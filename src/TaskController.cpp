#include "TaskController.h"
#include "MeshController.h"
#include "AnimationController.h"
#include "UiController.h"

#define TASK_CHECK_BUTTON_PRESS_INTERVAL 10
#define CURRENTPATTERN_SELECT_DEFAULT_INTERVAL 5 // default scheduling time for currentPatternSELECT, in milliseconds

Scheduler TaskController::scheduler;

TaskController *TaskController::instance = nullptr;

TaskController::TaskController(MeshController &meshCtrl, AnimationController &animCtrl, UIController &uiCtrl)
    : _meshController(meshCtrl), _animController(animCtrl), _uiController(uiCtrl), taskCheckButtonPress(TASK_CHECK_BUTTON_PRESS_INTERVAL, TASK_FOREVER, &checkButtonPressCallback), taskCurrentPatternRun(CURRENTPATTERN_SELECT_DEFAULT_INTERVAL, TASK_FOREVER, &currentPatternRunCallback), taskSendMessage(TASK_SECOND * 5, TASK_FOREVER, &sendMessageCallback), taskSelectNextPattern(TASK_SECOND * AUTO_ADVANCE_DELAY, TASK_FOREVER, &selectNextPatternCallback)
{
    instance = this;
}

void TaskController::init()
{
    scheduler.addTask(taskCheckButtonPress);
    scheduler.addTask(taskCurrentPatternRun);
    scheduler.addTask(taskSendMessage);

#ifdef AUTOADVANCE
    scheduler.addTask(taskSelectNextPattern);
    taskSelectNextPattern.enable();
#endif

    taskCheckButtonPress.enable();
    taskCurrentPatternRun.enable();
}

// Non-static member functions that do the actual work
void TaskController::checkButtonPress()
{
    _uiController.checkButtonPress();
}

void TaskController::currentPatternRun()
{
    _animController.currentPatternRun();
}

void TaskController::sendMessage()
{
    _meshController.sendMessage();
}
// Changing to the next pattern can come from 2 sources,
// Either the user presses a button, or the scheduler
// timer reaches a ~30 second interval
void TaskController::selectNextPattern()
{
    _animController.selectNextPattern();
    _meshController.broadcastPatternChange();
}

// Static callback wrappers that delegate to the instance methods
void TaskController::checkButtonPressCallback()
{
    if (instance)
        instance->checkButtonPress();
}

void TaskController::currentPatternRunCallback()
{
    if (instance)
        instance->currentPatternRun();
}

void TaskController::sendMessageCallback()
{
    if (instance)
        instance->sendMessage();
}

void TaskController::selectNextPatternCallback()
{
    if (instance)
        instance->selectNextPattern();
}