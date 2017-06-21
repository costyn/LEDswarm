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
    taskSendBeatSync.enableIfNot() ;
    taskCheckButtonPress.enableIfNot() ;
    taskSelectNextPattern.enableIfNot() ;
  } else {
    role = "SLAVE" ;
    taskSendBeatSync.disable() ;        // Only MASTER sends broadcast
    taskCheckButtonPress.disable() ;    // Slaves can't set BPM
    taskSelectNextPattern.disable() ;   // Slaves wait for instructions from the MASTER
  }
}


void receivedCallback( uint32_t from, String &msg ) {
  //  Serial.printf("Received msg from %u: %s\n", from, msg.c_str());
  if( role == "SLAVE" ) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root  = jsonBuffer.parseObject(msg);

//    if( root["currentBeatLength"] ) {  // Check if it's a sync msg
    currentBeatLength = root["currentBeatLength"].as<uint32_t>() ;
//      Serial.printf("SLAVE %u: from: %u \tBL: %u\n", mesh.getNodeTime(), from, currentBeatLength  );
    nextPattern   = root["nextPattern"].as<uint8_t>() ;

    // TODO: make this check a little less strict; if it's within x microseconds, it's in SYNC
    if( tapTimer != root["taptimer"].as<uint32_t>() ) {
      tapTimer    = root["taptimer"].as<uint32_t>() ;
      Serial.printf("SLAVE %u: Unsynced with MASTER. Next tapTime (from master): %u.\tPattern: %u\n", mesh.getNodeTime(), tapTimer, nextPattern ) ;
    } else {
      Serial.printf("SLAVE %u: In SYNC with MASTER. Next tapTime: %u\tPattern: %u\n", mesh.getNodeTime(), tapTimer, nextPattern );
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
