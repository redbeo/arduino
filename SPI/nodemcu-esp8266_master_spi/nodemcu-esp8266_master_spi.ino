#include <SPI.h>
#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(115200);
  SPI.begin();
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);  // set SS pin high to start with

  SPI.setClockDivider(SPI_CLOCK_DIV128);
}

void loop() {
  // Request data from the slave
  // digitalWrite(SS, LOW);
  // SPI.transfer(0x00);
  // digitalWrite(SS, HIGH);

  // delay(50); // Add a delay to avoid flooding the SPI bus

  // Read data from the slave
  String message = "";
  digitalWrite(SS, LOW);
  while (true) {
    char c = SPI.transfer(0x00);
    if (c == '\0') {
      break;
    }
    message += c;
  }
  digitalWrite(SS, HIGH);
  Serial.println(message);

  // Split the message into individual values
  float floatValues[4];
  String stringValues[2];
  String values[6];
  int count = 0;

  // int startIndex = 0;
  // int endIndex = message.indexOf("|");

  // for (int i = 0; i < 4; i++) {
  //   String valueStr = message.substring(startIndex, endIndex);
  //   values[i] = valueStr.toFloat();
  //   startIndex = endIndex + 1;
  //   endIndex = message.indexOf("|", startIndex);
  // }

  // String stringValue = message.substring(startIndex);

  int startIndex = 0;
  int endIndex = message.indexOf("|");
  while (endIndex >= 0) {
    values[count] = message.substring(startIndex, endIndex);
    startIndex = endIndex + 1;
    endIndex = message.indexOf("|", startIndex);
    count++;
  }
  values[count] = message.substring(startIndex);

  for (int i = 0; i < 4; i++) {
    floatValues[i] = values[i].toFloat();
  }
  for (int i = 0, j = 4; i < 2, j < 6; i++, j++) {
    stringValues[i] = values[j];
  } 

  // char buffer[20]; // character array to hold the converted value
  // dtostrf(values[4], 0, 0, buffer); // convert the float to a character array
  // sprintf(buffer, "", values[4]);

  float temperature = floatValues[0];
  float humidity = floatValues[1];
  float led_status = floatValues[2];
  float lux = floatValues[3];
  String keys = stringValues[1];
  String serNum = stringValues[2];
  
  // Print the values
  Serial.print("Temperature is: ");
  Serial.print(temperature);
  Serial.print("  //  Humidity is: ");
  Serial.print(humidity);
  Serial.print("  //  Led status is: ");
  Serial.print(led_status);
  Serial.print("  //  Lux is: ");
  Serial.print(lux);
  Serial.print("  //  Entered keys are: ");
  Serial.print(keys);
  Serial.print("  //  Serial number is: ");
  Serial.println(serNum);

  // int freeHeap = ESP.getFreeHeap();
  // Serial.print("Free heap: ");
  // Serial.println(freeHeap);
  delay(1000);
}
