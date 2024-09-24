/**
 * @author George Papamichail
 */
#ifndef esp_now_HR_h
#define esp_now_HR_h


#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>

#include "esp_now_HR_enums.h"
#include "esp_now_HR_utils.h"
#include "esp_now_HR_Queue.h"



/**
 * @class   ESP_NOW_HR
 * @brief   This class implements the ESP-NOW communication protocol with support for encryption, peer management, and message queuing.
 */
class ESP_NOW_HR {
  private:
    static Msg_Queue recieved_msgs; 
    /********The callback_fuctions for sending and reiciving messages********/

    /**
     * @brief   Sets a received message in the message queue.
     * @param   msg The message structure to be added.
     */
    static void setRecvMsg(msg_struct msg);
    
    /**
     * @brief   Callback function for handling sent messages.
     * @param   mac_addr MAC address of the peer to which the message was sent.
     * @param   status Status of the sent message (e.g., success or failure).
     */
    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

    /**
     * @brief   Callback function for handling received messages.
     * @param   info Information about the received message (e.g., RSSI, MAC address).
     * @param   incomingData The raw data received.
     * @param   len The length of the received data.
     */
    static void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len);
    /************************************************************************/

    int error_counter = 0;                              ///< Counter to track the number of errors during initialization.
    INITIALIZATION_ERRORS setup_errors[SETTUP_ERRORS];  ///< Array to store initialization error statuses.
    

    int ESP_COM;                                        ///< Variable to manage the communication type (sending, receiving, or both).
    static uint8_t Local_MAC[MAC_LENGTH];               ///< Array to hold the local MAC address of the ESP.
    char* PMK_key = nullptr;                            ///< Pointer to hold the PMK encryption key for secure communication.
    
    int id_counter = 0;                                 ///< Counter to assign unique IDs to peers.
    int* ids;                                           ///< Pointer to an array storing IDs of peers.
    
    uint8_t** Peers_MAC;                                ///< Pointer to a 2D array storing MAC addresses of peers.
    bool Encryption;                                    ///< Flag indicating whether encryption is enabled or not.
    char** LMK_key;                                     ///< Pointer to an array of LMK encryption keys for each peer.

  public:
    /********Constructors********/
    /**
     * @brief   Constructor for initializing espnow protocol
     * @param   communication This enum indicates whether the ESP is sending messages, receiving messages, or doing both
     * @param   peers_crowd This indicates how many ESPs are communicating with this ESP
     * @param   new_local_MAC The MAC address that is going to be assigned to this ESP
     */
    ESP_NOW_HR(const COMMUNICATION communication, const int peers_crowd, const uint8_t* new_local_MAC);  
    
    /**
     * @brief   Constructor for initializing espnow protocol
     * @param   communication This enum indicates whether the ESP is sending messages, receiving messages, or doing both
     * @param   peers_crowd This indicates how many ESPs are communicating with this ESP
     * @param   new_local_MAC The MAC address that is going to be assigned to this ESP
     * @param   new_PMK_key The PMK encryption key used in the ESPs network
     */
    ESP_NOW_HR(const COMMUNICATION communication, const int peers_crowd, const uint8_t* new_local_MAC, const char* new_PMK_key);  
    /********Constructors********/

    /********Setting up the esps network********/
    /**
     * @brief   Initializes the options set in the constructor
     */
    void begin();

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
     *          - true : There were no errors
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
    void Send(const int id, const T msg);

    /**
     * @brief   Method for sending arrays  
     * @tparam T The type of the array elements
     * @param   id Peers's setted ID
     * @param   msg The message to be sent
     * @param   size The size of the array
     */
    template<typename T> 
    void Send(const int id, const T* msg, int size); // method for sending arrays data 
    
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
    /********Msg sending and recieving methods********/

    /********Other utils********/
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
     * @brief   Set WiFi mode to Station
     */
    void setWiFi_to_STA();

    /**
     * @brief   Set WiFi mode to Access Point 
     */
    void setWiFi_to_AP();

    /**
     * @brief   Set WiFi mode to Access Point + Station
     */
    void setWiFi_to_APSTA();
    /********Other utils********/

    /**
     * @brief   Destructor
     */
    ~ESP_NOW_HR();
};

template<typename T> 
void ESP_NOW_HR::Send(const int id, const T msg) {
    bool id_exists = false;
    int key;
    for(int i = 0; i<this->id_counter; i++){
        if(this->ids[i]==id){
            id_exists = true;
            key = i;
            break;
        }
    }
    
    esp_now_peer_info_t* temp_peer;
    esp_now_get_peer(this->Peers_MAC[key], temp_peer);


    if(!id_exists){
        Serial.println("[Fail] Unknown esp id");
        return;
    }

    if(WiFi.channel() == temp_peer->channel){
        WiFi.setChannel(temp_peer->channel);
        delay(100);
    }

    msg_struct msg_to_sent;
    msg_to_sent.array = false;
    
    if constexpr (std::is_same_v<T, int>){
        msg_to_sent.type = INT;
        msg_to_sent.msg.i = msg;
    }else if constexpr (std::is_same_v<T, short>){
        msg_to_sent.type = SHORT;
        msg_to_sent.msg.s = msg;
    }else if constexpr (std::is_same_v<T, long>){
        msg_to_sent.type = LONG;
        msg_to_sent.msg.l = msg;
    }else if constexpr (std::is_same_v<T, float>){
        msg_to_sent.type = FLOAT;
        msg_to_sent.msg.f = msg;
    }else if constexpr (std::is_same_v<T, double>){
        msg_to_sent.type = DOUBLE;
        msg_to_sent.msg.d = msg;
    }else if constexpr (std::is_same_v<T, char>){
        msg_to_sent.type = CHAR;
        msg_to_sent.msg.c = msg;
    }else if constexpr (std::is_same_v<T, bool>){
        msg_to_sent.type = BOOL;
        msg_to_sent.msg.b = msg;
    }else if constexpr (std::is_same_v<T, data>){
        msg_to_sent.type = DATA;
        msg_to_sent.msg.dt = msg;
    }


    bool result = esp_now_send(this->Peers_MAC[key], (uint8_t*) &msg_to_sent, sizeof(msg_to_sent));
    result == ESP_OK ? Serial.println("Successfully sent msg") : Serial.println("Failed to send msg");
}

