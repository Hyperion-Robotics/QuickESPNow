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
#ifndef QuickESPNow_Queue_h
#define QuickESPNow_Queue_h

#include <cstddef>
#include <type_traits>
#include <Arduino.h>

#include "QuickESPNow_utils.h"

/**
 * @class   Msg_Queue
 * @brief   A template-based message queue capable of storing any type of data, including arrays.
 */
class Msg_Queue {
    private:
        /**
         * @struct  node
         * @brief   Template class that inherits from father_node. Holds either a single value or an array of values.
         * @tparam  T Type of data that the node stores.
         */
        struct node {
            msg_struct msg;
            node* next;
        };

        node* front; ///< Pointer to the front of the queue.
        node* rear;  ///< Pointer to the rear of the queue.
    public:
        /**
         * @brief   Constructor to initialize an empty queue.
         */
        Msg_Queue(); 
        
        /**
         * @brief   Destructor to clean up the Msg_Queue.
         */
        ~Msg_Queue();               

        /**
         * @brief   Adds a single value to the queue (enqueue).
         * @tparam  T The type of the value to be added.
         * @param   value The value to be added.
         */
        void add(msg_struct* value);

        /**
         * @brief   Removes and returns a single value from the front of the queue (dequeue).
         * @tparam  T The type of the value to be returned.
         * @return  T The dequeued value.
         */
        template<typename T>
        T pop();            

        /**
         * @brief Removes and copies an array from the front of the queue (dequeue).
         * @tparam T The type of the array elements.
         * @param output Pointer to the output array where the dequeued data will be copied.
         */   
        template<typename T>
        void popArray(T* output);          

        /**
         * @brief Checks if the queue is empty.
         * 
         * @return true if the queue is empty, false otherwise.
         */
        bool isEmpty() const;    

        /**
         * @brief Checks if the front node contains an array.
         * 
         * @return 
         *          -true : The front node is an array
         *          -false : The front node is not an array.
         */
        bool isFrontArray() const; 

        /**
         * @brief Gets the data type of the front node.
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
};


template<typename T>
T Msg_Queue::pop() {
    if (front == nullptr) {
        return T(); // Return default-constructed object of type T
    }

    node* temp = front;
    front = front->next;

    if (front == nullptr) {
        rear = nullptr;
    }

    // Ensure that temp->msg is of the type we expect (it must be compatible with T)
    // If you need to deserialize it or perform a cast, you can do it here
    T value;
    memcpy(&value, &temp->msg.data, sizeof(T)); // Copy the raw memory from temp->msg

    delete temp; // Clean up memory

    return value; // Return the value of the appropriate type
}

template<typename T>
void Msg_Queue::popArray(T* output) {
    if (front == nullptr) {
        return; // Queue is empty
    }

    node* temp = front;
    front = front->next;

    if (front == nullptr) {
        rear = nullptr;
    }

    for(int i = 0; i < temp->msg.size; i++){
        T value;
        memcpy(&value, &temp->msg.data_array[i], sizeof(T)); // Copy the raw memory from temp->msg
        output[i] = value;
    }
    
    delete temp;
}


#endif