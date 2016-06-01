#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <FS.h>

const char motor_pin = 15;

const char dns_port = 53;
const char* ssid = "mini4wd";
const char* pass = "miniyonku";
const char* host = "mini4wd";
const char* host_dns = "mini4wd.local";

IPAddress ip = IPAddress(192, 168, 1, 1);

ESP8266WebServer server(80);
DNSServer dns_server;
File file;

void MotorSlow(void) {
  analogWrite(motor_pin, 256);
  server.send(200, "text/plain", "OK");
}

void MotorNormal(void) {
  analogWrite(motor_pin, 512);
  server.send(200, "text/plain", "OK");
}

void MotorMax(void) {
  analogWrite(motor_pin, 1023);
  server.send(200, "text/plain", "OK");
}

void MotorStop(void) {
  analogWrite(motor_pin, 0);
  server.send(200, "text/plain", "OK");
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
  server.on("/1", MotorSlow);
  server.on("/2", MotorNormal);
  server.on("/3", MotorMax);
  server.on("/s", MotorStop);
  server.begin();
  MDNS.addService("http", "tcp", 80);

  analogWriteFreq(100);
}

void loop() {
  dns_server.processNextRequest();
  server.handleClient();
}
