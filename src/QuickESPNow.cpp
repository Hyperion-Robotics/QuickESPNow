#include "QuickESPNow.h"


/**************Declaration of static variables and methods**************/
uint8_t* QuickESPNow::getEspMAC(){
    uint8_t *temp;
    getSTRINGtoMAC(WiFi.macAddress(), temp);
    return temp;
}

void QuickESPNow::getEspMAC(uint8_t* MAC){
    getSTRINGtoMAC(WiFi.macAddress(), MAC);
}



#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
void QuickESPNow::OnDataSent(const esp_now_send_info_t *tx_info, esp_now_send_status_t status){
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
#else
void QuickESPNow::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
#endif

#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
void QuickESPNow::OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    msg_struct receivedData;
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    QuickESPNow::recieved_msgs.add(&receivedData);
}
#elif ESP_ARDUINO_VERSION == ESP_ARDUINO_VERSION_VAL(2, 0, 17)
void QuickESPNow::OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    msg_struct receivedData;
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    QuickESPNow::recieved_msgs.add(&receivedData);
}
#endif
uint8_t QuickESPNow::Local_MAC[MAC_LENGTH];

Msg_Queue QuickESPNow::recieved_msgs;
/***********************************************************************/

/**************Constructors**************/
QuickESPNow::QuickESPNow(const COMMUNICATION communication, const int peers_crowd, const uint8_t* new_local_MAC){
    if(communication == SENDER || communication == RECEIVER || communication == TWO_WAY_COMMUNICATION){
        this->ESP_COM = communication;
    }else{
        this->setup_errors[this->error_counter] = ESP_NOW_COMMUNICATION_SETUP_ERROR; 
        this->error_counter++;
    }

    this->ids = (int*)malloc(peers_crowd*sizeof(int));
    this->Peers_MAC = (uint8_t**)malloc(peers_crowd*sizeof(uint8_t*));
    for(int i=0; i<MAC_LENGTH; i++){
        this->Peers_MAC[i] = (uint8_t*)malloc(MAC_LENGTH*sizeof(uint8_t));
    }

    this->Encryption = false;

    memcpy(QuickESPNow::Local_MAC, new_local_MAC, MAC_LENGTH);
}

QuickESPNow::QuickESPNow(const COMMUNICATION communication, const int peers_crowd, const uint8_t* new_local_MAC, const char* new_PMK_key){
    if(communication == SENDER || communication == RECEIVER || communication == TWO_WAY_COMMUNICATION){
        this->ESP_COM = communication;
    }else{
        this->setup_errors[this->error_counter] = ESP_NOW_COMMUNICATION_SETUP_ERROR;
        this->error_counter++;
    }

    this->ids = (int*)malloc(peers_crowd*sizeof(int));
    this->Peers_MAC = (uint8_t**)malloc(peers_crowd*sizeof(uint8_t*));
    for(int i=0; i<MAC_LENGTH; i++){
        this->Peers_MAC[i] = (uint8_t*)malloc(MAC_LENGTH*sizeof(uint8_t));
    }

    this->Encryption = true;

    memcpy(QuickESPNow::Local_MAC, new_local_MAC, MAC_LENGTH);

    if(this->PMK_key == nullptr){
        this->PMK_key = (char*)malloc((ENCRYPTION_KEY_LENGTH + 1) * sizeof(char));
    }

    if (this->PMK_key != nullptr) {
        // Copy the String content to PMK_key
        strcpy(this->PMK_key, new_PMK_key);
    } else {
        // Handle memory allocation failure
        this->setup_errors[this->error_counter] = MEMORY_ALLOCATION_ERROR;
        this->error_counter++;
    }
}
/***************************************/

/**************Starting of the espnow communication**************/
void QuickESPNow::addPeer(int id, uint8_t* Peers_MAC, int Ch, wifi_interface_t mode){

    if(Ch < 0 || 13 < Ch){
        this->setup_errors[this->error_counter] = CHANNEL_OUT_OF_RANGRE; 
        this->error_counter++;
    }

    this->ids[this->id_counter] = id;
    memcpy(this->Peers_MAC[this->id_counter], Peers_MAC, MAC_LENGTH);
    this->id_counter++;

    // Initialize the peerInfo structure
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, Peers_MAC, MAC_LENGTH);
    peerInfo.channel = Ch;  
    peerInfo.encrypt = false;
    peerInfo.ifidx = mode;  // Use station interface (most common for ESP-NOW)

    // Add receiver as peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("[Error] Failed to add peer");
        return;
    }else{
        Serial.println("[SUCCESS] peer has been added succesfuly");
    }
}





void QuickESPNow::addPeer(int id, uint8_t* Peers_MAC, int Ch, wifi_interface_t mode, char* LMK_keys_array){
    if(Ch < 0 || 13 < Ch){
        this->setup_errors[this->error_counter] = CHANNEL_OUT_OF_RANGRE; 
        this->error_counter++;
    }

    this->ids[this->id_counter] = id;
    memcpy(this->Peers_MAC[this->id_counter], Peers_MAC, MAC_LENGTH);
    this->id_counter++;

    // Initialize the peerInfo structure
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, Peers_MAC, MAC_LENGTH);
    peerInfo.channel = Ch;  
    peerInfo.encrypt = false;
    peerInfo.ifidx = mode;  // Use station interface (most common for ESP-NOW)

    //Set the receiver device LMK key
    for (uint8_t i = 0; i < 16; i++) {
        peerInfo.lmk[i] = LMK_keys_array[i];
    }


    // Add receiver as peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }else{
        Serial.println("[SUCCESS] peer has been added succesfuly");
    }
}


