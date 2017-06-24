uint32_t getMasterNodeId() {
  uint32_t lowestId = UINT32_MAX ;
  SimpleList<uint32_t>::iterator node = nodes.begin();
  while( node != nodes.end() ) {
    if( *node < lowestId ) {
      lowestId = *node ;
    }
    node++;
  }
  return lowestId ;
}

void checkMastership() {
  if( mesh.getNodeId() == getMasterNodeId() ) {
    role = "MASTER";
    taskSendMessage.enableIfNot() ;
    taskCheckButtonPress.enableIfNot() ;
    taskSelectNextPattern.enableIfNot() ;
  } else {
    role = "SLAVE" ;
    taskSendMessage.disable() ;        // Only MASTER sends broadcast
    taskCheckButtonPress.disable() ;    // Slaves can't set BPM
    taskSelectNextPattern.disable() ;   // Slaves wait for instructions from the MASTER
  }
}


void receivedCallback( uint32_t from, String &msg ) {
  //  Serial.printf("Received msg from %u: %s\n", from, msg.c_str());
  if( role == "SLAVE" ) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root  = jsonBuffer.parseObject(msg);

    if( root["currentBPM"] ) {
      currentBPM = root["currentBPM"].as<uint32_t>() ;
      currentPattern   = root["currentPattern"].as<uint8_t>() ;
      Serial.printf("%s %u: \tBPM: %u\t Pattern: %u\n", role.c_str(), mesh.getNodeTime(), currentBPM, currentPattern );
    }

    if( root["runOnce"] ) {
      executeOneCycle = true ;
    }

  } else if( role == "MASTER") {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root  = jsonBuffer.parseObject(msg);

    uint32_t patternRunTime = root["patternRunTime"].as<uint32_t>() ;
    Serial.printf("%s %u (slave end time): \tBPM: %u\t Pattern: %u\tRunTime: %u\n", role.c_str(), mesh.getNodeTime(), currentBPM, currentPattern, patternRunTime );
    taskRunPatternOnNode.forceNextIteration(); // Send message to next SLAVE
  }

  Serial.println(); // whitespace for easier reading

}



void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("-- > startHere: New Connection, nodeId = %u\n", nodeId);
  nodes = mesh.getNodeList();
  nodes.push_back(mesh.getNodeId()); // add ourselves to the nodes list
  checkMastership() ;
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());

  nodes = mesh.getNodeList();
  nodes.push_back(mesh.getNodeId()); // add ourselves to the nodes list
  checkMastership() ;

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
