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
#ifndef QuickESPNow_enums_h 
#define QuickESPNow_enums_h

///< Define constants
#define MAC_LENGTH 6                    ///< Length of MAC address
#define STRING_LENGTH 40                ///< Maximum string length
#define SETTUP_ERRORS 8                 ///< Number of setup errors
#define ENCRYPTION_KEY_LENGTH 16        ///< Length of the encryption key
#define VARIABLE_TYPES 17

/**
 * @brief   Enum for communication modes
 */ 
enum COMMUNICATION {
    TWO_WAY_COMMUNICATION,  ///< Bidirectional communication
    SENDER,                 ///< Sending only
    RECEIVER                ///< Receiving only
};

/**
 * @brief   Enum for initialization error types.
 */ 
enum INITIALIZATION_ERRORS {
    NO_ERROR,                            ///< No error                             
    ESP_NOW_INITIALIZATION_ERROR,        ///< [Error] initializing ESP-NOW                              *1              
    ESP_NOW_COMMUNICATION_SETUP_ERROR,   ///< [Error] in the consructors communication parameter        *2                                         
    CHANNEL_OUT_OF_RANGRE,               ///< [Error] in the value of the channel (channel ranges 0-13) *3                                  
    NEW_MAC_INITIALIZATION_ERROR,        ///< [Error] setting new MAC address                           *4                  
    ADD_PEER_INITIALIZATION_ERROR,       ///< [Error] adding peer                                       *5      
    MEMORY_ALLOCATION_ERROR,             ///< [Error] allocating memory                                 *6
    ADDED_USED_PEER_WARNING,             ///< [Warning] added a peer that has already been added        *7
    GET_NUMBER_OF_PEERS_ERROR            ///< [Error] getting the number of peers                       *8
};

/**
 * @brief   Enum for variable types.
 */
enum MSG_VARIABLE_TYPE {
    INT,        ///< integer
    SHORT,      ///< short
    LONG,       ///< long
    FLOAT,      ///< float
    DOUBLE,     ///< double
    CHAR,       ///< char
    STRING,     ///< string
    BOOL,       ///< boolean
    DATA,       ///< Data struct
    UNKNOWN     ///< Unknown type
};

#endif
