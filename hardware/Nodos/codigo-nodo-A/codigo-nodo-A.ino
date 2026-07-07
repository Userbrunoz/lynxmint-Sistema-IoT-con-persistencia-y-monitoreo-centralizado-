#include <WiFi.h>
#include <PubSubClient.h>

// --- CONFIGURACIÓN DE IDENTIFICADORES DE RED ---
const char* ssid = "elpleto";
const char* password = "123456789";

// --- PARÁMETROS DE CONECTIVIDAD MQTT CON IP ACTUALIZADA ---
const char* mqtt_server = "172.24.230.143"; // <--- Tu IP actual de red
const int mqtt_port = 1883;
const char* mqtt_topic = "sistema/comandos";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String trama = "";
  for (int i = 0; i < length; i++) { trama += (char)payload[i]; }
  
  Serial.println("\n==================================================");
  Serial.print("[NODO A - SNIFFER]: Trama de datos interceptada: "); Serial.println(trama);
  Serial.println("==================================================");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("[WIFI]: Inicializando Sonda de Diagnóstico");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\n[WIFI OK]: Nodo A enlazado a la red de control.");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    while (!client.connected()) {
      Serial.print("[MQTT]: Buscando pasarela central de comunicaciones...");
      if (client.connect("ESP32_Gateway_Central")) {
        client.subscribe(mqtt_topic);
        Serial.println("\n[MQTT OK]: Enlace de diagnóstico establecido con éxito.");
      } else {
        Serial.print(" Falló con estado: "); Serial.println(client.state());
        delay(3000);
      }
    }
  }
  client.loop();
}