/** 
 * @author George Papamichail 
 * 
 * Copyright 2024 George Papamichail 
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at 
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * See the License for the specific language governing permissions and 
 * limitations under the License. 
 */
#ifndef QuickESPNow_h
#define QuickESPNow_h


#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>


#include "QuickESPNow_enums.h"
#include "QuickESPNow_utils.h"
#include "QuickESPNow_Queue.h"


/**
 * @class   QuickESPNow
 * @brief   This class implements the ESP-NOW communication protocol with support for encryption, peer management, and message queuing.
 */
class QuickESPNow {
  private:
    static Msg_Queue recieved_msgs; 
    /********The callback_fuctions for sending and reiciving messages********/

    /**
     * @brief   Callback function for handling sent messages.
     * @param   mac_addr MAC address of the peer to which the message was sent.
     * @param   status Status of the sent message (e.g., success or failure).
     */
    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
    
    #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
        /**
         * @brief   Callback function for handling received messages.
         * @param   info Information about the received message (e.g., RSSI, MAC address).
         * @param   incomingData The raw data received.
         * @param   len The length of the received data.
         */
        static void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len);
    #elif ESP_ARDUINO_VERSION == ESP_ARDUINO_VERSION_VAL(2, 0, 17)
        /**
         * @brief   Callback function for handling received messages.
         * @param   mac_addr MAC address of the peer that sent the message.
         * @param   incomingData The raw data received.
         * @param   len The length of the received data.
         */
        static void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len);
    #else
        #error unsapported board version
    #endif

    /************************************************************************/

    INITIALIZATION_ERRORS setup_errors[SETTUP_ERRORS + 1];  ///< Array to store initialization error statuses.
    

    int ESP_COM;                                        ///< Variable to manage the communication type (sending, receiving, or both).
    static uint8_t Local_MAC[MAC_LENGTH];               ///< Array to hold the local MAC address of the ESP.
    char* PMK_key = nullptr;                            ///< Pointer to hold the PMK encryption key for secure communication.
    
    static int id_counter;                                 ///< Counter to assign unique IDs to peers.
    int* ids;                                           ///< Pointer to an array storing IDs of peers.
    
    // static uint8_t** Peers_MAC;                         ///< Pointer to a 2D array storing MAC addresses of peers.
    static esp_now_peer_info* Peer_list;
    bool Encryption;                                    ///< Flag indicating whether encryption is enabled or not.

    static volatile bool msg_recved;
    static bool verify_peers;

  public:
    /********Constructors********/
    /**
     * @brief   Constructor for initializing espnow protocol
     * @param   communication This enum indicates whether the ESP is sending messages, receiving messages, or doing both
     * @param   peers_crowd This indicates how many ESPs are communicating with this ESP
     * @param   new_local_MAC The MAC address that is going to be assigned to this ESP
     */
    QuickESPNow(const COMMUNICATION communication, const int peers_crowd, const uint8_t* new_local_MAC);  
    
    /**
     * @brief   Constructor for initializing espnow protocol
     * @param   communication This enum indicates whether the ESP is sending messages, receiving messages, or doing both
     * @param   peers_crowd This indicates how many ESPs are communicating with this ESP
     * @param   new_local_MAC The MAC address that is going to be assigned to this ESP
     * @param   new_PMK_key The PMK encryption key used in the ESPs network
     */
    QuickESPNow(const COMMUNICATION communication, const int peers_crowd, const uint8_t* new_local_MAC, const char* new_PMK_key);  
    /********Constructors********/

    /********Setting up the esps network********/
    /**
     * @brief   Initializes the options set in the constructor
     */
    void begin(wifi_mode_t mode = WIFI_STA);

    /**
     * @brief   Changes the ESP's channel
     * @param   ch The channel that the ESP will be set
     * @note    The range of the channel can be from 0 to 13
     */
    void setChannel(int ch);
    
    /**
     * @brief   Adds the information of the peer
     * @param   id The ID number to be assigned to this peer
     * @param   Peers_MAC The peer's MAC adress
     * @param   Ch The peer's channel
     * @param   mode  The peers WIFI mode
     */
    void addPeer(int id, uint8_t* Peers_MAC, int Ch, wifi_interface_t mode);

    /**
     * @brief   Adds the information of the peer
     * @param   id The ID number to be assigned to this peer
     * @param   Peers_MAC The peer's MAC adress
     * @param   Ch The peer's channel
     * @param   mode  The peers WIFI mode
     * @param   LMK_keys_array The LMK key
     */
    void addPeer(int id, uint8_t* Peers_MAC, int Ch, wifi_interface_t mode, char* LMK_keys_array);

    /**
     * @brief   Adds the information of the peer
     * @param   id The ID number to be assigned to this peer
     * @param   Peer The peer's information struct
     * @note    It is recommended to use this method only if you want to add private information for the peer
     */
    void addPeer(int id, esp_now_peer_info_t* Peer);

    /**
     * @brief   Sets whether the received messages will be verified by MAC address.
     * @param   verify A boolean value indicating the verification status.
     *                 - true: Enable verification of received messages by MAC address.
     *                 - false: Disable verification of received messages by MAC address.
     * @note    When verification is enabled, the MAC address of the sender will be checked against known peers.
     */
    void peerVerification(bool verify);

    /**
     * @brief   Set custom send callback function
     * @param   custom The function to be called when a message is sent
     */
    void setCustomSendCallback(esp_now_send_cb_t custom);

    /**
     * @brief   Set custom reicieve callback function
     * @param   custom The function to be called when a message is received
     */
    void setCustomRecvCallback(esp_now_recv_cb_t custom);

    /**
     * @brief   Prints all the possible initialization errors
     * 
     * @return
     *          - false : There were no errors
     *          - true : There were errors
     */
    bool FAIL_CHECK();          // Check for initialization errors
    /********Setting up the esps network********/
    
    /********Msg sending and recieving methods********/
    /**
     * @brief   Checks if the ESP received any messages
     * 
     * @return  
     *          - true: Received a message
     *          - false: Did not receive a message
     */
    bool available() const;           // Check if a message was received

    /**
     * @brief   Method for sending non-pointers/non-arrays  
     * @tparam T The type of the array elements
     * @param   id Peers's setted ID
     * @param   msg The message to be sent
     */
    template<typename T> 
    bool Send(const int id, const T msg);

    /**
     * @brief   Method for sending arrays  
     * @tparam T The type of the array elements
     * @param   id Peers's setted ID
     * @param   msg The message to be sent
     * @param   size The size of the array
     */
    template<typename T> 
    bool Send(const int id, T* msg, int size);  

    /**
     * @brief   Method for sending non-pointers/non-arrays  
     * @tparam T The type of the array elements
     * @param   group The group of peers
     * @param   msg The message to be sent
     */
    template<typename T> 
    bool Send(const MAC peer, const T msg, int size);

    /**
     * @brief   Method for sending arrays  
     * @tparam T The type of the array elements
     * @param   id Peers's setted ID
     * @param   msg The message to be sent
     * @param   size The size of the array
     */
    template<typename T> 
    bool Send(const MAC* group, T* msg, int size); // method for sending arrays data 

    
    /**
     * @brief       Method for recieving the non-pointers/non-arrays messages
     * @tparam T The type of the array elements
     * @attention   The use of <_var_type_> is required
     * @attention   String and other class types are not supported
     * @attention   The variable type is unkown, it's based on the type variable of the message
     * @example     int recv_msg = object.read<int>();
     * 
     * @return
     *          - Unkown type : it's based on the type variable of the message
     */
    template<typename T> T read(); // method for sending non-pointer data 

    /**
     * @brief   Method for recieving the arrays messages
     * @tparam T The type of the array elements
     * @param   output The array that will copy the messages value
     * @attention   String and other class types are not supported
     * @attention   The variable type is unkown, it's based on the type variable of the message
     */
    template<typename T> void read_array(T* output); // method for sending pointer data 
    
    /**
     * @brief   Gives information about whether the received message is an array
     * 
     * @return
     *          - true : It's an array
     *          - false : It's not an array
     */
    bool isArray() const; // Check if the front node is an array

    /**
     * @brief   Gives information about the received message type
     * 
     * @return
     *          - ITN : The recieved message is type of int
     *          - SHORT : The recieved message is type of short
     *          - LONG : The recieved message is type of long
     *          - FLOAT : The recieved message is type of float
     *          - DOUBLE : The recieved message is type of double
     *          - CHAR : The recieved message is type of char
     *          - BOOL : The recieved message is type of bool
     *          - DATA : The recieved message is type of data struct
     */
    MSG_VARIABLE_TYPE data_type() const;

    static bool isPeer(const uint8_t* MAC);
    
    static bool isKnownMAC(const uint8_t* MAC);
    /********Msg sending and recieving methods********/
    
    /********Other utils********/
    /**
     * @brief   Get the peer's ID
     * @param   MAC The peer's MAC adress
     * 
     * @return
     *         - int : The peer's ID
     */
    int getPeerID(const uint8_t* MAC);

    /**
     * @brief   Get the ESP's MAC adress 
     * 
     * @return
     *          - uint8_t* : A 6 byte pointer of the ESP's MAC adress
     */
    static uint8_t* getEspMAC();
    
    /**
     * @brief   Get the ESP's MAC adress 
     * @param   MAC The variable that will be assigned a pointer of the ESP's MAC adress
     */
    static void getEspMAC(uint8_t* MAC); 

    /**
     * @brief   Get the number of all peers
     * @param   peer_num The variable that will be assigned the number of all peers
     */
    void getNumberOfALLPeers(esp_now_peer_num_t *peer_num);

    /**
     * @brief   Get the number of non-encrypted peers
     * 
     * @return
     *         - int : The number of non-encrypted peers
     */
    int getNumberOfNonEncryptedPeers();

    /**
     * @brief   Get the number of encrypted peers
     * 
     * @return
     *         - int : The number of encrypted peers
     */
    int getNumberOfEncryptedPeers();

    /**
     * @brief   Set WiFi mode to Station
     */
    void setWiFi_to_STA();

    /**
     * @brief   Set WiFi mode to Access Point 
     */
    void setWiFi_to_AP();

    /**
     * @brief   Set WiFi mode to Access Point and Station
     */
    void setWiFi_to_APSTA();
    /********Other utils********/

    /**
     * @brief   Destructor
     */
    ~QuickESPNow();
};

