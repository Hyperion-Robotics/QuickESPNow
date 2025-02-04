#include "QuickESPNow.h"

#define MAX_PEERS 2

#define ID 1
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

QuickESPNow receiver_1(RECEIVER, MAX_PEERS, MACS[ID]);

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);

  receiver_1.begin();
  // delay(500);
  receiver_1.addPeer(SENDER, MACS[0], CHANNEL, WIFI_IF_STA);
  receiver_1.addPeer(TWOWAY, MACS[3], CHANNEL, WIFI_IF_STA);
  receiver_1.FAIL_CHECK();//Since it has print function inside theres no need to print our own
}

void loop() {
  int received_value;
  if(receiver_1.available()){
    received_value = my_esp.read<int>();
    Serial.print("Received: ");
    Serial.println(received_value);
  }
  delay(100);
}
