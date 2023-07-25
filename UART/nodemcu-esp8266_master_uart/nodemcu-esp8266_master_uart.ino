#include <SPI.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

// Define the SoftwareSerial object for RX and TX pins
SoftwareSerial arduinoSerial(D5, D6); // RX, TX

void setup() {
  Serial.begin(115200);
  arduinoSerial.begin(115200);
  delay(1000);
}

void loop() {
  // Read data from the slave
  String message = "";
  message = arduinoSerial.readString();
  Serial.println(message);

  // Split the message into individual values
  // float floatValues[4];
  // String stringValues[2];
  String values[6];
  int count = 0;
  int startIndex = 0;
  int endIndex = message.indexOf("|");
  while (endIndex >= 0) {
    values[count] = message.substring(startIndex, endIndex);
    startIndex = endIndex + 1;
    endIndex = message.indexOf("|", startIndex);
    count++;
  }
  values[count] = message.substring(startIndex);

  // for (int i = 0; i < 4; i++) {
  //   floatValues[i] = values[i].toFloat();
  // }
  // for (int i = 0, j = 4; i < 2, j < 6; i++, j++) {
  //   stringValues[i] = values[j];
  // } 

  float temperature = values[0].toFloat();
  float humidity = values[1].toFloat();
  int led_status = values[2].toInt();
  float lux = values[3].toFloat();
  String keys = values[4];
  String serNum = values[5];
  
  // Print the values
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" // Humidity: ");
  Serial.print(humidity);
  Serial.print(" // Led: ");
  Serial.print(led_status);
  Serial.print(" // Lux: ");
  Serial.print(lux);
  Serial.print(" // Keys: ");
  Serial.print(keys);
  Serial.print(" // Serial: ");
  Serial.println(serNum);

  // int freeHeap = ESP.getFreeHeap();
  // Serial.print("Free heap: ");
  // Serial.println(freeHeap);
  // delay(1000);
}
