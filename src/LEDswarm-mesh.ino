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

    currentBPM = root["currentBPM"].as<uint32_t>() ;
    currentPattern   = root["currentPattern"].as<uint8_t>() ;
    Serial.printf("SLAVE %u: \tBPM: %u\t Pattern: %u\n", mesh.getNodeTime(), currentBPM, currentPattern );

  } else {
    Serial.printf("received msg from %u but I'm the master\n", from) ;
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
