//Connect IR led on pin 3.

#include <IRremote.h>

IRsend irsend;

void setup() {}

void loop() {
    irsend.sendSony(103, 12);
    delay(200);
}
