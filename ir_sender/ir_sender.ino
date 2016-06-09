#include <IRremote.h>

IRsend irsend;

void setup() {}

void loop() {
    irsend.sendNEC(0x3, 2);
    delay(50); //5 second delay between each signal burst
}