#include <WiFi.h>

#define trigPin 14
#define echoPin 12

const char* ssid = "UTP";
const char* password = "tecnologica";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 100) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi Conectada.");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP().toString());
    server.begin();
  } else {
    Serial.println("");
    Serial.println("Error al conectar a la red WiFi. Por favor verifica tus credenciales o la conexión.");
  }

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    long duration, distance;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = (duration / 2) * 0.0343;

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html; charset=UTF-8");
    client.println("");

    client.print("<html>");
    client.print("<head>");
    client.print("<meta charset='UTF-8'>");
    client.print("<style>");
    client.print("body { font-family: 'Arial', sans-serif; background-color: #f4f4f4; text-align: center; margin: 40px; }");
    client.print(".card { background-color: #fff; border-radius: 15px; padding: 20px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); margin: 20px auto; max-width: 400px; }");
    client.print(".icon { font-size: 3em; margin-bottom: 10px; }");
    client.print(".chart-container { display: flex; justify-content: center; align-items: center; margin: 20px auto; }"); // Utilizando flexbox para centrar
    client.print(".gauge { width: 600px; height: 200px; display: flex; justify-content: center; }"); // Ajuste en el tamaño
    client.print("</style>");
    client.print("<script type='text/javascript' src='https://www.gstatic.com/charts/loader.js'></script>");
    client.print("<script type='text/javascript'>");
    client.print("google.charts.load('current', {'packages':['gauge']});");
    client.print("google.charts.setOnLoadCallback(drawChart);");
    client.print("function drawChart() {");
    client.print("var data = google.visualization.arrayToDataTable([");
    client.print("['Label', 'Value'],");
    client.print("['Distancia', ");
    client.print(distance);
    client.print("]");
    client.print("]);");
    client.print("var options = { width: 600, height: 200, redFrom: 150, redTo: 200, yellowFrom:100, yellowTo: 150, min: 0, max: 200, minorTicks: 5 };");
    client.print("var chart = new google.visualization.Gauge(document.getElementById('chart_div'));");
    client.print("chart.draw(data, options);");
    client.print("}");
    client.print("</script>");
    client.print("</head>");
    client.print("<body>");
    client.print("<div class='card'>");
    client.print("<i class='icon'>📏</i>");
    client.print("<h1>Mediciones del Sensor</h1>");
    client.print("<p><strong>Duración:</strong> ");
    client.print(duration);
    client.print(" microsegundos</p>");
    client.print("<p><strong>Distancia:</strong> ");
    if (distance <= 200) {
      client.print(distance);
      client.print(" centímetros</p>");
    } else {
      client.print("<span class='out-of-range'>Fuera de rango</span></p>");
    }
    client.print("</div>");

    client.print("<div class='chart-container'>");
    client.print("<div id='chart_div' class='gauge'></div>");
    client.print("</div>");

    client.print("</body>");
    client.print("</html>");

    delay(100);
    Serial.println("Cliente desconectado");
    Serial.println("");
  }
}
