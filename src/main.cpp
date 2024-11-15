#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define DHTPIN 21
#define DHTTYPE DHT22

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_SDA 21
#define OLED_SCL 22

const char *ssid = "ESP32-WiFi";
const char *password = "12345678";

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WebServer server(80);

float temperature = 0.0;
float humidity = 0.0;

void readDHT() {
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Ошибка чтения данных с DHT22");
    }
}

void displayData() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Temperature: ");
    display.print(temperature);
    display.println(" C");
    display.print("Humidity: ");
    display.print(humidity);
    display.println(" %");
    display.display();
}

void handleRoot() {
    String html = "<!DOCTYPE html><html><head><title>ESP32 Data</title></head><body><h1>Информация о температуре и влажности</h1>";
    html += "<p>Температура: " + String(temperature) + " C</p>";
    html += "<p>Влажность: " + String(humidity) + " %</p>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void setup() {
    Serial.begin(115200);
    dht.begin();
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    Serial.println("Запуск точки доступа...");
    if (WiFi.softAP(ssid, password)) {
        Serial.print("Точка доступа успешно создана. IP: ");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("Ошибка создания точки доступа");
    }

    server.on("/", handleRoot);
    server.begin();

    display.clearDisplay();
}

void loop() {
    readDHT();
    displayData();

    server.handleClient();

    Serial.print("Температура: ");
    Serial.print(temperature);
    Serial.print(" C, Влажность: ");
    Serial.print(humidity);
    Serial.println(" %");

    delay(2000);
}