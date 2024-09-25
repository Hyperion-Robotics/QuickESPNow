#include "esp_now_HR.h"

#define ID 3

int timer = 5;
int led = 0;

uint8_t MACS[4][MAC_LENGTH] = {
  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA},
  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAB},
  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAC},
  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAD}
};

uint8_t senders_MAC[MAC_LENGTH] = {0xFF, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

ESP_NOW_HR my_esp(RECEIVER, 1, MACS[ID]);

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);

  my_esp.begin();
  // delay(500);
  my_esp.addPeer(0, senders_MAC, 0, WIFI_IF_STA);
  my_esp.FAIL_CHECK();
  // my_esp.FAIL_CHECK();
}

void loop() {
  if(my_esp.available()){
    timer = my_esp.read<int>();
  }

  if(timer > 0){
    digitalWrite(led, HIGH);
    sleep(timer);
    digitalWrite(led, LOW);
    timer = 0;
  }
}
