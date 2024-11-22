#include <WiFi.h>
#include <WebServer.h>
///////////////////////////////////////////////////////////////////////////////
//  Define pins and variables

const int ampPin = 34;  //  ADC pin for the amplifier (GPIO 34)

// Configura la red WiFi
const char* ssid = "Wokwi-GUEST";     //  Replace with your WiFi network name
const char* password = ""; // Replace with the password of your WiFi network

// Create a server on port 80
WebServer server(80);
///////////////////////////////////////////////////////////////////////////////
const int esc = 36;   //  Pin to recognize the measurement scale

///////////////////////////////////////////////////////////////////////////////

void setup() {
  ///////////////////////////////////////////////////////////////////////////////
  // Configure amplifier pins as pull-up resistor inputs

  pinMode(esc, INPUT_PULLUP);

  ///////////////////////////////////////////////////////////////////////////////

  //  Start serial communication to monitor
  Serial.begin(115200);

  // Configure the pin as analog input
  pinMode(ampPin, INPUT);

  // Connect to the WiFi network
  Serial.println("Conectando a la red WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando...");
  }

  // Confirm connection
  Serial.println("Conectado a la red WiFi.");
  Serial.println(WiFi.localIP()); // Display the IP address of the ESP32

  // Configure the web page
  server.on("/", handleRoot);  // Main site
  server.begin();              // Start the web server
  Serial.println("Servidor web iniciado.");
}

void loop() {
  ///////////////////////////////////////////////////////////////////////////////

  server.handleClient();  //  Listen to web requests
}
///////////////////////////////////////////////////////////////////////////////

// Main function for the site
void handleRoot() {
  int state = digitalRead(esc); // Read the digital input
  int ampValue = analogRead(ampPin);  // Read the analog value of the amplifier
  float current = ampValue * (3.3 / 4095.0);  // Calculate the current
  String scale = " uA";

  // Change of scale
  switch (state) {
    case 0:  // signalValue = 0
      scale = " uA";
      break;

    case 1:  // signalValue = 1
      scale = " mA";
      break;

    default:
      scale = " uA";
      break;
  }

  //  HTML to display the value
  String htmlPage = "<html><body><h1>Lectura del Potenciómetro</h1>";
  htmlPage += "<p>Valor analógico: " + String(ampValue) + "</p>";
  htmlPage += "<p>Current: " + String(current) + scale + " V</p>";
  htmlPage += "</body></html>";

  // Send the web page to the customer
  server.send(200, "text/html", htmlPage);
}