template<typename T> 
bool QuickESPNow::Send(const int id, T msg) {
    bool id_exists = false;
    int key;
    for(int i = 0; i<QuickESPNow::id_counter; i++){
        if(this->ids[i]==id){
            id_exists = true;
            key = i;
            break;
        }
    }
    
    if(!id_exists){
        return false;
    }

    if(!isKnownMAC(QuickESPNow::Peer_list[key].peer_addr)){
        return false;
    }

    if (esp_now_add_peer(&QuickESPNow::Peer_list[key]) != ESP_OK) {
        return false;
    }

    if(WiFi.channel() != QuickESPNow::Peer_list[key].channel){
        setChannel(QuickESPNow::Peer_list[key].channel);
    }

    msg_struct msg_to_sent;

    if constexpr (std::is_same<T, int>::value){
        msg_to_sent.type = INT;
    }else if constexpr (std::is_same<T, short>::value){
        msg_to_sent.type = SHORT;
    }else if constexpr (std::is_same<T, long>::value){
        msg_to_sent.type = LONG;
    }else if constexpr (std::is_same<T, float>::value){
        msg_to_sent.type = FLOAT;
    }else if constexpr (std::is_same<T, double>::value){
        msg_to_sent.type = DOUBLE;
    }else if constexpr (std::is_same<T, char>::value){
        msg_to_sent.type = CHAR;
    }else if constexpr (std::is_same<T, bool>::value){
        msg_to_sent.type = BOOL;
    }else if constexpr (std::is_same<T, data>::value){
        msg_to_sent.type = DATA;
    }else{
        msg_to_sent.type = UNKNOWN;
    }

    msg_to_sent.size = 1;
    msg_to_sent.data = (void*)(msg);

    bool result = esp_now_send(QuickESPNow::Peer_list[key].peer_addr, (uint8_t*)&msg_to_sent, sizeof(msg_to_sent)) == ESP_OK;

    bool success = (result && msg_recved);
    msg_recved = false;

    esp_now_del_peer(QuickESPNow::Peer_list[key].peer_addr);
    return success;
}

