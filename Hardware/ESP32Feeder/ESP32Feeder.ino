// PawsAndPlay - ESP32 otomatik mama kabi
// HTTP API (port 80):
//   GET  /status                      -> {"portionsToday":n,"maxPerDay":n,"cooldownLeft":sn}
//   POST /feed  (X-Feeder-Key header) -> 200 dokuldu | 401 anahtar hatali | 429 bekleme/gunluk limit
// Hayvani korumak icin limitler oyunda degil BURADA uygulanir: oyun ne gonderirse gondersin
// cihaz bekleme suresi ve gunluk porsiyon sinirini asmaz.
// Gerekli kutuphane: ESP32Servo (Arduino Library Manager).

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include "secrets.h"  // WIFI_SSID, WIFI_PASS, FEEDER_KEY  (secrets.example.h'yi kopyalayin)

const int SERVO_PIN = 13;
const int CLOSED_ANGLE = 0;
const int OPEN_ANGLE = 90;
const unsigned long OPEN_MS = 600;               // kalibre edin: 1 porsiyonun gram miktarini belirler
const unsigned long COOLDOWN_MS = 60UL * 1000;   // iki porsiyon arasi en az bekleme
const int MAX_PER_DAY = 20;                      // gunluk ust sinir
const unsigned long DAY_MS = 24UL * 60 * 60 * 1000;

WebServer server(80);
Servo gate;
int portionsToday = 0;
unsigned long dayStart = 0;
unsigned long lastFeed = 0;
bool fedOnce = false;

unsigned long cooldownLeftMs() {
  if (!fedOnce) return 0;
  unsigned long elapsed = millis() - lastFeed;
  return elapsed >= COOLDOWN_MS ? 0 : COOLDOWN_MS - elapsed;
}

void rollDay() {
  // shortcut: 24 saatlik pencere acilistan itibaren sayilir, gece yarisi sifirlamak icin NTP ekleyin.
  if (millis() - dayStart >= DAY_MS) {
    dayStart = millis();
    portionsToday = 0;
  }
}

void sendStatus(int code) {
  rollDay();
  String json = String("{\"portionsToday\":") + portionsToday +
                ",\"maxPerDay\":" + MAX_PER_DAY +
                ",\"cooldownLeft\":" + (cooldownLeftMs() / 1000) + "}";
  server.send(code, "application/json", json);
}

void handleFeed() {
  if (strlen(FEEDER_KEY) == 0 || server.header("X-Feeder-Key") != FEEDER_KEY) {
    server.send(401, "application/json", "{\"error\":\"bad key\"}");
    return;
  }
  rollDay();
  if (portionsToday >= MAX_PER_DAY || cooldownLeftMs() > 0) {
    sendStatus(429);
    return;
  }
  gate.write(OPEN_ANGLE);
  delay(OPEN_MS);
  gate.write(CLOSED_ANGLE);
  portionsToday++;
  lastFeed = millis();
  fedOnce = true;
  Serial.printf("Porsiyon dokuldu (%d/%d)\n", portionsToday, MAX_PER_DAY);
  sendStatus(200);
}

void setup() {
  Serial.begin(115200);
  gate.attach(SERVO_PIN);
  gate.write(CLOSED_ANGLE);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("WiFi baglaniyor");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nIP: ");
  Serial.println(WiFi.localIP());  // Bu adresi oyunda FeederUrl olarak yazin

  const char* headers[] = {"X-Feeder-Key"};
  server.collectHeaders(headers, 1);
  server.on("/status", HTTP_GET, []() { sendStatus(200); });
  server.on("/feed", HTTP_POST, handleFeed);
  server.begin();
}

void loop() {
  server.handleClient();
}
