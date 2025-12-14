#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ---- WiFi ----
const char* ssid     = "SSID";
const char* password = "password";

// ---- MQTT ----
const char* mqtt_server = "test.mosquitto.org";
const int   mqtt_port   = 1883;
const char* mqtt_topic  = "clase/iot/sensor1";

WiFiClient espClient;
PubSubClient client(espClient);

const int sensor   = A0;
const int ledPin   = 5;      

// ----------------- CONEXIÓN WIFI -----------------
void setup_wifi() {
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ----------------- RECONEXIÓN MQTT -----------------
void reconnect() {
  // Bucle hasta conseguir conexión
  while (!client.connected()) {
    Serial.print("Conectando con broker MQTT... ");
    String clientId = "ESP8266-iot-demo-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("conectado");
      // suscribirse a un tópico para recibir órdenes
      client.subscribe("clase/iot/led");
    } else {
      Serial.print("fallo, rc=");
      Serial.print(client.state());
      Serial.println(" intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

// ----------------- CALLBACK MQTT (mensajes entrantes) -----------------
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");

  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println(msg);

  // Ejemplo: controlar LED con "ON"/"OFF"
  if (String(topic) == "clase/iot/led") {
    if (msg == "ON")  digitalWrite(ledPin, HIGH);
    if (msg == "OFF") digitalWrite(ledPin, LOW);
  }
}


void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.begin(115200);
  delay(1000);

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// ----------------- LOOP PRINCIPAL -----------------
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();   // gestiona MQTT

  // Leer un "sensor" (aquí A0) y publicarlo
  int value = analogRead(sensor);
  char msg[16];
  snprintf(msg, sizeof(msg), "%d", value);

  Serial.print("Publicando: ");
  Serial.println(msg);
  client.publish(mqtt_topic, msg);

  delay(2000);   // cada 2 segundos
}
