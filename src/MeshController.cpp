#include "MeshController.h"

extern uint8_t currentPattern;
extern uint8_t nextPattern;
extern uint32_t currentBPM;
extern ArduinoTapTempo tapTempo;
extern Task taskSendMessage;
extern Task taskCheckButtonPress;
extern Task taskSelectNextPattern;
// Global scheduler
extern Scheduler userScheduler;
extern painlessMesh mesh;

MeshController *MeshController::instance = nullptr;

MeshController::MeshController(AnimationController &animController)
    : _animController(animController),
      _nodePos(0),
      _meshNumLeds(LEDS_PER_NODE),
      _numNodes(1),
      _role(LEADER)
{
    instance = this;
}

void MeshController::init()
{
    Serial.println("after mesh init");
}

void MeshController::update()
{
    mesh.update();
}

uint32_t getNodeId() { return mesh.getNodeId(); }
uint32_t getNodeTime() { return mesh.getNodeTime(); }
bool alone() { return mesh.getNodeList().size() <= 1; }

void MeshController::checkLeadership()
{
    uint32_t leaderNodeId = UINT32_MAX;
    nodes = mesh.getNodeList(true);
    _numNodes = nodes.size();

    // Determine leadership and node position
    _nodePos = 0;
    uint32_t currentNodeId = mesh.getNodeId();
    uint32_t timeNow = mesh.getNodeTime();

    Serial.printf("Node list: ");
    int k = 0;
    for (uint32_t nodeId : nodes)
    {
        Serial.printf("%u ", nodeId);
        if (nodeId == currentNodeId)
        {
            _nodePos = k;
        }
        if (nodeId < leaderNodeId)
        {
            leaderNodeId = nodeId;
        }
        k++;
    }
    Serial.printf("\n");

    // Update LED count
    _meshNumLeds = _numNodes * LEDS_PER_NODE;
    _animController.setMeshNumLeds(_meshNumLeds);
    _animController.setAlone(alone());

    // Assign roles based on leadership
    bool wasLeader = (_role == LEADER);
    _role = (currentNodeId == leaderNodeId) ? LEADER : FOLLOWER;

    if (_role == LEADER)
    {
        taskSendMessage.enableIfNot();
        taskCheckButtonPress.enableIfNot();
        taskSelectNextPattern.enableIfNot();
    }
    else
    {
        taskSendMessage.disable();
        taskCheckButtonPress.disable();
        taskSelectNextPattern.disable();
    }

    // If we just became leader or follower, we might need to sync state
    if (wasLeader != (_role == LEADER))
    {
        if (_role == LEADER)
        {
            broadcastPatternChange();
        }
    }

    Serial.printf("%s %u: checkLeadership(); I am %s\n", _role.c_str(), timeNow, _role.c_str());
    Serial.printf("%s %u: %d nodes. Mesh LEDs: %d. NodePos: %d Root: %d\n",
                  _role.c_str(), timeNow, _numNodes, _meshNumLeds, _nodePos, mesh.isRoot());
}

void MeshController::updateNodePosition(uint32_t currentNodeId)
{
    _nodePos = 0;
    int k = 0;
    Serial.printf("Node list: ");
    for (uint32_t nodeId : nodes)
    {
        Serial.printf("%u ", nodeId);
        if (nodeId == currentNodeId)
        {
            _nodePos = k;
        }
        k++;
    }
    Serial.printf("\n");
}

void MeshController::broadcastPatternChange()
{
    if (_role == LEADER)
    {
        taskSendMessage.forceNextIteration();
    }
}

void MeshController::sendMessage()
{
    if (!tapTempo.isChainActive() || (currentPattern != nextPattern))
    {
        JsonDocument outgoingJsonMessage;

        currentPattern = nextPattern;
        currentBPM = tapTempo.getBPM();

        outgoingJsonMessage["currentBPM"] = currentBPM;
        outgoingJsonMessage["currentPattern"] = currentPattern;

        String outgoingJsonString;
        serializeJson(outgoingJsonMessage, outgoingJsonString);
        mesh.sendBroadcast(outgoingJsonString);

        Serial.printf("%s (%u) %u: Sent broadcast message: ", _role.c_str(), _nodePos, mesh.getNodeTime());
        Serial.println(outgoingJsonString);
    }
    else
    {
        Serial.printf("%s %u: No msg to send.\tBPM: %u\tPattern: %u\n",
                      _role.c_str(), mesh.getNodeTime(), currentBPM, currentPattern);
    }
}

void MeshController::receivedCallback(uint32_t from, String &msg)
{
    if (!instance)
        return;

    if (instance->_role == FOLLOWER)
    {
        JsonDocument incomingJsonString;
        deserializeJson(incomingJsonString, msg);

        if (incomingJsonString["currentBPM"])
        {
            currentBPM = incomingJsonString["currentBPM"].as<uint32_t>();
            nextPattern = incomingJsonString["currentPattern"].as<uint8_t>();

            Serial.printf("%s (%u) %u: \tBPM: %u\t Pattern: %u\n",
                          instance->_role.c_str(), instance->_nodePos,
                          mesh.getNodeTime(), currentBPM, currentPattern);

            tapTempo.setBPM(currentBPM);
        }
    }
}

void MeshController::newConnectionCallback(uint32_t nodeId)
{
    if (!instance)
        return;
    Serial.printf("%s %u: New Connection from nodeId = %u\n",
                  instance->_role.c_str(), mesh.getNodeTime(), nodeId);
    instance->checkLeadership();
}

void MeshController::changedConnectionCallback()
{
    if (!instance)
        return;
    Serial.printf("%s %u: Changed connections %s\n",
                  instance->_role.c_str(), mesh.getNodeTime(),
                  mesh.subConnectionJson().c_str());
    instance->checkLeadership();
}

void MeshController::nodeTimeAdjustedCallback(int32_t offset)
{
    if (!instance)
        return;
    Serial.printf("%s: Adjusted time %u. Offset = %d\n",
                  instance->_role.c_str(), mesh.getNodeTime(), offset);
}