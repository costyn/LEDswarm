void checkMastership() {
  uint32_t masterNodeId = UINT32_MAX ;
  nodes = mesh.getNodeList();
  nodes.push_back(mesh.getNodeId()); // add ourselves to the nodes list

  SimpleList<uint32_t>::iterator node = nodes.begin();

  // Print out a list of nodes and check which has the lowest Node ID:
  Serial.printf("Node list: ");


  while( node != nodes.end() ) {
    if( *node < masterNodeId ) {
      masterNodeId = *node ;
    }
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.printf(" (%d nodes)\n", nodes.size());

  if( mesh.getNodeId() == masterNodeId ) {
    role = "MASTER";
    taskSendMessage.enableIfNot() ;
    taskCheckButtonPress.enableIfNot() ;
    taskSelectNextPattern.enableIfNot() ;
    Serial.printf("%s %u: checkMastership(); I am MASTER\n", role.c_str(), mesh.getNodeTime() );
  } else {
    role = "SLAVE" ;
    taskSendMessage.disable() ;         // Only MASTER sends broadcast
    taskCheckButtonPress.disable() ;    // Slaves can't set BPM
    taskSelectNextPattern.disable() ;   // Slaves wait for instructions from the MASTER
    Serial.printf("%s %u: checkMastership(); I am SLAVE\n", role.c_str(), mesh.getNodeTime() );
  }
} // end checkMastership()


void receivedCallback( uint32_t from, String &msg ) {
  //  Serial.printf("Received msg from %u: %s\n", from, msg.c_str());
  if( role == "SLAVE" ) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root  = jsonBuffer.parseObject(msg);

    if( root["currentBPM"] ) {
      currentBPM = root["currentBPM"].as<uint32_t>() ;  // TODO: set BPM in tapTempo object
      currentPattern   = root["currentPattern"].as<uint8_t>() ;
      Serial.printf("%s %u: \tBPM: %u\t Pattern: %u\n", role.c_str(), mesh.getNodeTime(), currentBPM, currentPattern );
    }

    // if( root["runOnce"] ) {
    //   executeOneCycle = true ;
    // }

  } else if( role == "MASTER") {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root  = jsonBuffer.parseObject(msg);

    uint32_t patternRunTime = root["patternRunTime"].as<uint32_t>() ;
    Serial.printf("%s %u (slave end time): \tBPM: %u\t Pattern: %u\tRunTime: %u\n", role.c_str(), mesh.getNodeTime(), currentBPM, currentPattern, patternRunTime );
    //      taskRunPatternOnNode.forceNextIteration(); // Send message to next SLAVE
  }
  Serial.println(); // whitespace for easier reading
} // end receivedCallback()



void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("%s %u: New Connection from nodeId = %u\n", role.c_str(), mesh.getNodeTime(), nodeId);
  checkMastership() ;
}

void changedConnectionCallback() {
  Serial.printf("%s %u: Changed connections %s\n", role.c_str(), mesh.getNodeTime(), mesh.subConnectionJson().c_str());
  checkMastership() ;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("%s: Adjusted time %u. Offset = %d\n", role.c_str(), mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}
