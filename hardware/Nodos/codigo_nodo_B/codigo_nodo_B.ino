#include <WiFi.h>
#include <PubSubClient.h>

// --- CONFIGURACIÓN DE IDENTIFICADORES DE RED ---
const char* ssid = "elpleto";
const char* password = "123456789";

// --- PARÁMETROS DE CONECTIVIDAD MQTT CON IP ACTUALIZADA ---
const char* mqtt_server = "172.24.230.143"; // <--- Tu IP actual de red
const int mqtt_port = 1883;
const char* mqtt_topic = "sistema/comandos";

// --- ASIGNACIÓN DE PINES DE HARDWARE ---
const int PIN_ROJO = 23;     
const int PIN_AMARILLO = 22; 
const int PIN_VERDE = 21;    

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String trama = "";
  for (int i = 0; i < length; i++) { trama += (char)payload[i]; }
  
  Serial.print("\n[MQTT RECEPTOR] -> Comando capturado: "); Serial.println(trama);

  if (trama == "LED_ROJO_ON")         digitalWrite(PIN_ROJO, HIGH);
  else if (trama == "LED_ROJO_OFF")   digitalWrite(PIN_ROJO, LOW);
  else if (trama == "LED_AMARILLO_ON")  digitalWrite(PIN_AMARILLO, HIGH);
  else if (trama == "LED_AMARILLO_OFF") digitalWrite(PIN_AMARILLO, LOW);
  else if (trama == "LED_VERDE_ON")     digitalWrite(PIN_VERDE, HIGH);
  else if (trama == "LED_VERDE_OFF")    digitalWrite(PIN_VERDE, LOW);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_ROJO, OUTPUT);
  pinMode(PIN_AMARILLO, OUTPUT);
  pinMode(PIN_VERDE, OUTPUT);
  
  digitalWrite(PIN_ROJO, LOW);
  digitalWrite(PIN_AMARILLO, LOW);
  digitalWrite(PIN_VERDE, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("[WIFI]: Sincronizando interfaz de red");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\n[WIFI OK]: Conexión inalámbrica establecida.");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    while (!client.connected()) {
      Serial.print("[MQTT]: Reintentando sincronización con el servidor...");
      if (client.connect("ESP32_Receptor_LEDs")) {
        client.subscribe(mqtt_topic);
        Serial.println("\n[MQTT OK]: Canal de escucha activo.");
      } else {
        Serial.print(" Código de error de socket: "); Serial.println(client.state());
        delay(3000);
      }
    }
  }
  client.loop();
}