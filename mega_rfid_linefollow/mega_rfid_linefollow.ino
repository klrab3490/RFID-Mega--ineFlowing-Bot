#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

#define RST_PIN 8   // Define the pin connected to RST on the RFID reader
#define SS_PIN 9    // Define the pin connected to SDA on the RFID reader

MFRC522 rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance

SoftwareSerial espSerial(10, 11); // RX, TX -> ESP8266

// Motor
// - - Motor A - -
const int enA = 2;
const int in1 = 3;
const int in2 = 4;
// - - Motor B - - 
const int enB = 5;
const int in3 = 6;
const int in4 = 7;

// IR Sensor
const int sensor_left = 30; //Left Sensor
const int sensor_right = 32; //Right Sensor

// State variables
bool scanningRFID = false;
String expectedUID;

void setup() {
  // Serial Communication
  Serial.begin(9600); // Computer
  espSerial.begin(9600); // ESP8266(NodeMCU)
  
  // RFID Module
  SPI.begin(); // Init SPI bus(PINS 50,51,52,53)
  rfid.PCD_Init(); // Init RFID Module
  Serial.println("RFID reader initialized.");

  // Motor Pins Control Setting
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  Serial.println("Motor initialized.");

  // IR Sensor
  pinMode(sensor_right, INPUT);
  pinMode(sensor_left, INPUT);
  Serial.println("IR Sensor initialized.");
}

void loop() { 
  // Handle RFID scanning
  if (!scanningRFID && espSerial.available()) {
    String message = espSerial.readStringUntil('\n');
    Serial.println("Message from ESP8266: " + message);

    if (message.startsWith("UID:")) {
      expectedUID = message.substring(4); // Remove "UID:" prefix
      expectedUID.trim(); // Remove leading/trailing whitespaces

      Serial.println("RFID Tag Value Send from Website(UID): " + expectedUID);
      scanningRFID = true;
    }
  }

  if (scanningRFID) {
    String cardUID = scanRFIDCard();
    if (cardUID != "") {  
      compareAndAct(expectedUID, cardUID);
    }else{
      handleSensorInput();
    }
  }
  
}

String scanRFIDCard() {
  String cardUID = "";
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    for (int i = 0; i < rfid.uid.size; i++) {
      char hexBuffer[3];
      sprintf(hexBuffer, "%02X", rfid.uid.uidByte[i]);
      cardUID += hexBuffer;
    }
    Serial.println("Read RFID Value: " + cardUID);
  }
  return cardUID;
}

void handleSensorInput() {
  int sensor_left_value = digitalRead(sensor_left); 
  int sensor_right_value = digitalRead(sensor_right);

  if(sensor_left_value == LOW && sensor_right_value == LOW) {
    moveForward();
  } else if(sensor_left_value == LOW && sensor_right_value == HIGH) {
    turnLeft();
  } else if(sensor_left_value == HIGH && sensor_right_value == LOW) {
    turnRight();
  } else {
    stopMotors();
  }
}

void compareAndAct(String expectedUID, String scannedUID) {
  if (expectedUID.equalsIgnoreCase(scannedUID)) {
    Serial.println("RFID tag found!");
    rotate();
    stopMotors();
    scanningRFID = false; // Reset the RFID scanning flag
  } else {
    Serial.println("RFID tag not found.");
    // Perform action for not finding the RFID tag
  }
}

void moveForward() {
  analogWrite(enA, 75);
  analogWrite(enB, 75);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void turnRight() {
  analogWrite(enA, 75);
  analogWrite(enB, 75);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void turnLeft() {
  analogWrite(enA, 75);
  analogWrite(enB, 75);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void stopMotors() {
  analogWrite(enA, 0);
  analogWrite(enB, 0);
}

void rotate() {
  analogWrite(enA, 100);
  analogWrite(enB, 100);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(1000);
}
