#include <ZMPT101B.h> 
#include "EmonLib.h" 
#include <WiFi.h> 
#include <HTTPClient.h> 
#include <EMailSender.h> 
 
// Pin Definitions and Constants 
#define LED 2 
#define Sensor 32 
#define SENSITIVITY 333.0f 
#define flowSensorPin 34 
 
ZMPT101B voltageSensor(33, 50.0); 
EnergyMonitor emon2; 
 
// Variables for Current Sensor 
float current2; 
 
// Variables for Voltage Sensor 
float voltage; 

 
// Variables for Flow Sensor volatile 
int pulseCount = 0; 
float calibrationFactorFlow = 16.039; 
unsigned long oldTime = 0; 
unsigned long totalMilliLitres = 0; 
 
// Constants for Flow-Pressure Relationship 
const float flowToPressureFactor = 0.07;  
 
// Thresholds 
const float pressureThreshold = 21.7;     // Pressure in psi 
const float flowRateThreshold = 100.0;    // Flow rate in mL/s 
const float currentThreshold = 4.5;       // Current in A 
const float voltageThreshold = 220.0;     // Voltage in V 
 
// WiFi Credentials 
const char* ssid = "Fast Faculty"; 
const char* password = "Faculty@Fast987"; 
 
// ThingSpeak Server Details 
const char* server = "http://api.thingspeak.com/update"; 
const char* apiKey = "YLI308G4SMLB3IOA"; 
 
// Email Configuration 
EMailSender emailSend("Shiza Sahoo", "shiza123."); 
const char* recipientEmail = "shizasahu@gmail.com"; 
 
 
// ISR for Flow Sensor 
void IRAM_ATTR pulseCounter() {   
  pulseCount++; 
} 
 
// Vibration Sensor Function 
void handleVibrationSensor() {   
  bool value = digitalRead(Sensor);   
  digitalWrite(LED, value ? HIGH : LOW); 
  Serial.print("Vibration value: "); 
  Serial.println(value); 
} 
 
// Current Sensor Function 
void handleCurrentSensor() { 
  current2 = emon2.calcIrms(1480) / 100; // RMS current calculation 
  Serial.print("Calibrated Current: "); 
  Serial.print(current2); 
  Serial.println(" A"); 
} 
 
// Voltage Sensor Function 
void handleVoltageSensor() {   
  voltage = voltageSensor.getRmsVoltage();   
  if (voltage < 100) {     
    voltage = 0; 
} 
Serial.print("Voltage Actual: "); 

  Serial.println(voltage); 
} 
 
// Flow Sensor Function 
void handleFlowSensor() {   
  if ((millis() - oldTime) > 1000) { 
    detachInterrupt(digitalPinToInterrupt(flowSensorPin)); 
    float flowRatePerSecond = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactorFlow; 
    unsigned long flowMilliLitresPerSecond = flowRatePerSecond * 1000 / 60;     
    totalMilliLitres += flowMilliLitresPerSecond;     
    pulseCount = 0;     
    oldTime = millis(); 
    attachInterrupt(digitalPinToInterrupt(flowSensorPin), pulseCounter, FALLING); 
 
    // Calculate pressure from flow rate 
    float pressure = flowMilliLitresPerSecond * flowToPressureFactor; // Pressure in psi 
 
    // Display flow rate and calculated pressure 
    Serial.print("Flow Rate (mL/s): "); 
    Serial.println(flowMilliLitresPerSecond); 
    Serial.print("Calculated Pressure: "); 
    Serial.print(pressure); 
    Serial.println(" psi"); 
 
    // Check for threshold violations and send notifications     
    String statusMessage = "";     
    if (pressure < pressureThreshold) { 
    statusMessage += "Low Pressure "; 
  } 
  if (flowMilliLitresPerSecond < flowRateThreshold) {     statusMessage += "Low Flow Rate "; 
    } 
    if (current2 < currentThreshold) {       
      statusMessage += "Low Current "; 
    } 
    if (voltage < voltageThreshold) {       
      statusMessage += "Low Voltage "; 
    } 
 
    // Send Email Alert if any threshold is violated    
    if (!statusMessage.isEmpty()) {       
      EMailSender::EMailMessage message;       
      message.subject = "Sensor Alert"; 
      message.message = "The following issues were detected:<br>" + statusMessage; 
 
      EMailSender::Response resp = emailSend.send(recipientEmail, message); 
      Serial.println("Email sending status: "); 
      Serial.println(resp.status); 
      Serial.println(resp.code); 
      Serial.println(resp.desc); 
    } 
 
    // Send data to ThingSpeak   
    if (WiFi.status() == WL_CONNECTED) { 
    HTTPClient http; 
 
    String url = String(server) + "?api_key=" + apiKey + 
                  "&field1=" + flowMilliLitresPerSecond + 
                  "&field2=" + pressure +                  
                  "&field3=" + voltage + 
                  "&field4=" + current2 + 
                  "&field5=" + (digitalRead(Sensor) ? 1 : 0) + 
                  "&status=" + statusMessage;       
                   Serial.println("Constructed URL: " + url);       
                   http.begin(url);       
                   int httpCode = http.GET(); 
      Serial.printf("HTTP Response Code: %d\n", httpCode);       
      if (httpCode > 0) { 
        Serial.println("Response: " + http.getString()); 
      } else { 
        Serial.printf("HTTP Error: %s\n", http.errorToString(httpCode).c_str()); 
      } 
      http.end(); 
    } else { 
      Serial.println("WiFi not connected."); 
    } 
  } 
} 
 
// Setup Function void setup() { 
  Serial.begin(9600); 
// Connect to WiFi 
WiFi.begin(ssid, password); while (WiFi.status() != WL_CONNECTED) {   delay(1000); 
  Serial.println("Connecting to WiFi..."); 
  } 
  Serial.println("Connected to WiFi."); 
 
  // Vibration Sensor Setup   
  pinMode(Sensor, INPUT);   
  pinMode(LED, OUTPUT); 
 
  // Current Sensor Setup 
  emon2.current(35, 111.1); // Pin 13 with calibration factor 111.1 
 
  // Voltage Sensor Setup 
  voltageSensor.setSensitivity(SENSITIVITY); 
 
  // Flow Sensor Setup 
  pinMode(flowSensorPin, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(flowSensorPin), pulseCounter, FALLING); 
 
  Serial.println("/** Sensor Monitoring System **/"); 
} 
 
// Loop Function void loop() { 
handleVibrationSensor(); 
handleCurrentSensor();   
handleVoltageSensor();   
handleFlowSensor(); 
 
delay(3000); 
} 
 
 
 
 
