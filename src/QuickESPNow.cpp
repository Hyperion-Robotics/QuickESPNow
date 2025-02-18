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

void QuickESPNow::getNumberOfALLPeers(esp_now_peer_num_t *peer_num){
    if(esp_now_get_peer_num(peer_num) != ESP_OK){
        setup_errors[GET_NUMBER_OF_PEERS_ERROR] = GET_NUMBER_OF_PEERS_ERROR;
    }
}

int QuickESPNow::getNumberOfNonEncryptedPeers(){
    esp_now_peer_num_t peer_num;
    if(esp_now_get_peer_num(&peer_num) != ESP_OK){
        setup_errors[GET_NUMBER_OF_PEERS_ERROR] = GET_NUMBER_OF_PEERS_ERROR;
    }

    return (peer_num.total_num - peer_num.encrypt_num);
}

int QuickESPNow::getNumberOfEncryptedPeers(){
    esp_now_peer_num_t peer_num;
    if(esp_now_get_peer_num(&peer_num) != ESP_OK){
        setup_errors[GET_NUMBER_OF_PEERS_ERROR] = GET_NUMBER_OF_PEERS_ERROR;
    }

    return peer_num.encrypt_num;
}


void QuickESPNow::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
    msg_recved = (status == ESP_NOW_SEND_SUCCESS);
}

#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
void QuickESPNow::OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    if(QuickESPNow::verify_peers && !isKnownMAC(info->src_addr)){
        return;
    }
    msg_struct receivedData;
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    QuickESPNow::recieved_msgs.add(&receivedData);
}
#elif ESP_ARDUINO_VERSION == ESP_ARDUINO_VERSION_VAL(2, 0, 17)
void QuickESPNow::OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    if(QuickESPNow::verify_peers && !isKnownMAC(mac_addr)){
        return;
    }
    msg_struct receivedData;
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    QuickESPNow::recieved_msgs.add(&receivedData);
}
#endif
uint8_t QuickESPNow::Local_MAC[MAC_LENGTH];

volatile bool QuickESPNow::msg_recved = false;

bool QuickESPNow::verify_peers = false;

int QuickESPNow::id_counter = 0;

uint8_t** QuickESPNow::Peers_MAC;

Msg_Queue QuickESPNow::recieved_msgs;
/***********************************************************************/

/**************Constructors**************/
QuickESPNow::QuickESPNow(const COMMUNICATION communication, const int peers_crowd, const uint8_t* new_local_MAC){
    if(communication == SENDER || communication == RECEIVER || communication == TWO_WAY_COMMUNICATION){
        this->ESP_COM = communication;
    }else{
        this->setup_errors[ESP_NOW_COMMUNICATION_SETUP_ERROR] = ESP_NOW_COMMUNICATION_SETUP_ERROR; 
    }

    this->ids = (int*)malloc(peers_crowd*sizeof(int));
    QuickESPNow::Peers_MAC = (uint8_t**)malloc(peers_crowd*sizeof(uint8_t*));
    for(int i=0; i<peers_crowd; i++){
        QuickESPNow::Peers_MAC[i] = (uint8_t*)malloc(MAC_LENGTH*sizeof(uint8_t));
    }

    this->Encryption = false;

    memcpy(QuickESPNow::Local_MAC, new_local_MAC, MAC_LENGTH);
}

QuickESPNow::QuickESPNow(const COMMUNICATION communication, const int peers_crowd, const uint8_t* new_local_MAC, const char* new_PMK_key){
    if(communication == SENDER || communication == RECEIVER || communication == TWO_WAY_COMMUNICATION){
        this->ESP_COM = communication;
    }else{
        this->setup_errors[ESP_NOW_COMMUNICATION_SETUP_ERROR] = ESP_NOW_COMMUNICATION_SETUP_ERROR;
    }

    this->ids = (int*)malloc(peers_crowd*sizeof(int));
    QuickESPNow::Peers_MAC = (uint8_t**)malloc(peers_crowd*sizeof(uint8_t*));
    for(int i=0; i<peers_crowd; i++){
        QuickESPNow::Peers_MAC[i] = (uint8_t*)malloc(MAC_LENGTH*sizeof(uint8_t));
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
        this->setup_errors[MEMORY_ALLOCATION_ERROR] = MEMORY_ALLOCATION_ERROR;
    }
}
/***************************************/

/**************Starting of the espnow communication**************/
void QuickESPNow::addPeer(int id, uint8_t* Peers_MAC, int Ch, wifi_interface_t mode){

    if(Ch < 0 || 14 < Ch){
        this->setup_errors[CHANNEL_OUT_OF_RANGRE] = CHANNEL_OUT_OF_RANGRE; 
    }

    if (esp_now_is_peer_exist(Peers_MAC)) {
        this->setup_errors[ADDED_USED_PEER_WARNING] = ADDED_USED_PEER_WARNING; 
    }

    this->ids[QuickESPNow::id_counter] = id;
    memcpy(QuickESPNow::Peers_MAC[QuickESPNow::id_counter], Peers_MAC, MAC_LENGTH);
    QuickESPNow::id_counter++;

    // Initialize the peerInfo structure
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(esp_now_peer_info_t));  // Properly zero-initialize

    memcpy(peerInfo.peer_addr, Peers_MAC, 6);
    peerInfo.channel = 1;  
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;


    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        this->setup_errors[ADD_PEER_INITIALIZATION_ERROR] = ADD_PEER_INITIALIZATION_ERROR; 
    }
}





