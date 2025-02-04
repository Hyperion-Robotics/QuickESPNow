/***THIS IS A VERY SIMPLE EXCAMPLE THAT DOENT CHECK THE MACK OF THE SENDER IT JUST READS THE VALUE ALSO WE ASSUME THAT ONLY INTEGERS ARE SEND***/

#include "QuickESPNow.h"

#define MAX_PEERS 2

#define ID 2
#define SENDER 1
#define RECEIVER1 2
#define RECEIVER2 3
#define TWOWAY 4

#define CHANNEL 0


uint8_t MACS[MAX_PEERS +1 ][MAC_LENGTH] = {
  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA},//Sender MAC
  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAB},//RECEIBER1
  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAC},//RECEIVER2
  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAD}//TWOWAY ESP
};

volatile int received_value = -1;

//Use this parameters for esp32 board version after the 2.0.17
//void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len);

//Use this parameters for esp32 board that's from 2.0.17 and before
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len);

QuickESPNow receiver_2(RECEIVER, MAX_PEERS, MACS[ID]);

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);

  receiver_2.begin();
  // delay(500);
  receiver_2.addPeer(SENDER, MACS[0], CHANNEL, WIFI_IF_STA);
  receiver_2.addPeer(TWOWAY, MACS[3], CHANNEL, WIFI_IF_STA);
  receiver_2.FAIL_CHECK();//Since it has print function inside theres no need to print our own
}

void loop() {
    received_value = my_esp.read<int>();
    Serial.print("Received: ");
    Serial.println(received_value);

    delay(100);
}


// void customrecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len){
//   msg_struct receivedData;
//   memcpy(&receivedData, incomingData, sizeof(receivedData));

//   int current;
//   memcpy(&current, &receivedData.data_array[0], sizeof(int));

//   received_value = current;
// }

void customrecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len){
  msg_struct receivedData;
  memcpy(&receivedData, incomingData, sizeof(receivedData));

  int current;
  memcpy(&current, &receivedData.data_array[0], sizeof(int));

  received_value = current;
}