#include <WiFi.h>
#include <HTTPClient.h>

#define PIR_PIN       13
#define LDR_PIN       34
#define LED_PIN       26
#define LDR_THRESHOLD 500

const char* ssid     = "Wokwi-GUEST";
const char* password = "";
const char* apiKey   = "QPKUOOPB0GTT26LQ";
const int channelID  = 3409155;
const char* server   = "https://api.thingspeak.com/update";

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");
  Serial.println("System Initialized");
}

void loop() {
  int motionDetected = digitalRead(PIR_PIN);
  int ldrValue       = analogRead(LDR_PIN);

  Serial.print("Motion: "); Serial.print(motionDetected);
  Serial.print(" | LDR: "); Serial.println(ldrValue);

  int ledStatus = 0;

  if (motionDetected == HIGH && ldrValue < LDR_THRESHOLD) {
    digitalWrite(LED_PIN, HIGH);
    ledStatus = 1;
    Serial.println("ALERT: Motion in dark — LED ON");
  } else if (motionDetected == HIGH && ldrValue >= LDR_THRESHOLD) {
    digitalWrite(LED_PIN, LOW);
    ledStatus = 0;
    Serial.println("Motion in bright light — No alert");
  } else {
    digitalWrite(LED_PIN, LOW);
    ledStatus = 0;
    Serial.println("No motion detected — LED OFF");
  }

  sendToThingSpeak(motionDetected, ldrValue, ledStatus);
  delay(15000);
}

void sendToThingSpeak(int pirStatus, int ldrVal, int ledStatus) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) +
                 "?api_key=" + apiKey +
                 "&field1=" + pirStatus +
                 "&field2=" + ldrVal +
                 "&field3=" + ledStatus;
    http.begin(url);
    int httpCode = http.GET();
    Serial.print("ThingSpeak Response: ");
    Serial.println(httpCode);
    http.end();
  }
}
