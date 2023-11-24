#include <WiFi.h>
#include <HTTPClient.h>

#define trigPin 14  // Conecta Trig del sensor HC-SR04 a D32 en el ESP32
#define echoPin 12  // Conecta Echo del sensor HC-SR04 a D34 en el ESP32

const char *ssid = "UTP";
const char *password = "tecnologica";

String server = "https://maker.ifttt.com";
String eventName = "data_periodo";
String IFTTT_Key = "E2Eunp_ZbmRcEoU3CCQNk";

int distancia = 0;
unsigned long duracion = 0;

void setup() {
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi Conectada.");
  } else {
    Serial.println("");
    Serial.println("Error al conectar a la red WiFi. Por favor verifica tus credenciales o la conexión.");
  }

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void sendDataToSheet() {
  String url = "https://maker.ifttt.com/trigger/" + eventName + "/with/key/" + IFTTT_Key + "?value1=" + String(duracion / 1000) + "&value2=" + String(distancia); // Convertir duracion a segundos
  Serial.println(url);

  // Comienza a enviar datos a IFTTT
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  http.begin(url); // HTTP

  Serial.print("[HTTP] GET...\n");
  // Iniciar la conexión y enviar la cabecera HTTP
  int httpCode = http.GET();
  // httpCode será negativo en caso de error
  if (httpCode > 0) {
    // La cabecera HTTP ha sido enviada y se ha manejado la respuesta del servidor
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    // Archivo encontrado en el servidor
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void loop() {
  const int maxSets = 50;  // Número máximo de conjuntos a enviar
  int setsSent = 0;        // Contador de conjuntos enviados
  unsigned long startTime = millis();  // Tiempo de inicio

  while (setsSent < maxSets) {
    long duration, distance;

    // Generar un pulso ultrasónico
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Medir el tiempo que tarda en regresar el eco
    duration = pulseIn(echoPin, HIGH);

    // Calcular la distancia en centímetros
    distance = duration * 0.0343 / 2;

    // Mostrar la distancia en el monitor serial
    Serial.print("pasos: ");
    Serial.print(distance);
    Serial.println(" cm");


    // Enviar datos a IFTTT
    sendDataToSheet();

    // Actualizar valores
    duracion = millis() - startTime;
    distancia = distance;

    delay(1000);  // Esperar un segundo antes de la siguiente medición
    setsSent++;
  }

  delay(1200000);
}
