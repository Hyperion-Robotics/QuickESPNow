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
#ifndef QuickESPNow_utils_h
#define QuickESPNow_utils_h

#include <cstddef>
#include <Arduino.h>

#include "QuickESPNow_enums.h"

/**
 * @brief   Create a struct that contains the data of the message
 */
typedef struct {
    char type;                      ///< Type of the message (character).
    char msg_char[STRING_LENGTH];   ///< Array to hold character message.
    int msg_int;                    ///< Integer message.
    float msg_float;                ///< Float message.
    bool msg_bool;                  ///< Boolean message.
} data;

/**
 * @brief   Create a struct that contains the data of the message
 */
typedef struct {
    MSG_VARIABLE_TYPE type;             ///< Type of the message (Enum).
    int size;                           ///< Size of the message.
    void* data = nullptr;               ///< Pointer to the data.
    void* data_array[STRING_LENGTH];    ///< Array to hold the data.
} msg_struct;

/**
 * @brief   Create a struct that contains the MAC addresses for multicasting
 */
typedef struct {
    uint8_t** MACS;
} group_MAC;

/**
 * @brief   Create a struct that contains ids for multicasting
 * @warning The ids must be given with the .addPeer method or it will fail so an unkown id will be skipped
 */
typedef struct{
    int* ids;
} group_ids;

/**
 * @brief Function to set data parameters in the data structure.
 * @param new_struct Pointer to the data structure to be modified.
 * @param type The type of message to set.
 * @param new_char Character array to set in the message.
 * @param new_int Integer value to set in the message.
 * @param new_float Float value to set in the message.
 * @param new_bool Boolean value to set in the message.
 */
void Set_Data_parameters(data *new_struct, char type, char new_char[], int new_int, float new_float, bool new_bool);

/**
 * @brief Function to convert MAC address to a string.
 * @param mac Pointer to the MAC address.
 * @return String representation of the MAC address.
 */
String getMACtoSTRING(const uint8_t *mac);

/**
 * @brief Function to convert string to MAC address.
 * @param text String representation of the MAC address.
 * @param new_mac Pointer to store the converted MAC address.
 */
void getSTRINGtoMAC(String text, uint8_t *new_mac);

#endif
