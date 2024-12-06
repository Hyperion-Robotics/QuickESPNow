#include "QuickESPNow.h"


uint8_t MACS[4][MAC_LENGTH] = {
  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA},
  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAB},
  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAC},
  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAD}
};

uint8_t Senders_MAC[MAC_LENGTH] = {0x84, 0xfc, 0xe6, 0x08, 0xee, 0x7c};

QuickESPNow my_esp(SENDER, 4, Senders_MAC);

void setup() {
  Serial.begin(115200);

  // delay(500);
  my_esp.begin();
  my_esp.addPeer(0, MACS[0], 0, WIFI_IF_STA);
  my_esp.addPeer(1, MACS[1], 0, WIFI_IF_STA);
  my_esp.addPeer(2, MACS[2], 0, WIFI_IF_STA);
  my_esp.addPeer(3, MACS[3], 0, WIFI_IF_STA);
  my_esp.FAIL_CHECK();
}

void loop() {
  my_esp.Send(0, 1);
  delay(1000);
  my_esp.Send(1, 1);
  delay(1000);
  my_esp.Send(2, 1);
  delay(1000);
  my_esp.Send(3, 1);
  delay(1000);
  my_esp.Send(2, 1);
  delay(1000);
  my_esp.Send(1, 1);
  delay(1000);
  my_esp.Send(0, 1);
  delay(1000);
  my_esp.Send(3, 5);
  delay(5000);
}
