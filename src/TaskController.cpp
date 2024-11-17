#include "TaskController.h"
#include "MeshController.h"
#include "AnimationController.h"
#include "UiController.h"

Scheduler TaskController::scheduler;

#define TASK_CHECK_BUTTON_PRESS_INTERVAL 10
#define CURRENTPATTERN_SELECT_DEFAULT_INTERVAL 5

TaskController::TaskController(MeshController &meshCtrl, AnimationController &animCtrl, UIController &uiCtrl)
    : _meshController(meshCtrl),
      _animController(animCtrl),
      _uiController(uiCtrl),
      taskCheckButtonPress(TASK_CHECK_BUTTON_PRESS_INTERVAL, TASK_FOREVER, &checkButtonPressCallback),
      taskCurrentPatternRun(CURRENTPATTERN_SELECT_DEFAULT_INTERVAL, TASK_FOREVER, &currentPatternRunCallback),
      taskSendMessage(TASK_SECOND * 5, TASK_FOREVER, &sendMessageCallback),
      taskSelectNextPattern(TASK_SECOND * AUTO_ADVANCE_DELAY, TASK_FOREVER, &selectNextPatternCallback)
{
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

void TaskController::checkButtonPressCallback()
{
    _uiController.checkButtonPress();
}

void TaskController::currentPatternRunCallback()
{
    _animController.updatePattern();
}

void TaskController::sendMessageCallback()
{
    _meshController.broadcastCurrentState();
}

void TaskController::selectNextPatternCallback()
{
    _animController.nextPattern();
}