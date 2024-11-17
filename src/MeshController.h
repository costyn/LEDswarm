#ifndef MESH_CONTROLLER_H
#define MESH_CONTROLLER_H

#include <painlessMesh.h>
#include <Arduino.h>
#include "AnimationController.h"

class MeshController
{
public:
    MeshController(AnimationController &animController);
    void init()
    {
        mesh.setDebugMsgTypes(ERROR | STARTUP | SYNC);
        mesh.init(MESH_PREFIX, MESH_PASSWORD, &TaskController::scheduler, MESH_PORT);
    }
    void update();
    uint32_t getNodeTime() { return mesh.getNodeTime(); }
    bool isAlone() { return mesh.getNodeList().size() <= 1; }
    void checkLeadership();
    uint8_t getNodePosition() const { return _nodePos; }
    uint16_t getMeshNumLeds() const { return _meshNumLeds; }
    bool isLeader() const { return role == LEADER; }

private:
    static void receivedCallback(uint32_t from, String &msg);
    static void newConnectionCallback(uint32_t nodeId);
    static void changedConnectionCallback();
    static void nodeTimeAdjustedCallback(int32_t offset);

    painlessMesh mesh;
    SimpleList<uint32_t> nodes;
    String role;
    uint8_t _nodePos;
    uint16_t _meshNumLeds;
    uint8_t _numNodes;
    AnimationController &_animController;

    void handlePatternMessage(const JsonDocument &doc);
    void broadcastCurrentState();
};

#endif