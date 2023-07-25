#include <SPI.h>
#include <DHT.h>
#include <ezButton.h>
#include <Wire.h>
#include <BH1750.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <MemoryFree.h>
#include <MFRC522.h>

#define LED_LCD_PIN 12
#define LED_PIN 9
#define DHTPIN 7
#define DHTTYPE DHT11
#define RC522_SS_PIN 49
#define RC522_RST_PIN 48

DHT dht(DHTPIN, DHTTYPE);
ezButton button(8);
BH1750 lightMeter;
LiquidCrystal lcd(A0, A1, A5, A4, A3, A2);
MFRC522 rfid(RC522_SS_PIN, RC522_RST_PIN);

const byte ROWS = 4;  // number of rows on the keypad
const byte COLS = 4;  // number of columns on the keypad

// Define the keymap
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// Define the row and column pins
byte rowPins[ROWS] = { 22, 24, 26, 28 };
byte colPins[COLS] = { 30, 32, 34, 36 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//  ##################################################
//  ###########       CLASS               ############
//  ##################################################
// class Sensor {
// private:
//   DHT dht;
//   BH1750 lightMeter;
//   float temp, humid, lux;
// public:
//   SensorClass(int dhtPin, int dhtType): dht(dhtPin, dhtType), lightMeter() {}
//   void setup() {
//     dht.begin();
//     Wire.begin();
//     lightMeter.begin();
//   }

//   void loop() {
//     humidity = dht.readHumidity();
//     temperature = dht.readTemperature();
//     lightLevel = lightMeter.readLightLevel();
//   }
//   float getTemperature() {
//     return temperature;
//   }
//   float getHumidity() {
//     return humidity;
//   }
//   float getLux() {
//     return lightLevel;
//   }
// }

// SensorClass sensor(DHTPIN, DHTTYPE);

//  ##################################################
//  ###########       GLOBAL VARIABLES    ############
//  ##################################################
char buffer[256];                 // buffer to store UID data
unsigned long lastPrintTime = 0;  // time of last serial print

bool button_status = false;
bool led_status = false;

String enteredKeys = "";  // string variable to store the entered keys

volatile float temperature, humidity;



//  ##################################################
//  ###########           SETUP           ############
//  ##################################################

void setup() {
  Serial.begin(115200);
  SPI.begin();
  pinMode(MISO, OUTPUT);  // Set MISO pin to output mode
  pinMode(SS, INPUT);
  // button.setDebounceTime(100);
  SPCR |= _BV(SPE);  //Turn on SPI in Slave Mode
  // SPCR |= !(_BV(MSTR)); //Arduino is Slave
  SPCR &= ~(_BV(MSTR));
  // SPCR |= _BV(SPIE);                      //Turn on Interrupts
  // SPI.attachInterrupt();

  // dht.begin();
  // Wire.begin();
  // lightMeter.begin();

  lcd.begin(16, 2);
  lcd.print("Hello, World!");

  rfid.PCD_Init();  // initialize MFRC522 module

  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_LCD_PIN, OUTPUT);
  digitalWrite(LED_LCD_PIN, HIGH);
}

void loop() {
  button.loop();
  char key = keypad.getKey();
  float lux = lightMeter.readLightLevel();

  // #####################################
  buttonUsage();
  digitalWrite(LED_PIN, led_status);

  // #####################################
  keypadUsage(key);

  // #####################################
  // rfidUsage();

  // Read the temperature and humidity values
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // #####################################
  // print the buffer to the serial monitor every 5 seconds
  // unsigned long currentTime = millis();
  // if (currentTime - lastPrintTime >= 1000 && buffer[0] != '\0') {
  //   Serial.print("Card UID: ");
  //   Serial.println(buffer);
  //   buffer[0] = '\0'; // clear the buffer
  //   lastPrintTime = currentTime; // update the last print time
  // }

  // if (SPSR & _BV(SPIF)) {
  sendData(temperature, humidity, led_status, lux, enteredKeys, buffer);
  // }
  printLCD(temperature, humidity, led_status, lux, enteredKeys, buffer);
  Serial.print("Temperature is: ");
  Serial.print(temperature);
  Serial.print("  //  Humidity is: ");
  Serial.print(humidity);
  Serial.print("  //  Led status is: ");
  Serial.print(led_status);
  Serial.print("  //  Lux is: ");
  Serial.print(lux);
  Serial.print("  //  Entered keys are: ");
  Serial.print(enteredKeys);
  Serial.print("  //  Serial number is: ");
  Serial.println(buffer);

  Serial.print("freeMemory()=");
  Serial.println(freeMemory());

  // delay(1000);  // Add a delay to avoid flooding the SPI bus
}

void sendData(float temperature, float humidity, float led_status, float lux, String keys, String serNum) {
  // Send a message to the master
  String message = String(temperature, 2) + "|" + String(humidity, 2) + "|" + String(led_status, 2) + "|" + String(lux, 2) + "|" + keys + "|" + serNum;
  for (int i = 0; i < message.length(); i++) {
    if (i < message.length()) {
      SPDR = message[i];
    } else {
      SPDR = 0;
    }
    while (!(SPSR & _BV(SPIF)))
      ;  // Wait for SPI transfer to complete
  }
  Serial.println(message);
  delay(50);
}

void printLCD(float temp, float humid, float led_status, float lux, String keys, String serNum) {
  String message = String(temp, 2) + "|" + String(humid, 2) + "|" + serNum;
  String message_2 = String(led_status, 2) + "|" + String(lux, 2) + "|" + keys;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  lcd.setCursor(0, 1);
  lcd.print(message_2);
  delay(50);
}

void buttonUsage() {
  if (button.isPressed() && button_status == false) {
    button_status = true;
    led_status = HIGH;
    Serial.println("Button pressed!");
  } else if (button.isPressed() && button_status == true) {
    button_status = false;
    led_status = LOW;
    Serial.println("Button pressed!");
  } else if (button.isReleased()) {
    Serial.println("Button released!");
  }
}

void keypadUsage(char key) {
  // If "#" button is pressed, clear the stored variable
  if (key == '#') {
    enteredKeys.remove(0);
    Serial.println("Variable cleared.");
  }
  // If a key is pressed, add it to the entered keys string
  else if (key != '\0') {
    enteredKeys += key;
    Serial.print("ENTERED KEYS: ");
    Serial.println(enteredKeys);
  }
}

void rfidUsage() {
  if (rfid.PICC_IsNewCardPresent()) {
    // read the card UID
    if (rfid.PICC_ReadCardSerial()) {
      // store the UID in the buffer
      for (byte i = 0; i < rfid.uid.size; i++) {
        buffer[i] = rfid.uid.uidByte[i];
      }
      buffer[rfid.uid.size] = '\0';  // add null terminator

      rfid.PICC_HaltA();       // halt the card
      rfid.PCD_StopCrypto1();  // stop crypto1 communication
    }
  }
}

// ISR(SPI_STC_vect) {
//   // A master has requested data
//   // temperature = dht.readTemperature();
//   // humidity = dht.readHumidity();

//   // Send the values to the master if requested
// }
