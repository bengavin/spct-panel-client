#include <AltSoftSerial.h>

/* Arduino GPIO PINS IN USE */
#define LED_PIN 6
/* PINS 8, 9 and 10 are all effectively used by AltSoftSerial */

#define LF 10

#define COMMAND_BUFFER_LEN 100
#define CMD_HELLO_LEN 6
#define CMD_HELLO "HELLO"
#define CMD_GOODBYE_LEN 7
#define CMD_GOODBYE "GOODBYE"
#define CMD_PANEL_OFF_LEN 8
#define CMD_PANEL_OFF "PANELOFF"
#define CMD_SET_TEMPO_LEN 8
#define CMD_SET_TEMPO "SETTEMPO"
#define CMD_PLAY_LEN 4
#define CMD_PLAY "PLAY"
#define CMD_RESET_LEN 5
#define CMD_STOP_LEN 4
#define CMD_STOP "STOP"
#define CMD_SETCOLOR_LEN 8
#define CMD_SETCOLOR "SETCOLOR"
#define CMD_ON_LEN 2
#define CMD_ON "ON"
#define CMD_OFF_LEN 3
#define CMD_OFF "OFF"

void processHelloCommand(AltSoftSerial *controller);
void processGoodbyeCommand(AltSoftSerial *controller);

AltSoftSerial controller;

static bool commandComplete = false;
static int commandLen = 0;
static char commandBuffer[COMMAND_BUFFER_LEN];

static void readSerialToCommandBuffer();
static void processCommand();

void setup() {
  // Initialize our connected indicator LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
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
    processHelloCommand(&controller);
  }
  else if (strncmp(CMD_GOODBYE, commandBuffer, CMD_GOODBYE_LEN) == 0) {
    resetCommandBuffer();
    processGoodbyeCommand(&controller);
  }
}