void QuickESPNow::addPeer(int id, uint8_t* Peers_MAC, int Ch, wifi_interface_t mode, char* LMK_keys_array){
    if(Ch < 0 || 14 < Ch){
        this->setup_errors[CHANNEL_OUT_OF_RANGRE] = CHANNEL_OUT_OF_RANGRE; 
    }

    if (esp_now_is_peer_exist(Peers_MAC)) {
        this->setup_errors[ADDED_USED_PEER_WARNING] = ADDED_USED_PEER_WARNING; 
    }

    this->ids[QuickESPNow::id_counter] = id;
    memcpy(QuickESPNow::Peers_MAC[QuickESPNow::id_counter], Peers_MAC, MAC_LENGTH);
    QuickESPNow::id_counter++;

    // Initialize the peerInfo structure
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(esp_now_peer_info_t));
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
        return;
    }
}


void QuickESPNow::addPeer(int id, esp_now_peer_info_t* Peer){
    this->ids[QuickESPNow::id_counter] = id;
    memcpy(QuickESPNow::Peers_MAC[QuickESPNow::id_counter], Peer->peer_addr, MAC_LENGTH);
    QuickESPNow::id_counter++;

    // Add receiver as peer        
    if (esp_now_add_peer(Peer) != ESP_OK){
        return;
    }
}

void QuickESPNow::peerVerification(bool verify){
    verify_peers = verify;
}

void QuickESPNow::begin(wifi_mode_t mode){
    // Set the WiFi module to station mode
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        this->setup_errors[ESP_NOW_INITIALIZATION_ERROR] = ESP_NOW_INITIALIZATION_ERROR; 
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

    delay(100);

    // Set new MAC
    // esp_wifi_set_mac(WIFI_IF_STA, this->Local_MAC);

    switch (mode) {
        case WIFI_STA:
            esp_wifi_set_mac(WIFI_IF_STA, this->Local_MAC);
            break;
        case WIFI_AP:
            esp_wifi_set_mac(WIFI_IF_AP, this->Local_MAC);
            break;
        case WIFI_AP_STA:
            esp_wifi_set_mac(WIFI_IF_STA, this->Local_MAC);
            esp_wifi_set_mac(WIFI_IF_AP, this->Local_MAC);
            break;
        default:
            esp_wifi_set_mac(WIFI_IF_STA, this->Local_MAC);
            break;
    }

    delay(100);
    
    // Verify that the new MAC is set correctly

    if (!WiFi.macAddress().equals(getMACtoSTRING(this->Local_MAC))) {
        this->setup_errors[NEW_MAC_INITIALIZATION_ERROR] = NEW_MAC_INITIALIZATION_ERROR;
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
    bool error_encountered = false;
    for(int i = 0; i < SETTUP_ERRORS; i++) {
        if(this->setup_errors[i]==ESP_NOW_INITIALIZATION_ERROR) Serial.println("\r[Error] initializing ESP-NOW");
        else if(this->setup_errors[i]==ESP_NOW_COMMUNICATION_SETUP_ERROR ) Serial.println("\r[Error] in the consructors communication parameter");
        else if(this->setup_errors[i]==CHANNEL_OUT_OF_RANGRE) Serial.println("\r[Error] in the value of the channel (channel ranges 0-13)");        
        else if(this->setup_errors[i]==NEW_MAC_INITIALIZATION_ERROR) Serial.println("\r[Error] setting new MAC address");
        else if(this->setup_errors[i]==ADD_PEER_INITIALIZATION_ERROR) Serial.println("\r[Error] adding peer");
        else if(this->setup_errors[i]==MEMORY_ALLOCATION_ERROR) Serial.println("\r[Error] allocating memory");
        else if(this->setup_errors[i]==ADDED_USED_PEER_WARNING) Serial.println("\r[Warning] added a peer that has already been added");
        
        if(this->setup_errors[i] != NO_ERROR) {
            error_encountered = true;
        }
    }
    
    if(!error_encountered) {
        Serial.println("\r[SUCCESS] THERE WERE NO INITIALIZATION ERROR");
        return false;
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

bool QuickESPNow::isPeer(const uint8_t* MAC){
    esp_now_peer_info_t peer;
    return esp_now_get_peer(MAC, &peer) == ESP_OK;
}

bool QuickESPNow::isKnownMAC(const uint8_t* MAC){
    for(int i = 0; i < QuickESPNow::id_counter; i++){
        if(memcmp(MAC, QuickESPNow::Peers_MAC[i], 6) == 0){
            return true;
        }
    }
    
    return false;
}

int QuickESPNow::getPeerID(const uint8_t* MAC){
    for(int i = 0; i < QuickESPNow::id_counter; i++){
        if(memcmp(MAC, QuickESPNow::Peers_MAC[i], 6) == 0){
            return this->ids[QuickESPNow::id_counter];
        }
    }

    return -1;
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
    for(int i=0; i<QuickESPNow::id_counter; i++){
        free(QuickESPNow::Peers_MAC[i]);
    }
    free(QuickESPNow::Peers_MAC);
    free(this->PMK_key);
    free(ids);

    for(int i=0; i<QuickESPNow::id_counter; i++){
        esp_now_del_peer(QuickESPNow::Peers_MAC[i]);
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

