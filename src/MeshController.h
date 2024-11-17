#ifndef MESH_CONTROLLER_H
#define MESH_CONTROLLER_H

#include <painlessMesh.h>
#include <Arduino.h>
#include "AnimationController.h"
#include "ArduinoTapTempo.h"

#define LEADER "LEADER"
#define FOLLOWER "FOLLOWER"

class MeshController
{

#define LEADER "LEADER"
#define FOLLOWER "FOLLOWER"

public:
    MeshController(AnimationController &animController);
    static MeshController *instance; // For callback access

    // These need to be static C-style callbacks

    void init(); // Take scheduler as parameter to init
    void update();
    void checkLeadership();
    void sendMessage();
    void broadcastPatternChange();
    uint32_t getNodeId();
    uint32_t getNodeTime();
    bool isLeader() const { return _role == LEADER; }

    void receivedCallback(uint32_t from, String &msg);
    void newConnectionCallback(uint32_t nodeId);
    void changedConnectionCallback();
    void nodeTimeAdjustedCallback(int32_t offset);

private:
    AnimationController &_animController;
    uint8_t _nodePos;
    uint16_t _meshNumLeds;
    uint8_t _numNodes;
    String _role;
    SimpleList<uint32_t> nodes;

    void updateNodePosition(uint32_t currentNodeId);
    bool alone();
};

#endif