void QuickESPNow::addPeer(int id, esp_now_peer_info_t* Peer){
    this->ids[this->id_counter] = id;
    memcpy(this->Peers_MAC[this->id_counter], Peer->peer_addr, MAC_LENGTH);
    this->id_counter++;

    // Add receiver as peer        
    if (esp_now_add_peer(Peer) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }else{
        Serial.println("[SUCCESS] peer has been added succesfuly");
    }
}

void QuickESPNow::begin(){
    // Set the WiFi module to station mode
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        this->setup_errors[this->error_counter] = ESP_NOW_INITIALIZATION_ERROR; 
        this->error_counter++;
    }

    // Setup communication based on mode
    switch(this->ESP_COM) {
        case SENDER:
            // Register for Send CB to get the status of Transmitted packet
            esp_now_register_send_cb(QuickESPNow::OnDataSent);
            break;
        case RECEIVER:
            esp_now_register_recv_cb(QuickESPNow::OnDataRecv);
            break;
        case TWO_WAY_COMMUNICATION:
            esp_now_register_send_cb(QuickESPNow::OnDataSent);
            esp_now_register_recv_cb(QuickESPNow::OnDataRecv);
            break;
    }


    // Read the old MAC
    Serial.print("[OLD] ESP32 Board MAC Address:  ");
    Serial.println(WiFi.macAddress());

    delay(100);

    // Set new MAC
    esp_wifi_set_mac(WIFI_IF_STA, this->Local_MAC);

    delay(100);
    
    Serial.print("\r[NEW] ESP32 Board MAC Address:  ");
    Serial.println(WiFi.macAddress());

    // Verify that the new MAC is set correctly

    if (!WiFi.macAddress().equals(getMACtoSTRING(this->Local_MAC))) {
        Serial.println("\r[Error] Failed to change MAC");
        this->setup_errors[this->error_counter] = NEW_MAC_INITIALIZATION_ERROR;
        this->error_counter++;
    }

    if(this->Encryption){
        esp_now_set_pmk((uint8_t *)this->PMK_key);
    }
    delay(100);
}
/**********************************************************/

void QuickESPNow::setChannel(int ch){
    #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    WiFi.setChannel(ch);
    delay(100);
    #elif ESP_ARDUINO_VERSION == ESP_ARDUINO_VERSION_VAL(2, 0, 17)
    WiFi.channel(ch);
    delay(100);
    #endif
}
/**************Checking for istalisation errors**************/
bool QuickESPNow::FAIL_CHECK() {
    if(this->error_counter == 0) {
        Serial.println("\r[SUCCESS] THERE WERE NO INITIALIZATION ERROR");
        return false;
    }
    for(int i = 0; i < this->error_counter; i++) {
        if(this->setup_errors[i]==ESP_NOW_INITIALIZATION_ERROR) Serial.println("\r[Error] initializing ESP-NOW");
        else if(this->setup_errors[i]==ESP_NOW_COMMUNICATION_SETUP_ERROR ) Serial.println("\r[Error] in the consructors communication parameter");
        else if(this->setup_errors[i]==CHANNEL_OUT_OF_RANGRE) Serial.println("\r[Error] in the value of the channel (channel ranges 0-13)");        
        else if(this->setup_errors[i]==NEW_MAC_INITIALIZATION_ERROR) Serial.println("\r[Error] setting new MAC address");
        else if(this->setup_errors[i]==ADD_PEER_INITIALIZATION_ERROR) Serial.println("\r[Error] adding peer");
        else if(this->setup_errors[i]==MEMORY_ALLOCATION_ERROR) Serial.println("\r[Error] allocating memory");
    }
    Serial.println();
    return true;
}
/***********************************************************/

/**************Checking if the esp has recieved any msg**************/
bool QuickESPNow::available() const{
    return !this->recieved_msgs.isEmpty();
}

bool QuickESPNow::isArray() const{
    return QuickESPNow::recieved_msgs.isFrontArray();
}

MSG_VARIABLE_TYPE QuickESPNow::data_type() const{
    return QuickESPNow::recieved_msgs.data_type();
}
/********************************************************************/

void QuickESPNow::setWiFi_to_STA(){
    WiFi.mode(WIFI_MODE_STA);
    delay(10);
}

// Set WiFi to AP mode
void QuickESPNow::setWiFi_to_AP() {
    WiFi.mode(WIFI_MODE_AP);
    delay(10);  
}

// Set WiFi to AP+STA mode
void QuickESPNow::setWiFi_to_APSTA() {
    WiFi.mode(WIFI_MODE_APSTA);
    delay(10);
}

QuickESPNow::~QuickESPNow(){
    QuickESPNow::recieved_msgs.~Msg_Queue();
    for(int i=0; i<this->id_counter; i++){
        free(this->Peers_MAC[i]);
    }
    free(this->Peers_MAC);
    free(this->PMK_key);
    free(ids);

    for(int i=0; i<this->id_counter; i++){
        esp_now_del_peer(this->Peers_MAC[i]);
        delay(10);
    }
    esp_now_deinit();
    
    QuickESPNow::recieved_msgs.~Msg_Queue();
    QuickESPNow::recieved_msgs = Msg_Queue();
}


void QuickESPNow::setCustomSendCallback(esp_now_send_cb_t custom){
    esp_now_unregister_send_cb();
    esp_now_register_send_cb(custom);
}

void QuickESPNow::setCustomRecvCallback(esp_now_recv_cb_t custom){
    esp_now_unregister_recv_cb();
    esp_now_register_recv_cb(custom);
}