template<typename T> 
bool QuickESPNow::Send(const int id, T* msg, int size) {
    bool id_exists = false;
    int key;
    for(int i = 0; i<QuickESPNow::id_counter; i++){
        if(this->ids[i]==id){
            id_exists = true;
            key = i;
            break;
        }
    }
    
    if(!id_exists){
        return false;
    }

    if(!isKnownMAC(QuickESPNow::Peer_list[key].peer_addr)){
        return false;
    }

    if (esp_now_add_peer(&QuickESPNow::Peer_list[key]) != ESP_OK) {
        return false;
    }

    if(WiFi.channel() != QuickESPNow::Peer_list[key].channel){
        setChannel(QuickESPNow::Peer_list[key].channel);
    }

    msg_struct msg_to_sent;
    
    if constexpr (std::is_same<T, int>::value){
        msg_to_sent.type = INT;
    }else if constexpr (std::is_same<T, short>::value){
        msg_to_sent.type = SHORT;
    }else if constexpr (std::is_same<T, long>::value){
        msg_to_sent.type = LONG;
    }else if constexpr (std::is_same<T, float>::value){
        msg_to_sent.type = FLOAT;
    }else if constexpr (std::is_same<T, double>::value){
        msg_to_sent.type = DOUBLE;
    }else if constexpr (std::is_same<T, char>::value){
        msg_to_sent.type = CHAR;
    }else if constexpr (std::is_same<T, bool>::value){
        msg_to_sent.type = BOOL;
    }else{
        msg_to_sent.type = UNKNOWN;
    }
    
    msg_to_sent.size = size;
   
    for(int i = 0; i < size; i++){
        msg_to_sent.data_array[i] = (void*)(msg[i]);
    }
    
    // Send the message
    bool result = esp_now_send(QuickESPNow::Peer_list[key].peer_addr, (uint8_t*)&msg_to_sent, sizeof(msg_to_sent)) == ESP_OK;

    bool success = (result && msg_recved);
    msg_recved = false;

    esp_now_del_peer(QuickESPNow::Peer_list[key].peer_addr);
    return success;
}


