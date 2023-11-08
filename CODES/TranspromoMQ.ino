#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <MQ135.h>

#define anInput     A0
#define digTrigger   2
#define co2Zero     55
#define OLED_RESET   4

unsigned int value;

#define ledVerte 8
#define ledJaune 9
#define ledRouge 10

#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

Adafruit_SSD1306 display(OLED_RESET);

int displayState = 0;
unsigned long lastDisplayChange = 0;

void setup() {
  pinMode(anInput, INPUT);
  pinMode(digTrigger, INPUT);
  pinMode(ledVerte, OUTPUT);
  pinMode(ledJaune, OUTPUT);
  pinMode(ledRouge, OUTPUT);
  pinMode(DHTPIN, INPUT);
  pinMode(6, OUTPUT);
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();
  dht.begin();
}

void loop() {
  int co2now[10];
  int co2raw = 0;
  int co2comp = 0;
  int co2ppm = 0;
  int zzz = 0;

  if (millis() - lastDisplayChange > 3000) {
    displayState = (displayState + 1) % 4;
    lastDisplayChange = millis();
  }

  display.clearDisplay();

  if (displayState == 0) {
    zzz = 0;

    for (int x = 0; x < 10; x++) {
      co2now[x] = analogRead(anInput);
      delay(200);
    }

    for (int x = 0; x < 10; x++) {
      zzz = zzz + co2now[x];
    }

    co2raw = zzz / 10;
    co2comp = co2raw - co2Zero;
    co2ppm = map(co2comp, 0, 1023, 400, 5000);

    if (co2ppm < 250) {
      digitalWrite(ledVerte, HIGH);
      digitalWrite(ledJaune, LOW);
      digitalWrite(ledRouge, LOW);
    } else if (co2ppm < 400) {
      digitalWrite(ledVerte, LOW);
      digitalWrite(ledJaune, HIGH);
      digitalWrite(ledRouge, LOW);
    } else if (co2ppm < 700) {
      digitalWrite(ledVerte, LOW);
      digitalWrite(ledJaune, LOW);
      digitalWrite(ledRouge, HIGH);
      tone(6, 10, 20);
      delay(500);
      tone(6, 10, 20);
      delay(500);
      noTone(6);
      delay(500);
    } else {
      digitalWrite(ledVerte, HIGH);
      digitalWrite(ledJaune, HIGH);
      digitalWrite(ledRouge, HIGH);
      tone(6, 600);
      delay(500);
      tone(6, 900);
      delay(500);
      noTone(6);
      delay(500);
    }

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    Serial.print(co2ppm);
    display.print(" CO2 Level:");
    display.print(co2ppm);
    display.println(" PPM");
  } else if (displayState == 1) {
    float temperature = dht.readTemperature();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    Serial.print(",");
    Serial.print(temperature);
    display.print("Temperature:");
    display.print(temperature);
    display.println(" degrees");
  } else if (displayState == 2) {
    float humidity = dht.readHumidity();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    Serial.print(",");
    Serial.println(humidity);
    display.print("Humidity:");
    display.print(humidity);
    display.print("% ");
  } 

  display.display();
  delay(3000);
}