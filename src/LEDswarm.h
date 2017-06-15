// prototype methods:
void receivedCallback( uint32_t from, String &msg );
void newConnectionCallback(uint32_t nodeId) ;
void changedConnectionCallback() ;
void nodeTimeAdjustedCallback(int32_t offset) ;
void delayReceivedCallback(uint32_t from, int32_t delay) ;
void setTapTempo() ;
uint32_t getMasterNodeId() ;
bool thisNodeMaster() ;
void checkButtonPress() ;