// template<typename T> 
// bool QuickESPNow::Send(const esp_now_peer_info peer, const T msg){
//     if (esp_now_add_peer(&peer) != ESP_OK) {
//         return false;
//     }

//     msg_struct msg_to_sent;
    
//     if constexpr (std::is_same<T, int>::value){
//         msg_to_sent.type = INT;
//     }else if constexpr (std::is_same<T, short>::value){
//         msg_to_sent.type = SHORT;
//     }else if constexpr (std::is_same<T, long>::value){
//         msg_to_sent.type = LONG;
//     }else if constexpr (std::is_same<T, float>::value){
//         msg_to_sent.type = FLOAT;
//     }else if constexpr (std::is_same<T, double>::value){
//         msg_to_sent.type = DOUBLE;
//     }else if constexpr (std::is_same<T, char>::value){
//         msg_to_sent.type = CHAR;
//     }else if constexpr (std::is_same<T, bool>::value){
//         msg_to_sent.type = BOOL;
//     }else{
//         msg_to_sent.type = UNKNOWN;
//     }
    
//     msg_to_sent.size = 1;
   
//     msg_to_sent.data = (void*)(msg);
    
//     // Send the message
//     bool result = esp_now_send(peer.peer_addr, (uint8_t*)&msg_to_sent, sizeof(msg_to_sent)) == ESP_OK;

//     bool success = (result && msg_recved);
//     msg_recved = false;

//     esp_now_del_peer(peer.peer_addr);
//     return success;
// }

// template<typename T> 
// bool QuickESPNow::Send(const esp_now_peer_info peer, T* msg, int size){
//     if (esp_now_add_peer(&peer) != ESP_OK) {
//         return false;
//     }

//     msg_struct msg_to_sent;
    
//     if constexpr (std::is_same<T, int>::value){
//         msg_to_sent.type = INT;
//     }else if constexpr (std::is_same<T, short>::value){
//         msg_to_sent.type = SHORT;
//     }else if constexpr (std::is_same<T, long>::value){
//         msg_to_sent.type = LONG;
//     }else if constexpr (std::is_same<T, float>::value){
//         msg_to_sent.type = FLOAT;
//     }else if constexpr (std::is_same<T, double>::value){
//         msg_to_sent.type = DOUBLE;
//     }else if constexpr (std::is_same<T, char>::value){
//         msg_to_sent.type = CHAR;
//     }else if constexpr (std::is_same<T, bool>::value){
//         msg_to_sent.type = BOOL;
//     }else{
//         msg_to_sent.type = UNKNOWN;
//     }
    
//     msg_to_sent.size = size;
   
//     for(int i = 0; i < size; i++){
//         msg_to_sent.data_array[i] = (void*)(msg[i]);
//     }
    
//     // Send the message
//     bool result = esp_now_send(peer.peer_addr, (uint8_t*)&msg_to_sent, sizeof(msg_to_sent)) == ESP_OK;

//     bool success = (result && msg_recved);
//     msg_recved = false;

//     esp_now_del_peer(peer.peer_addr);
//     return success;
// }

template<typename T>
T QuickESPNow::read(){
    return QuickESPNow::recieved_msgs.pop<T>();
}


template<typename T>
void QuickESPNow::read_array(T* output){
    QuickESPNow::recieved_msgs.popArray(output);
}
#endif