template<typename T> 
void ESP_NOW_HR::Send(const int id, const T* msg, int size) {
    bool id_exists = false;
    int key;
    for(int i = 0; i<this->id_counter; i++){
        if(this->ids[i]==id){
            id_exists = true;
            key = i;
            break;
        }
    }

    if(!id_exists){
        Serial.println("Unknown esp id");
        return;
    }

    esp_now_peer_info_t* temp_peer;
    esp_now_get_peer(this->Peers_MAC[key], temp_peer);

    if(WiFi.channel() == temp_peer->channel){
        WiFi.setChannel(temp_peer->channel);
        delay(100);
    }


    msg_struct msg_to_sent;
    msg_to_sent.array = true;

    if constexpr (std::is_same_v<T, int>) {
        msg_to_sent.type = INT;
        msg_to_sent.msg.i_ptr = (T*)malloc(size * sizeof(T));
        for (int i = 0; i < size; i++) {
            msg_to_sent.msg.i_ptr[i] = msg[i];
        }
    }else if constexpr (std::is_same_v<T, short>) {
        msg_to_sent.type = SHORT;
        msg_to_sent.msg.s_ptr = (T*)malloc(size * sizeof(T));
        for (int i = 0; i < size; i++) {
            msg_to_sent.msg.s_ptr[i] = msg[i];
        }
    }else if constexpr (std::is_same_v<T, long>) {
        msg_to_sent.type = LONG;
        msg_to_sent.msg.l_ptr = (T*)malloc(size * sizeof(T));
        for (int i = 0; i < size; i++) {
            msg_to_sent.msg.l_ptr[i] = msg[i];
        }
    }else if constexpr (std::is_same_v<T, float>) {
        msg_to_sent.type = FLOAT;
        msg_to_sent.msg.f_ptr = (T*)malloc(size * sizeof(T));
        for (int i = 0; i < size; i++) {
            msg_to_sent.msg.f_ptr[i] = msg[i];
        }
    }else if constexpr (std::is_same_v<T, double>) {
        msg_to_sent.type = DOUBLE;
        msg_to_sent.msg.d_ptr = (T*)malloc(size * sizeof(T));
        for (int i = 0; i < size; i++) {
            msg_to_sent.msg.d_ptr[i] = msg[i];
        }
    }else if constexpr (std::is_same_v<T, char>) {
        msg_to_sent.type = CHAR;
        msg_to_sent.msg.c_ptr = (T*)malloc(size * sizeof(T));
        for (int i = 0; i < size; i++) {
            msg_to_sent.msg.c_ptr[i] = msg[i];
        }
    }else if constexpr (std::is_same_v<T, bool>) {
        msg_to_sent.type = BOOL;
        msg_to_sent.msg.b_ptr = (T*)malloc(size * sizeof(T));
        for (int i = 0; i < size; i++) {
            msg_to_sent.msg.b_ptr[i] = msg[i];
        }
    }else if constexpr (std::is_same_v<T, data>) {
        msg_to_sent.type = DATA;
        msg_to_sent.msg.dt_ptr = (T*)malloc(size * sizeof(T));
        for (int i = 0; i < size; i++) {
            msg_to_sent.msg.dt_ptr[i] = msg[i];
        }
    }

    

    bool result = esp_now_send(this->Peers_MAC[key], (uint8_t*) &msg_to_sent, sizeof(msg_to_sent));
    result == ESP_OK ? Serial.println("Successfully sent msg") : Serial.println("Failed to send msg");

    if(msg_to_sent.type == INT) free(msg_to_sent.msg.i_ptr);
    else if(msg_to_sent.type == SHORT) free(msg_to_sent.msg.s_ptr);
    else if(msg_to_sent.type == LONG) free(msg_to_sent.msg.l_ptr);
    else if(msg_to_sent.type == FLOAT) free(msg_to_sent.msg.f_ptr);
    else if(msg_to_sent.type == DOUBLE) free(msg_to_sent.msg.d_ptr);
    else if(msg_to_sent.type == CHAR) free(msg_to_sent.msg.c_ptr);
    else if(msg_to_sent.type == BOOL) free(msg_to_sent.msg.b_ptr);
    else if(msg_to_sent.type == DATA) free(msg_to_sent.msg.dt_ptr);
}

template<typename T>
T ESP_NOW_HR::read(){
    return ESP_NOW_HR::recieved_msgs.pop<T>();
}


template<typename T>
void ESP_NOW_HR::read_array(T* output){
    ESP_NOW_HR::recieved_msgs.popArray(output);
}

#endif