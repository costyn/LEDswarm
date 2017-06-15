uint32_t getMasterNodeId() {
  uint32_t lowestId = UINT32_MAX ;
  SimpleList<uint32_t>::iterator node = nodes.begin();
  while ( node != nodes.end() ) {
    if (  *node < lowestId ) {
      lowestId = *node ;
    }
    node++;
  }
  return lowestId ;
}

// TODO: set a global variable with MASTER instead of checking every time(?).
// Only check at changedConnectionCallback or newConnectionCallback
bool thisNodeMaster() {
  return mesh.getNodeId() == getMasterNodeId() ;
}

// TODO: differentiate between various received messages
void receivedCallback( uint32_t from, String &msg ) {
  //  Serial.printf("Received msg from %u: %s\n", from, msg.c_str());
  if ( ! thisNodeMaster() ) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root  = jsonBuffer.parseObject(msg);
    currentBeatLength = root["currentBeatLength"].as<uint32_t>() ;
    Serial.printf("SLAVE %u: from: %u \tBL: %u\n", mesh.getNodeTime(), from, currentBeatLength  );

    if( tapTimer != root["taptimer"].as<uint32_t>() ) {
      tapTimer = root["taptimer"].as<uint32_t>() ;
      Serial.printf("SLAVE %u: Unsynced with MASTER. Next tapTime (from master): %u\n", mesh.getNodeTime(), tapTimer ) ;
    } else {
      Serial.printf("SLAVE %u: In SYNC with MASTER. Next tapTime: %u\n", mesh.getNodeTime(), tapTimer );
    }

  } else {
    Serial.printf("received msg from %u but I'm the master\n", from) ;
  }

  Serial.println(); // whitespace for easier reading

}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("-- > startHere: New Connection, nodeId = %u\n", nodeId);
  nodes = mesh.getNodeList();
  nodes.push_back(mesh.getNodeId()); // add ourselves to the nodes list
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());

  nodes = mesh.getNodeList();
  nodes.push_back(mesh.getNodeId()); // add ourselves to the nodes list

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list: ");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}
