#include "QuickESPNow_Queue.h"

// Constructor for Msg_Queue
Msg_Queue::Msg_Queue() : front(nullptr), rear(nullptr) {}

// Destructor to clean up the Msg_Queue
Msg_Queue::~Msg_Queue() {
    while (!isEmpty()) {
        node* temp = front;
        front = front->next;
        delete temp;  // Clean up the base node directly
    }
}

void Msg_Queue::add(msg_struct* value) {
    node* newNode = new node; // Dynamically allocate memory for a new node
    memcpy(&newNode->msg, value, sizeof(msg_struct));
    newNode->next = nullptr;  // Initialize the next pointer to nullptr

    if (rear == nullptr) {
        // Queue is empty, both front and rear should point to the new node
        front = rear = newNode;
    } else {
        // Link the new node at the end and update the rear pointer
        rear->next = newNode;
        rear = newNode;
    }
}

// Check if the Msg_Queue is empty
bool Msg_Queue::isEmpty() const {
    return front == nullptr;
}

// Implementation of isFrontArray
bool Msg_Queue::isFrontArray() const {
    return front->msg.size == 0; // Queue is empty, no front node
}
// Implementation of isFrontArray
 MSG_VARIABLE_TYPE Msg_Queue::data_type() const{
    if (front == nullptr) {
        return UNKNOWN; // Queue is empty, no front node
    }

    
    return front->msg.type; // Return isArray if cast is successful
}

