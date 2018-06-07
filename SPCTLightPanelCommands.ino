int extractArgs(String *outBuffer, size_t outBufferLength, char *args) {
  String argString(args);
  int lastSpaceIndex = 0;
  
  for(int i = 0; i < outBufferLength && lastSpaceIndex >= 0; i++) {
    int spaceIndex = argString.indexOf(' ', lastSpaceIndex +1);
    Serial.print("Space Index: ");
    Serial.print(spaceIndex, DEC);
    Serial.print(" Last Space Index: ");
    Serial.println(lastSpaceIndex, DEC);
    
    if (spaceIndex == -1) {
      outBuffer[i] = argString.substring(lastSpaceIndex <= 0 ? lastSpaceIndex : (lastSpaceIndex + 1));
    }
    else {
      outBuffer[i] = argString.substring(lastSpaceIndex, spaceIndex);
    }
    lastSpaceIndex = spaceIndex;
  }

  return lastSpaceIndex;
}

int extractArgs(int *outBuffer, size_t outBufferLength, char *args) {
  String stringBuffer[outBufferLength];
  int retVal = extractArgs(stringBuffer, outBufferLength, args);

  for(int i = 0; i < outBufferLength && stringBuffer[i] != NULL; i++) {
      outBuffer[i] = stringBuffer[i].toInt();
  }

  return retVal;
}

int extractArgs(float *outBuffer, size_t outBufferLength, char *args) {
  String stringBuffer[outBufferLength];
  int retVal = extractArgs(stringBuffer, outBufferLength, args);

  for(int i = 0; i < outBufferLength && stringBuffer[i] != NULL; i++) {
      outBuffer[i] = stringBuffer[i].toFloat();
  }

  return retVal;
}

void processUnknownCommand(AltSoftSerial *controller) {
  Serial.println("Unrecognized command received");
  controller->println("ERROR");
}

void processHelloCommand(AltSoftSerial *controller) {
    Serial.println("Received HELLO command, turning on LED");
    
    // Turn on the LED
    digitalWrite(LED_PIN, HIGH);

    // Respond to HELLO
    Serial.println("Sending READY response");
    controller->println("READY");
}

void processGoodbyeCommand(AltSoftSerial *controller) {
    Serial.println("Received GOODBYE command, turning off LED");

    // Turn off the LED
    digitalWrite(LED_PIN, LOW);

    // Response to GOODBYE
    Serial.println("Sending SHUTDOWN response");
    controller->println("SHUTDOWN");
}

void processCommandPanelOff(AltSoftSerial *controller) {
  Serial.println("Turning off panel");

  // TODO - turn panel off
  
  controller->println("OFF");
}

void processSetTempoCommand(AltSoftSerial *controller, char *args) {
  String argString(args);
  int tempo = argString.toInt();

  Serial.print("Setting panel tempo to ");
  Serial.println(tempo, DEC);
  
  // TODO - set tempo to value
  controller->print("TEMPO ");
  controller->println(tempo, DEC);
}

void processResetCommand(AltSoftSerial *controller, char *args) {
  String argString(args);
  int row = argString.toInt();

  Serial.print("Resetting row");
  Serial.println(row, DEC);
  
  // TODO - set tempo to value
  controller->print("OK ");
  controller->println(row, DEC);
}

void processStopCommand(AltSoftSerial *controller, char *args) {
  int argInts[2];
  extractArgs(argInts, 2, args);

  Serial.print("Stopping Requested Row Display: ");
  Serial.println(argInts[0], DEC);

  // TODO - stop the pattern

  if (argInts[1] == 1) {
    Serial.println("Resetting Stopped Row");

    // TODO - reset the pattern
  }
  
  controller->print("STOPPED ");
  controller->println(argInts[0], DEC);
}

void processSetColorCommand(AltSoftSerial *controller, char *args) {
  int argInts[] = {-1, -1, -1, -1, -1};
  int lastIndex = extractArgs(argInts, 5, args);
  float argFloats[] = { -1.0 };
  if (lastIndex >= 0) {
    extractArgs(argFloats, 1, &args[lastIndex]);
  }

  Serial.print("Setting Color On Requested LED (");
  Serial.print(argInts[0], DEC);
  Serial.print(",");
  Serial.print(argInts[1], DEC);
  Serial.print(") to RGBA (");
  Serial.print(argInts[2], DEC);
  Serial.print(",");
  Serial.print(argInts[3], DEC);
  Serial.print(",");
  Serial.print(argInts[4], DEC);
  Serial.print(",");
  Serial.print(argFloats[0], DEC);
  Serial.println(")");

  // TODO - Set the color

  controller->println("DONE");  
}

void processSetLedOnCommand(AltSoftSerial *controller, char *args) {
  int argInts[] = {-1, -1, -1, -1, -1};
  int lastIndex = extractArgs(argInts, 5, args);
  float argFloats[] = { -1.0 };
  if (lastIndex >= 0) {
    extractArgs(argFloats, 1, &args[lastIndex]);
  }

  Serial.print("Turning On Requested LED (");
  Serial.print(argInts[0], DEC);
  Serial.print(",");
  Serial.print(argInts[1], DEC);
  Serial.println(")");

  // TODO - Turn on the LED

  if (argInts[2] >= 0) {
    Serial.print("Setting LED Color RGBA (");
    Serial.print(argInts[2], DEC);
    Serial.print(",");
    Serial.print(argInts[3], DEC);
    Serial.print(",");
    Serial.print(argInts[4], DEC);
    Serial.print(",");
    Serial.print(argFloats[0], DEC);
    Serial.println(")");

    // TODO - Set LED Color
  }

  controller->println("DONE");  
}

void processSetLedOffCommand(AltSoftSerial *controller, char *args) {
  int argInts[] = {-1, -1, -1, -1, -1};
  int lastIndex = extractArgs(argInts, 5, args);
  float argFloats[] = { -1.0 };
  if (lastIndex >= 0) {
    extractArgs(argFloats, 1, &args[lastIndex]);
  }

  Serial.print("Turning Off Requested LED (");
  Serial.print(argInts[0], DEC);
  Serial.print(",");
  Serial.print(argInts[1], DEC);
  Serial.println(")");

  // TODO - Turn off the LED

  if (argInts[2] >= 0) {
    Serial.print("Setting LED Color RGBA (");
    Serial.print(argInts[2], DEC);
    Serial.print(",");
    Serial.print(argInts[3], DEC);
    Serial.print(",");
    Serial.print(argInts[4], DEC);
    Serial.print(",");
    Serial.print(argFloats[0], DEC);
    Serial.println(")");

    // TODO - Set LED Color
  }

  controller->println("DONE");
}


