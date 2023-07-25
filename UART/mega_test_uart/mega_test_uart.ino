#include <SPI.h>
#include <DHT.h>
#include <ezButton.h>
#include <Wire.h>
#include <BH1750.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <MemoryFree.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

#define TX_PIN 14
#define RX_PIN 15

#define LED_LCD_PIN 12

#define LED_PIN 9
#define BUTTON_PIN 8

#define DHTPIN 6
#define DHTTYPE DHT11

#define RC522_SS_PIN 53
#define RC522_RST_PIN 49

#define PIR_PIN 7
#define PIR_LED_PIN 13



DHT dht(DHTPIN, DHTTYPE);
ezButton button(BUTTON_PIN);
BH1750 lightMeter;
LiquidCrystal lcd(A0, A1, A5, A4, A3, A2);
MFRC522 mfrc522(RC522_SS_PIN, RC522_RST_PIN);

SoftwareSerial mySerial(RX_PIN, TX_PIN); // RX, TX

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
//  ###########       GLOBAL VARIABLES    ############
//  ##################################################

char buffer[256];                 // buffer to store UID data
unsigned long lastPrintTime = 0;  // time of last serial print

bool button_status = false;
bool led_status = false;

String enteredKeys = "";  // string variable to store the entered keys
String cardUID = "";

String noti, noti_2;

volatile float temperature, humidity;


//  ##################################################
//  ###########           SETUP           ############
//  ##################################################

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
  SPI.begin();
  // pinMode(MISO, OUTPUT);  // Set MISO pin to output mode
  // pinMode(SS, INPUT);
  // button.setDebounceTime(100);
  // SPCR |= _BV(SPE);  //Turn on SPI in Slave Mode
  // // SPCR |= !(_BV(MSTR)); //Arduino is Slave
  // SPCR &= ~(_BV(MSTR));
  // // SPCR |= _BV(SPIE);                      //Turn on Interrupts
  // // SPI.attachInterrupt();

  dht.begin();
  Wire.begin();
  lightMeter.begin();

  lcd.begin(16, 2);
  lcd.print("Hello, World!");

  mfrc522.PCD_Init();

  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_LCD_PIN, OUTPUT);
  digitalWrite(LED_LCD_PIN, HIGH);

  pinMode(PIR_PIN, INPUT);
  pinMode(PIR_LED_PIN, OUTPUT);
}

void loop() {
  button.loop();
  char key = keypad.getKey();
  float lux = lightMeter.readLightLevel();
  int PIR_STATE = digitalRead(PIR_PIN);

  // #####################################
  buttonUsage();
  digitalWrite(LED_PIN, led_status);

  // #####################################
  keypadUsage(key);

  // #####################################
  rfidUsage();

  // #####################################
  pirUsage(PIR_STATE);

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

  // if (mySerial.available()) {
  sendData(temperature, humidity, led_status, lux, enteredKeys, cardUID);
  // }

  // printLCD(temperature, humidity, led_status, lux, enteredKeys, cardUID);
  // loadLCD();
  if(!enteredKeys && !cardUID){
    noti = "";
    noti_2 = "";
  }
  printLCD(noti, noti_2);


  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" // Humidity: ");
  Serial.print(humidity);
  Serial.print(" // Led: ");
  Serial.print(led_status);
  Serial.print(" // Lux: ");
  Serial.print(lux);
  Serial.print(" // Keys: ");
  Serial.print(enteredKeys);
  Serial.print(" // Serial: ");
  Serial.println(cardUID);

  // Serial.print("freeMemory()=");
  // Serial.println(freeMemory());

  // delay(1000);  // Add a delay to avoid flooding the SPI bus
}


void sendData(float temperature, float humidity, int led_status, float lux, String keys, String serNum) {
  // Send a message to the master
  String message = String(temperature, 2) + "|" + String(humidity, 2) + "|" + String(led_status) + "|" + String(lux, 2) + "|" + keys + "|" + serNum;
  mySerial.print(message);
  Serial.println(message);
  delay(1000);
}


// void printLCD(float temp, float humid, float led_status, float lux, String keys, String serNum) {
//   String message = String(temp, 2) + "|" + String(humid, 2) + "|" + serNum;
//   String message_2 = String(led_status, 2) + "|" + String(lux, 2) + "|" + keys;
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print(message);
//   lcd.setCursor(0, 1);
//   lcd.print(message_2);
//   delay(50);
// }

// void loadLCD(){
//   if((!enteredKeys) && (!cardUID)){
//     noti = "";
//   }
//   else if(enteredKeys){
//     noti = "Entered Keys: " + String(enteredKeys);
//   }
//   else if(cardUID){
//     noti = "Card UID: " + String(cardUID);
//   }
// }

void printLCD(String message, String message_2) {
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


void pirUsage(int state) {
  if (state == HIGH) {
    digitalWrite(PIR_LED_PIN, HIGH);
    Serial.println("Motion detected!");
  } else {
    digitalWrite(PIR_LED_PIN, LOW);
    // Serial.println("Motion detected!");
  }
}


void keypadUsage(char key) {
  // If "#" button is pressed, clear the stored variable
  if (key == '#') {
    enteredKeys.remove(0);
    Serial.println("Variable cleared.");
    noti = "Entered Keys:";
    noti_2 = enteredKeys;
  }
  // If a key is pressed, add it to the entered keys string
  else if (key != '\0') {
    enteredKeys += key;
    Serial.print("ENTERED KEYS: ");
    Serial.println(enteredKeys);
    noti = "Entered Keys:";
    noti_2 = enteredKeys;
  }
}


void rfidUsage() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    cardUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      cardUID += mfrc522.uid.uidByte[i] < 0x10 ? "0" : "";
      cardUID += String(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.print("Card UID: ");
    Serial.println(cardUID);
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    noti = "Card UID:";
    noti_2 = cardUID;

  }
}