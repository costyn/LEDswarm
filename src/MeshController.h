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
    void init();
    void update();
    void checkLeadership();
    void sendMessage();
    void broadcastPatternChange();
    uint32_t getNodeId() { return mesh.getNodeId(); }
    uint32_t getNodeTime() { return mesh.getNodeTime(); }
    bool isLeader() const { return _role == LEADER; }

    static MeshController *instance; // For callback access

private:
    painlessMesh mesh;
    SimpleList<uint32_t> nodes;
    String _role;
    uint8_t _nodePos;
    uint16_t _meshNumLeds;
    uint8_t _numNodes;
    AnimationController &_animController;

    static void receivedCallback(uint32_t from, String &msg);
    static void newConnectionCallback(uint32_t nodeId);
    static void changedConnectionCallback();
    static void nodeTimeAdjustedCallback(int32_t offset);
    void updateNodePosition(uint32_t currentNodeId);
    bool alone() { return mesh.getNodeList().size() <= 1; }
};

#endif