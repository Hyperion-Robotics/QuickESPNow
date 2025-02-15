#include "QuickESPNow_utils.h"





void getSTRINGtoMAC(String text, uint8_t *new_mac){
    String clean_MAC;

    for(char ch : text){
        if(ch != ':')
            clean_MAC += ch;
    }

    for(int i=0;i<MAC_LENGTH;i++){
        String bt_str = clean_MAC.substring(i*2,i*2+2);
        new_mac[i] = (uint8_t)strtol(bt_str.c_str(), NULL, 16);
    }
}

String getMACtoSTRING(const uint8_t *mac){
  // Create a String to store the MAC address
    String macString;

    // Iterate through each byte of the MAC address
    for (int i = 0; i < MAC_LENGTH; i++) {
        // Convert the byte to a two-digit hexadecimal string
        char hexBuffer[3];
        sprintf(hexBuffer, "%02X", mac[i]);

        // Concatenate the hexadecimal string to the macString
        macString += hexBuffer;

        // Add a colon (:) after each byte except the last one
        if (i < MAC_LENGTH - 1) {
            macString += ":";
        }
    }
    return macString;
}

// Set the parameters of the data struct
void Set_Data_parameters(data *new_struct,char type, char new_char[], int new_int, float new_float, bool new_bool) {
  if (new_char != NULL) {
    strncpy(new_struct->msg_char, new_char, STRING_LENGTH);
  }
  new_struct->type = type;
  new_struct->msg_int = new_int;
  new_struct->msg_float = new_float;
  new_struct->msg_bool = new_bool;
}
