#include <ESP8266WiFi.h>

const char* ssid     = "test123";
const char* password = "12345678";
const char* apiKey   = "AKO1X94T35J98X2H";

float sicaklik = 0;
int   bpm = 0;

#define LED D4

void ledYak()    { digitalWrite(LED, LOW);  }
void ledSondur() { digitalWrite(LED, HIGH); }

void ledCak(int kac, int sure) {
  for (int i = 0; i < kac; i++) {
    ledYak();    delay(sure);
    ledSondur(); delay(sure);
  }
}

void gonder() {
  Serial.println(">>> ThingSpeak'e gonderiliyor...");
  ledYak(); delay(400); ledSondur(); delay(400);
  ledYak(); delay(400); ledSondur(); delay(400);

  WiFiClient client;
  if (!client.connect("api.thingspeak.com", 80)) {
    Serial.println("!!! ThingSpeak BAGLANAMADI");
    ledCak(5, 80);
    return;
  }

  String url = "/update?api_key=" + String(apiKey)
             + "&field1=" + String(sicaklik, 1)
             + "&field2=" + String(bpm);

  client.println("GET " + url + " HTTP/1.1");
  client.println("Host: api.thingspeak.com");
  client.println("Connection: close");
  client.println();
  delay(1000);
  client.stop();

  Serial.println(">>> Gonderim BASARILI");
  ledCak(2, 100);
}

void wifiBaglan() {
  Serial.println("------------------------------");
  Serial.println("WiFi'ye baglaniliyor...");
  Serial.print("Ag adi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int deneme = 0;
  while (WiFi.status() != WL_CONNECTED) {
    ledYak();    delay(150);
    ledSondur(); delay(150);
    deneme++;
    Serial.print(".");

    if (deneme % 20 == 0) {
      Serial.println();
      Serial.print("Hala baglanilamiyor... (");
      Serial.print(deneme / 2);
      Serial.println(" saniye gecti)");
    }

    // 30 saniye sonra şifre yanlış uyarısı ver
    if (deneme == 60) {
      Serial.println();
      Serial.println("!!! 30 saniye gecti baglanilamadi!");
      Serial.println("!!! Kontrol et:");
      Serial.println("!!! 1. WiFi adi dogru mu?");
      Serial.println("!!! 2. Sifre dogru mu?");
      Serial.println("!!! 3. Modem 2.4GHz mi? (5GHz calismiyor)");
    }
  }

  Serial.println();
  Serial.println(">>> WiFi BAGLANDI!");
  Serial.print(">>> IP Adresi: ");
  Serial.println(WiFi.localIP());
  Serial.print(">>> Sinyal Gucu: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.println("------------------------------");

  ledCak(3, 200);
  ledSondur();
}

void setup() {
  pinMode(LED, OUTPUT);
  ledSondur();
  Serial.begin(9600);
  delay(500);

  Serial.println("==============================");
  Serial.println("  IoT Hasta Takip - NodeMCU  ");
  Serial.println("==============================");

  wifiBaglan();
}

void loop() {
  // WiFi koptu mu kontrol et
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("!!! WiFi koptu, yeniden baglaniliyor...");
    wifiBaglan();
  }

  if (Serial.available()) {
    String satir = Serial.readStringUntil('\n');
    satir.trim();

    if (satir.startsWith("T:")) {
      int virgul = satir.indexOf(",B:");
      if (virgul > 0) {
        sicaklik = satir.substring(2, virgul).toFloat();
        bpm      = satir.substring(virgul + 3).toInt();

        Serial.print("Alinan veri → Sicaklik: ");
        Serial.print(sicaklik);
        Serial.print(" C  |  BPM: ");
        Serial.println(bpm);

        gonder();
      }
    }
  }
}