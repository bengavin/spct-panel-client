#undef round

#include <math.h>
#include <AltSoftSerial.h>
#include <Adafruit_NeoPixel.h>
#include <avr/pgmspace.h>
#include "NeoAnimator.h"

/* Arduino GPIO PINS IN USE */
#define LED_PIN 6
/* PINS 8, 9 and 10 are all effectively used by AltSoftSerial */
#define TEMPO_PIN 13
#define CNT_TEMPO_LIGHTS 8
#define BANK_1_PIN 12
#define CNT_BANK_1_LIGHTS 8
#define BANK_2_PIN 11
#define CNT_BANK_2_LIGHTS 8

NeoAnimator tempoLine = NeoAnimator(CNT_TEMPO_LIGHTS, TEMPO_PIN, NEO_RGB + NEO_KHZ800, NULL);
NeoAnimator bank1Line = NeoAnimator(CNT_BANK_1_LIGHTS, BANK_1_PIN, NEO_RGB + NEO_KHZ800, NULL);
NeoAnimator bank2Line = NeoAnimator(CNT_BANK_2_LIGHTS, BANK_2_PIN, NEO_RGB + NEO_KHZ800, NULL);

#define LF 10

#define COMMAND_BUFFER_LEN 240

#define CMD_HELLO_LEN 6
#define CMD_HELLO F("HELLO")
#define CMD_GOODBYE_LEN 7
#define CMD_GOODBYE F("GOODBYE")
#define CMD_PANEL_OFF_LEN 8
#define CMD_PANEL_OFF F("PANELOFF")
#define CMD_SET_TEMPO_LEN 8
#define CMD_SET_TEMPO F("SETTEMPO")
#define CMD_PLAY_LEN 4
#define CMD_PLAY F("PLAY")
#define CMD_RESET_LEN 5
#define CMD_RESET F("RESET")
#define CMD_STOP_LEN 4
#define CMD_STOP F("STOP")
#define CMD_SET_COLOR_LEN 8
#define CMD_SET_COLOR F("SETCOLOR")
#define CMD_ON_LEN 2
#define CMD_ON F("ON")
#define CMD_OFF_LEN 3
#define CMD_OFF F("OFF")

void processUnknownCommand(AltSoftSerial *controller);
void processHelloCommand(AltSoftSerial *controller);
void processGoodbyeCommand(AltSoftSerial *controller);
void processCommandPanelOff(AltSoftSerial *controller);
void processSetTempoCommand(AltSoftSerial *controller, char *args);
void processResetCommand(AltSoftSerial *controller, char *args);
void processPlayCommand(AltSoftSerial *controller, char *args);
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

  controller.begin(57600);
  tempoLine.begin();
  tempoLine.show(); // all off

  bank1Line.begin();
  bank1Line.show();

  bank2Line.begin();
  bank2Line.show();
  
  tempoLine.InitializeTempoTracker(0xFFFFFFFF, floor(60 / (float)120 * 1000));
}

void loop() {
  if (controller.available()) {
    if (commandLen == 0) {
      //Serial.println("Received data on COM port");
    }
    
    readSerialToCommandBuffer();
    if (commandComplete) {
      Serial.println(commandBuffer);
      processCommand();
    }
  }

  tempoLine.Update();
  bank1Line.Update();
  bank2Line.Update();
}

static void readSerialToCommandBuffer() {
  char curByte = controller.read();
  if (curByte == LF) {
    // end of current command
    commandComplete = true;
    commandBuffer[commandLen] = 0; //'\0';
  }
  else {
    commandBuffer[commandLen++] = curByte;
  }
}

static void resetCommandBuffer() {
  commandComplete = false;
  commandLen = 0;
  for(int i = 0; i < COMMAND_BUFFER_LEN && commandBuffer[i] != '\0'; i++) { commandBuffer[i] = 0; }
}

static void processCommand() {
  if (strncmp_P(commandBuffer, (const char *)CMD_HELLO, CMD_HELLO_LEN) == 0) {
    processHelloCommand(&controller);
  }
  else if (strncmp_P(commandBuffer, (const char *)CMD_GOODBYE, CMD_GOODBYE_LEN) == 0) {
    processGoodbyeCommand(&controller);
  }
  else if (strncmp_P(commandBuffer, (const char *)CMD_PANEL_OFF, CMD_PANEL_OFF_LEN) == 0) {
    processCommandPanelOff(&controller);
  }
  else if (strncmp_P(commandBuffer, (const char *)CMD_SET_TEMPO, CMD_SET_TEMPO_LEN) == 0) {
    processSetTempoCommand(&controller, &commandBuffer[CMD_SET_TEMPO_LEN]);
  }
  else if (strncmp_P(commandBuffer, (const char *)CMD_RESET, CMD_RESET_LEN) == 0) {
    processResetCommand(&controller, &commandBuffer[CMD_RESET_LEN]);
  }
  else if (strncmp_P(commandBuffer, (const char *)CMD_PLAY, CMD_PLAY_LEN) == 0) {
    processPlayCommand(&controller, &commandBuffer[CMD_PLAY_LEN]);
  }
  else if (strncmp_P(commandBuffer, (const char *)CMD_STOP, CMD_STOP_LEN) == 0) {
    processStopCommand(&controller, &commandBuffer[CMD_STOP_LEN]);
  }
  else if (strncmp_P(commandBuffer, (const char *)CMD_SET_COLOR, CMD_SET_COLOR_LEN) == 0) {
    processSetColorCommand(&controller, &commandBuffer[CMD_SET_COLOR_LEN]);
  }
  else if (strncmp_P(commandBuffer, (const char *)CMD_ON, CMD_ON_LEN) == 0) {
    processSetLedOnCommand(&controller, &commandBuffer[CMD_ON_LEN]);
  }
  else if (strncmp_P(commandBuffer, (const char *)CMD_OFF, CMD_OFF_LEN) == 0) {
    processSetLedOffCommand(&controller, &commandBuffer[CMD_OFF_LEN]);
  }
  else {
    processUnknownCommand(&controller);
  }
  
  resetCommandBuffer();
}

