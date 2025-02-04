#include "QuickESPNow.h"

#define MAX_PEERS 3

#define ID 0
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

QuickESPNow sender(SENDER, MAX_PEERS, MACS[ID]);

void setup() {
  Serial.begin(115200);

  // delay(500);
  sender.begin();
  sender.addPeer(RECEIVER1, MACS[1], CHANNEL, WIFI_IF_STA);//
  sender.addPeer(RECEIVER2, MACS[2], CHANNEL, WIFI_IF_STA);//
  sender.addPeer(RECEIVER2, MACS[3], CHANNEL, WIFI_IF_STA);//
  sender.FAIL_CHECK();//Since it has print function inside theres no need to print our own
}

void loop() {
  my_esp.Send(RECEIVER1, 1);
  delay(1000);
  my_esp.Send(RECEIVER2, 1);
  delay(1000);
  my_esp.Send(TWOWAY, 1);
  delay(1000);
  my_esp.Send(RECEIVER1, 1);
  my_esp.Send(RECEIVER2, 1);
  delay(200);
  my_esp.Send(TWOWAY, 1);
  delay(1000);
  if(!my_esp.Send(RECEIVER1, 4);){
    my_esp.Send(TWOWAY, 8);
  }else{
    my_esp.Send(RECEIVER2, 9);
  }
  delay(500);
}
