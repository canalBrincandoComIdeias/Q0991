/*  
*  AUTOR:    BrincandoComIdeias
*  APRENDA:  https://cursodearduino.net/
*  SKETCH:   Sensor Fim de Curso
*  DATA:     07/03/23
*/

#include <ESP8266WiFi.h>
#include <Espalexa.h>

// DEFINIÇÕES DE PINOS
#define pinEspAbrir 2
#define pinEspFechar 0

// DEFINIÇÕES
#define DEBUG
#define tempoPulso 2000  // TEMPO DO PULSO PARA O ARDUINO IDENTIFICAR A AÇÃO

// VARIÁVEIS GLOBAIS
const char* ssid = "*****";
const char* password = "*****";

boolean wifiConnected = false;

Espalexa espalexa;

// DECLARAÇÃO DE FUNÇÕES
boolean connectWifi();

void firstLightChanged(uint8_t brightness);

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif

  pinMode(pinEspAbrir, OUTPUT);
  pinMode(pinEspFechar, OUTPUT);

  digitalWrite(pinEspAbrir, LOW);
  digitalWrite(pinEspFechar, LOW);

  wifiConnected = connectWifi();

  if (wifiConnected) {
    espalexa.addDevice("Persiana", firstLightChanged);
    espalexa.begin();
  } else {
    digitalWrite(pinEspAbrir, HIGH);
    digitalWrite(pinEspFechar, HIGH);

#ifdef DEBUG
    Serial.println("DEU RUIM");
#endif
  }
}

void loop() {
  espalexa.loop();
  delay(1);
}

void firstLightChanged(uint8_t brightness) {

  if (brightness) {

#ifdef DEBUG
    Serial.println("SUBINDO!");
#endif

    digitalWrite(pinEspAbrir, HIGH);
    digitalWrite(pinEspFechar, LOW);
  } else {

#ifdef DEBUG
    Serial.println("DESCENDO!");
#endif

    digitalWrite(pinEspAbrir, LOW);
    digitalWrite(pinEspFechar, HIGH);
  }

  delay(tempoPulso);

  digitalWrite(pinEspAbrir, LOW);
  digitalWrite(pinEspFechar, LOW);

#ifdef DEBUG
  Serial.println("Desligando portas!");
#endif
}

boolean connectWifi() {
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

#ifdef DEBUG
  Serial.println("");
  Serial.println("Connecting to WiFi");
  Serial.print("Connecting...");
#endif

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);

#ifdef DEBUG
    Serial.print(".");
#endif

    if (i > 20) {
      state = false;
      break;
    }
    i++;
  }

#ifdef DEBUG
  Serial.println("");
  if (state) {
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Connection failed.");
  }
#endif

  return state;
}
