#include <AltSoftSerial.h>
//#include <SoftwareSerial.h>

int rxPin = 10;
int txPin = 11;
//SoftwareSerial controller(rxPin, txPin);
AltSoftSerial controller;
int indicatorPin = 6;
int LF = 10;
const int COMMAND_BUFFER_LEN = 100;
bool commandComplete = false;
int commandLen = 0;
char commandBuffer[COMMAND_BUFFER_LEN];
const int CMD_HELLO_LEN = 6;
char CMD_HELLO[CMD_HELLO_LEN] = "HELLO";
const int CMD_GOODBYE_LEN = 7;
char CMD_GOODBYE[CMD_GOODBYE_LEN] = "GOODBYE";

static void readSerialToCommandBuffer();
static void processCommand();

void setup() {
  // Initialize our connected indicator LED
  pinMode(indicatorPin, OUTPUT);
  digitalWrite(indicatorPin, LOW);
  
  // Initialize Serial Port for console communications
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to finish opening...
  }

  controller.begin(9600);
}

void loop() {
  if (controller.available()) {
    if (commandLen == 0) {
      Serial.println("Received data on COM port");
    }
    
    readSerialToCommandBuffer();
    if (commandComplete) {
      Serial.println(commandBuffer);
      processCommand();
    }
  }
  /*else {
    Serial.println("Sending PING");
    controller.println("PING");
    delay(1000);
  }*/
}

static void readSerialToCommandBuffer() {
  char curByte = controller.read();
  if (curByte == LF) {
    // end of current command
    commandComplete = true;
    commandBuffer[commandLen] = '\0';
  }
  else {
    commandBuffer[commandLen++] = curByte;
  }
}

static void resetCommandBuffer() {
  commandLen = 0;
  commandBuffer[0] = '\0';
}

static void processCommand() {
  if (strncmp(CMD_HELLO, commandBuffer, CMD_HELLO_LEN) == 0) {
    resetCommandBuffer();
    Serial.println("Received HELLO command, turning on LED");
    
    // Turn on the LED
    digitalWrite(indicatorPin, HIGH);

    // Respond to HELLO
    Serial.println("Sending READY response");
    controller.println("READY");
  }
  else if (strncmp(CMD_GOODBYE, commandBuffer, CMD_GOODBYE_LEN) == 0) {
    resetCommandBuffer();
    Serial.println("Received GOODBYE command, turning off LED");

    // Turn off the LED
    digitalWrite(indicatorPin, LOW);

    // Response to GOODBYE
    Serial.println("Sending SHUTDOWN response");
    controller.println("SHUTDOWN");
  }
}

