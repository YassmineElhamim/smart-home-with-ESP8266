// Blynk configuration for project identification and authentication
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "template_id "
#define BLYNK_TEMPLATE_NAME "template_name"
#define BLYNK_AUTH_TOKEN "auth_token"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// WiFi Credentials
char ssid[] = "wifi name";
char pass[] = "wifi password";    

// Pin Definitions
Servo myServo;                       
int servoPin = D4;                  
const int relayPin = D8; 

// Pins for RGB LED
const int redPin = D5;   
const int greenPin = D6; 
const int bluePin = D7;  

// DHT Sensor Setup
#define DHTPIN 0        // D3 (GPIO0) on ESP8266
#define DHTTYPE DHT11   // DHT11 sensor type
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); 

bool manualFanControl = false; // Flag for manual/automatic fan control

void setup() {
  Serial.begin(115200);
  
  // Initialize WiFi and Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  // Setup Servo
  myServo.attach(servoPin);         
  
  // Setup Relay for Fan
  pinMode(relayPin, OUTPUT); 
  digitalWrite(relayPin, LOW); 

  // Setup RGB LED
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  // Initialize RGB LED to off
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);
  
  // Initialize DHT Sensor and LCD
  dht.begin();           
  lcd.init();            
  lcd.backlight();       
  lcd.setCursor(0, 0);   
  lcd.print("  Welcome Home");
}

void loop() {
  Blynk.run();                      
  readAndDisplaySensorData();       
}

void readAndDisplaySensorData() {
   
  // Read humidity and temperature
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any reads failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    lcd.setCursor(0, 1);
    lcd.print("Error!       ");
    return;
  }

  // Display readings on Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("% Temperature: ");
  Serial.print(temperature);
  Serial.println("C");

  // Display readings on the LCD
  lcd.setCursor(0, 1);
  lcd.print("H: ");
  lcd.print((int)humidity);
  lcd.print("% T: ");
  lcd.print((int)temperature);
  lcd.print("C");

  // Automatic fan control if manualFanControl is false
  if (!manualFanControl) {
    if (temperature > 25.0) {
      digitalWrite(relayPin, HIGH); // Turn fan on
      Serial.println("Fan ON (Automatic)");
    } else {
      digitalWrite(relayPin, LOW); // Turn fan off
      Serial.println("Fan OFF (Automatic)");
    }
  }
}

// Blynk control for Servo +
BLYNK_WRITE(V1) {
  int buttonState = param.asInt();
  if(buttonState == 1){
    myServo.write(180);
  } else {
    myServo.write(90);
  }
}

// Blynk control for Servo -
BLYNK_WRITE(V3) {
  int buttonState = param.asInt();
  if(buttonState == 1){
    myServo.write(0);
  } else {
    myServo.write(90);
  }
}

// Blynk control for Fan
BLYNK_WRITE(V0) { 
  int pinValue = param.asInt(); 
  manualFanControl = pinValue; // Update manual control state
  if (manualFanControl) {
    digitalWrite(relayPin, HIGH); // Turn fan on manually
    Serial.println("Fan ON (Manual)");
  } else {
    digitalWrite(relayPin, LOW);  // Turn fan off manually
    Serial.println("Fan OFF (Manual)");
  }
}

// Blynk control for RGB LED
BLYNK_WRITE(V6) { 
  int redValue = param.asInt(); 
  analogWrite(redPin, redValue);
}

BLYNK_WRITE(V2) { 
  int greenValue = param.asInt(); 
  analogWrite(greenPin, greenValue);
}

BLYNK_WRITE(V4) { 
  int blueValue = param.asInt(); 
  analogWrite(bluePin, blueValue);
}

