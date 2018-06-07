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
#define CMD_RESET "RESET"
#define CMD_STOP_LEN 4
#define CMD_STOP "STOP"
#define CMD_SET_COLOR_LEN 8
#define CMD_SET_COLOR "SETCOLOR"
#define CMD_ON_LEN 2
#define CMD_ON "ON"
#define CMD_OFF_LEN 3
#define CMD_OFF "OFF"

void processUnknownCommand(AltSoftSerial *controller);
void processHelloCommand(AltSoftSerial *controller);
void processGoodbyeCommand(AltSoftSerial *controller);
void processCommandPanelOff(AltSoftSerial *controller);
void processSetTempoCommand(AltSoftSerial *controller, char *args);
void processResetCommand(AltSoftSerial *controller, char *args);
void processStopCommand(AltSoftSerial *controller, char *args);
void processSetColorCommand(AltSoftSerial *controller, char *args);
void processSetLedOnCommand(AltSoftSerial *controller, char *args);
void processSetLedOffCommand(AltSoftSerial *controller, char *args);

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
  commandComplete = false;
  commandLen = 0;
  for(int i = 0; i < COMMAND_BUFFER_LEN && commandBuffer[i] != '\0'; i++) { commandBuffer[i] = '\0'; }
}

static void processCommand() {
  if (strncmp(CMD_HELLO, commandBuffer, CMD_HELLO_LEN) == 0) {
    processHelloCommand(&controller);
  }
  else if (strncmp(CMD_GOODBYE, commandBuffer, CMD_GOODBYE_LEN) == 0) {
    processGoodbyeCommand(&controller);
  }
  else if (strncmp(CMD_PANEL_OFF, commandBuffer, CMD_PANEL_OFF_LEN) == 0) {
    processCommandPanelOff(&controller);
  }
  else if (strncmp(CMD_SET_TEMPO, commandBuffer, CMD_SET_TEMPO_LEN) == 0) {
    processSetTempoCommand(&controller, &commandBuffer[CMD_SET_TEMPO_LEN]);
  }
  else if (strncmp(CMD_RESET, commandBuffer, CMD_RESET_LEN) == 0) {
    processResetCommand(&controller, &commandBuffer[CMD_RESET_LEN]);
  }
  else if (strncmp(CMD_STOP, commandBuffer, CMD_STOP_LEN) == 0) {
    processStopCommand(&controller, &commandBuffer[CMD_STOP_LEN]);
  }
  else if (strncmp(CMD_SET_COLOR, commandBuffer, CMD_SET_COLOR_LEN) == 0) {
    processSetColorCommand(&controller, &commandBuffer[CMD_SET_COLOR_LEN]);
  }
  else if (strncmp(CMD_ON, commandBuffer, CMD_ON_LEN) == 0) {
    processSetLedOnCommand(&controller, &commandBuffer[CMD_ON_LEN]);
  }
  else if (strncmp(CMD_OFF, commandBuffer, CMD_OFF_LEN) == 0) {
    processSetLedOffCommand(&controller, &commandBuffer[CMD_OFF_LEN]);
  }
  else {
    processUnknownCommand(&controller);
  }
  
  resetCommandBuffer();
}

