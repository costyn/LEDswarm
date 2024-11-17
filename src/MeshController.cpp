#include "MeshController.h"

MeshController::MeshController(AnimationController &animController)
    : _animController(animController), _nodePos(0), _meshNumLeds(LEDS_PER_NODE), _numNodes(1), role(LEADER)
{
}

void MeshController::init()
{
    mesh.setDebugMsgTypes(ERROR | STARTUP | SYNC);
    mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

    checkLeadership();
}

void MeshController::update()
{
    mesh.update();
}

void MeshController::checkLeadership()
{
    uint32_t leaderNodeId = UINT32_MAX;
    nodes = mesh.getNodeList(true);
    _numNodes = mesh.getNodeList().size();

    nodes.sort();

    // SimpleList<uint32_t>::iterator nodeIterator = nodes.begin(); // It's a std::list

    // Print out a list of nodes and check which has the lowest Node ID:
    Serial.printf("Node list: ");

    // https://www.techiedelight.com/convert-list-to-array-cpp/
    int k = 0;
    for (int const &i : nodes)
    {
        if (i == mesh.getNodeId())
        {
            _nodePos = k;
        }
        if (i < leaderNodeId)
        {
            leaderNodeId = i;
        }
        Serial.printf("%u ", i);
        k++;
    }
    Serial.printf("\n");

    _meshNumLeds = _numNodes * LEDS_PER_NODE;
    fx.setMeshNumLeds(_meshNumLeds);
    fx.setAlone(alone()); // How lonely are we?

    if (mesh.getNodeId() == leaderNodeId)
    {
        role = LEADER;
        taskSendMessage.enableIfNot();
        taskCheckButtonPress.enableIfNot();
        taskSelectNextPattern.enableIfNot();
    }
    else
    {
        role = FOLLOWER;
        taskSendMessage.disable();       // Only LEADER sends broadcast
        taskCheckButtonPress.disable();  // Followers can't set BPM
        taskSelectNextPattern.disable(); // Followers wait for instructions from the LEADER
    }
    Serial.printf("%s %u: checkLeadership(); I am %s\n", role.c_str(), mesh.getNodeTime(), role.c_str());

    Serial.printf("\n%s %u: %d nodes. Mesh Leds: %d. NodePos: %u root: %d\n", role.c_str(), mesh.getNodeTime(), _numNodes, _meshNumLeds, _nodePos, mesh.isRoot());
}