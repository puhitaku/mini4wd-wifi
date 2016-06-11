#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <FS.h>
#include <IRremoteESP8266.h>

using namespace std;

const char motor_pin = 15;
const char ir_pin = 4;

const char dns_port = 53;
const char* ssid = "mini4wd-xxxx";
const char* pass = "miniyonku-xxxx";
const char* host = "mini4wd";
const char* host_dns = "mini4wd.local";

IPAddress ip = IPAddress(192, 168, 1, 1);
ESP8266WebServer server(80);
DNSServer dns_server;
File file;

IRrecv ir_receiver(ir_pin);
decode_results ir_results;

char last_speed_level = 0;

void SendOk(void) {
    server.send(200, "text/plain", "OK");
}

void MotorSetVal(unsigned int v) {
    analogWrite(motor_pin, v);
    SendOk();
}

void MotorStop(void) {
    MotorSetVal(0);
}

void MotorSlow(void) {
    MotorSetVal(256);
}

void MotorNormal(void) {
    MotorSetVal(512);
}

void MotorMax(void) {
    MotorSetVal(1023);
}

void ChangeSpeed(char speed_level) {
    if (last_speed_level != speed_level) {
        switch (speed_level) {
        case 0:
            MotorStop();
            break;
        case 1:
            MotorSlow();
            break;
        case 2:
            MotorNormal();
            break;
        case 3:
            MotorMax();
            break;
        }
        last_speed_level = speed_level;
    }
}

void setup(void) {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    SPIFFS.begin();

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(ip, ip, IPAddress(255, 255, 255, 0));
    WiFi.softAP(ssid, pass);
    Serial.print("AP's IP:");
    Serial.println(WiFi.softAPIP());

    dns_server.start(dns_port, host_dns, ip);
    Serial.println("http://mini4wd.local (on Mac / iOS / Android)");
    Serial.println("http://192.168.1.1 (on Windows)");

    MDNS.begin(host);
    server.on("/", [](){ server.send(200, "text/plain", "Hello Mini-4WD!"); });
    server.on("/0", [](){ ChangeSpeed(0); });
    server.on("/1", [](){ ChangeSpeed(1); });
    server.on("/2", [](){ ChangeSpeed(2); });
    server.on("/3", [](){ ChangeSpeed(3); });
    server.begin();
    MDNS.addService("http", "tcp", 80);

    analogWriteFreq(100);

    ir_receiver.enableIRIn();
}

void loop() {
    if (ir_receiver.decode(&ir_results)) {
        if (ir_results.decode_type == SONY) {
            switch (ir_results.value) {
            case 100:
            case 101:
            case 102:
            case 103:
                //Decrease 100 to get actual operation
                //Operation number is biased for checking integrity
                ChangeSpeed(ir_results.value - 100);
                break;
            }
        }
        Serial.println(ir_results.value);
        ir_receiver.resume();
    }
    dns_server.processNextRequest();
    server.handleClient();
}
