
int extractArgs(String *outBuffer, size_t outBufferLength, char *args) {
  String argString(args);
  int lastSpaceIndex = 0;

  // Scroll forward to the first non-space
  while(args[lastSpaceIndex] == ' ' && lastSpaceIndex < outBufferLength) {
    lastSpaceIndex++;
  }
  
  for(int i = 0; i < outBufferLength && lastSpaceIndex >= 0; i++) {
    int spaceIndex = argString.indexOf(' ', lastSpaceIndex);
    //Serial.print("Last Space Index: ");
    //Serial.print(lastSpaceIndex, DEC);
    //Serial.print(" Space Index: ");
    //Serial.println(spaceIndex, DEC);
    
    if (spaceIndex == -1) {
      outBuffer[i] = argString.substring(lastSpaceIndex);
    }
    else {
      outBuffer[i] = argString.substring(lastSpaceIndex, spaceIndex);
    }
    
    lastSpaceIndex = spaceIndex < 0 ? spaceIndex : spaceIndex + 1;
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

int extractColor(uint32_t *color, char *args)
{
  int argInts[] = {-1, -1, -1};
  int lastIndex = extractArgs(argInts, 3, args);
  float argFloats[] = { -1.0 };
  if (lastIndex >= 0) {
    int floatLastIndex = extractArgs(argFloats, 1, &args[lastIndex]);
    lastIndex = floatLastIndex >= 0 ? lastIndex + floatLastIndex : floatLastIndex;
  }

  float alphaVal = argFloats[0] <= 0 ? 1.0 : argFloats[0];
  uint8_t alpha = (uint8_t)((uint32_t)round(alphaVal * 255) & 0xFF);
  uint8_t red = (uint8_t)((argInts[2] == -1 ? 0 : argInts[0]) & 0xFF);
  uint8_t green = (uint8_t)((argInts[3] == -1 ? 0 : argInts[1]) & 0xFF);
  uint8_t blue = (uint8_t)((argInts[4] == -1 ? 0 : argInts[2]) & 0xFF);

  uint32_t output = ((uint32_t)alpha << 24) +
                    ((uint32_t)red << 16) +
                    ((uint32_t)green << 8) +
                    blue;
  //Serial.print(F("RGBA -> HEX: "));
  //Serial.print(red);
  //Serial.print(F(","));
  //Serial.print(green);
  //Serial.print(F(","));
  //Serial.print(blue);
  //Serial.print(F(","));
  //Serial.print(alpha);
  //Serial.print(F(" -> "));
  //Serial.print(output >> 16, HEX);
  //Serial.println(output & 0xFFFF, HEX);
  
  *color = output;
  return lastIndex;
}

void processPatternSetup(int pattern, NeoAnimator *lineBank, char *patternArgs)
{
  Serial.print(F("Processing pattern setup: "));
  Serial.println(pattern);
  
  int colorTempoArgs[2];
  int lastIndex = extractArgs(colorTempoArgs, 2, patternArgs);
  char *args = lastIndex >= 0 ? &patternArgs[lastIndex] : NULL;
  //Serial.print("Args remaining: '");
  //Serial.print(args);
  //Serial.println("'");
  
  Serial.print(F("Speed: "));
  Serial.print(colorTempoArgs[0]);
  Serial.print(F(" Number of Colors: "));
  Serial.println(colorTempoArgs[1]);
  
  uint32_t tempoInterval = colorTempoArgs[0];
  uint32_t tempoStepMs = floor(60 / (float)tempoInterval * 1000);
  int numColors = colorTempoArgs[1];
  uint32_t *patternColors = NULL;
  if (numColors > 0)
  {
    patternColors = malloc(sizeof(uint32_t) * numColors);
    for (int i = 0; i < numColors && args != NULL; i++)
    {
      lastIndex = extractColor(&patternColors[i], args);
      args = lastIndex >= 0 ? &args[lastIndex] : NULL; 
      Serial.print(F("Extracted Color: "));
      Serial.println(patternColors[i], HEX);
      //Serial.print("Args remaining: '");
      //Serial.print(args);
      //Serial.println("'");
    }
  }
  
  switch(pattern)
  {
    case 0: // no pattern (leaves existing pattern)
      break;

    case 1: // Tempo pattern
      // interval is interpreted as BPM
      lineBank->InitializeTempoTracker(patternColors[0], tempoStepMs);
      
      break;

    case 2:
      // Interval is direct ms time
      lineBank->InitializeTheaterChase(patternColors, numColors, tempoInterval, 8 - numColors, FORWARD, false);
      break;

    case 3: // Bounce
      // Interval is direct ms time
      lineBank->InitializeBounce(patternColors, numColors, tempoInterval, FORWARD, false);
      break;

    case 4: // Chase 2
      lineBank->InitializeChase2(patternColors, numColors, tempoInterval, FORWARD, false);
      break;
  }

  if (patternColors != NULL) 
  {
    free(patternColors);
  }
}

void processUnknownCommand(AltSoftSerial *controller) {
  Serial.println("Unrecognized command received");
  controller->println("ERROR");
}

void processHelloCommand(AltSoftSerial *controller) {
    Serial.println("Received HELLO command, turning on LED");
    
    // Turn on the LED
    digitalWrite(LED_PIN, HIGH);

    // Run through test procedure
    tempoLine.ClearColor(255,0,0);
    bank1Line.ClearColor(0,255,0);
    bank2Line.ClearColor(0,0,255);
    delay(500);
    tempoLine.ClearColor(0,255,0);
    bank1Line.ClearColor(0,0,255);
    bank2Line.ClearColor(255,0,0);
    delay(500);
    tempoLine.ClearColor(0,0,255);
    bank1Line.ClearColor(255,0,0);
    bank2Line.ClearColor(0,255,0);
    delay(500);
    tempoLine.ClearColor(255,255,255);
    bank1Line.ClearColor(255,255,255);
    bank2Line.ClearColor(255,255,255);
    delay(500);
    tempoLine.ClearColor(255,255,255,128);
    bank1Line.ClearColor(255,255,255,128);
    bank2Line.ClearColor(255,255,255,128);
    delay(500);
    tempoLine.Reset();
    bank1Line.Reset();
    bank2Line.Reset();

    // Respond to HELLO
    Serial.println("Sending READY response");
    controller->println("READY");
}

void processGoodbyeCommand(AltSoftSerial *controller) {
    Serial.println("Received GOODBYE command, turning off LED");

    // Turn the panel off
    tempoLine.Stop();
    tempoLine.Reset();

    bank1Line.Stop();
    bank1Line.Reset();

    bank2Line.Stop();
    bank2Line.Reset();
    
    // Turn off the LED
    digitalWrite(LED_PIN, LOW);

    // Response to GOODBYE
    Serial.println("Sending SHUTDOWN response");
    controller->println("SHUTDOWN");
}

void processCommandPanelOff(AltSoftSerial *controller) {
  Serial.println("Turning off panel");

  tempoLine.Stop();
  tempoLine.Reset();
  
  bank1Line.Stop();
  bank1Line.Reset();

  bank2Line.Stop();
  bank2Line.Reset();

  controller->println("OFF");
}

void processSetTempoCommand(AltSoftSerial *controller, char *args) {
  String argString(args);
  int tempo = argString.toInt();
  long int tempoStepMs = floor(60 / (float)tempo * 1000);

  Serial.print("Setting panel tempo to ");
  Serial.println(tempo, DEC);

  tempoLine.Interval = tempoStepMs;
  
  controller->print("TEMPO ");
  controller->println(tempo, DEC);
}

void processPlayCommand(AltSoftSerial *controller, char *args) {
  int argInts[2];
  int lastIndex = extractArgs(argInts, 2, args);
  int row = argInts[0];
  int pattern = argInts[1];
  char *patternArgs = lastIndex >= 0 ? &args[lastIndex] : NULL;
  
  Serial.print("Starting panel row ");
  Serial.println(row, DEC);
  
  switch(row) {
    case 0:
      tempoLine.Start();
      break;

    case 1:
      bank1Line.Stop();
      bank1Line.Reset();
      if (pattern > -1)
      {
        processPatternSetup(pattern, &bank1Line, patternArgs);
      }
      bank1Line.Start();
      break;

    case 2:
      bank2Line.Stop();
      bank2Line.Reset();
      if (pattern > -1)
      {
        processPatternSetup(pattern, &bank2Line, patternArgs);
      }
      bank2Line.Start();
      break;
  }
  
  controller->print("PLAYING ");
  controller->println(row, DEC);
}

void processResetCommand(AltSoftSerial *controller, char *args) {
  String argString(args);
  int row = argString.toInt();

  Serial.print("Resetting row");
  Serial.println(row, DEC);
  
  switch(row) {
    case 0:
      tempoLine.Reset();
      break;

    case 1:
      bank1Line.Reset();
      break;

    case 2:
      bank2Line.Reset();
      break;
  }
  
  controller->print("OK ");
  controller->println(row, DEC);
}

void processStopCommand(AltSoftSerial *controller, char *args) {
  int argInts[2];
  extractArgs(argInts, 2, args);

  Serial.print("Stopping Requested Row Display: ");
  Serial.println(argInts[0], DEC);

  switch(argInts[0]) {
    case 0:
      tempoLine.Stop();
      break;
      
    case 1:
      bank1Line.Stop();
      break;

    case 2:
      bank2Line.Stop();
      break;
  }
  
  if (argInts[1] == 1) {
    Serial.println("Resetting Stopped Row");

    switch(argInts[0]) {
      case 0:
        tempoLine.Reset();
        break;
        
      case 1:
        bank1Line.Reset();
        break;
  
      case 2:
        bank2Line.Reset();
        break;
    }
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

  float alphaVal = argFloats[0] == -1 ? 1.0 : argFloats[0];
  uint8_t alpha = (uint8_t)((uint32_t)round(alphaVal * 255) & 0xFF);
  uint8_t red = (uint8_t)((argInts[2] == -1 ? 0 : argInts[2]) & 0xFF);
  uint8_t green = (uint8_t)((argInts[3] == -1 ? 0 : argInts[3]) & 0xFF);
  uint8_t blue = (uint8_t)((argInts[4] == -1 ? 0 : argInts[4]) & 0xFF);
  
  Serial.print("Converted Colors (");
  Serial.print(red, HEX);
  Serial.print(",");
  Serial.print(green, HEX);
  Serial.print(",");
  Serial.print(blue, HEX);
  Serial.println(")");

  switch(argInts[0]) {
    case 0:
      // Row 0 only supports setting a single color at this time
      tempoLine.SetColor(0, red, green, blue, alpha);
      break;

    case 1:
      bank1Line.setPixelColor(argInts[1], red, green, blue);
      break;

    case 2:
      bank2Line.setPixelColor(argInts[1], red, green, blue);
      break;
  }

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

  if (argInts[2] >= 0) {
    float alphaVal = argFloats[0] == -1 ? 1.0 : argFloats[0];
    uint8_t alpha = (uint8_t)((uint32_t)round(alphaVal * 255) & 0xFF);
    uint8_t red = (uint8_t)((argInts[2] == -1 ? 0 : argInts[2]) & 0xFF);
    uint8_t green = (uint8_t)((argInts[3] == -1 ? 0 : argInts[3]) & 0xFF);
    uint8_t blue = (uint8_t)((argInts[4] == -1 ? 0 : argInts[4]) & 0xFF);
  
    Serial.print("Setting LED Color RGBA (");
    Serial.print(argInts[2], DEC);
    Serial.print(",");
    Serial.print(argInts[3], DEC);
    Serial.print(",");
    Serial.print(argInts[4], DEC);
    Serial.print(",");
    Serial.print(argFloats[0], DEC);
    Serial.println(")");

    switch(argInts[0])
    {
      case 1:
        bank1Line.setPixelColor(argInts[1], red, green, blue);
        break;

      case 2:
        bank2Line.setPixelColor(argInts[1], red, green, blue);
        break;
     }
  }

  switch(argInts[0]) {
    case 0:
      // Do nothing (not supported)
      break;

    case 1:
      bank1Line.show();
      break;

    case 2:
      bank2Line.show();
      break;
  }

  controller->println("DONE");  
}

void processSetLedOffCommand(AltSoftSerial *controller, char *args) {
  int argInts[] = {-1, -1, -1, -1, -1};
  int lastIndex = extractArgs(argInts, 5, args);
  float argFloats[] = { -1.0 };
  if (lastIndex >= 0) 
  {
    extractArgs(argFloats, 1, &args[lastIndex]);
  }

  Serial.print("Turning Off Requested LED (");
  Serial.print(argInts[0], DEC);
  Serial.print(",");
  Serial.print(argInts[1], DEC);
  Serial.println(")");

  // TODO - Turn off the LED
  switch(argInts[0]) 
  {
    case 0:
      // Do nothing (not supported)
      break;
      
    case 1:
      bank1Line.setPixelColor(argInts[1], 0, 0, 0);
      bank1Line.show();
      break;
      
    case 2:
      bank2Line.setPixelColor(argInts[1], 0, 0, 0);
      bank2Line.show();
      break;
  }
  
  if (argInts[2] >= 0) 
  {
    float alphaVal = argFloats[0] == -1 ? 1.0 : argFloats[0];
    uint8_t alpha = (uint8_t)((uint32_t)round(alphaVal * 255) & 0xFF);
    uint8_t red = (uint8_t)((argInts[2] == -1 ? 0 : argInts[2]) & 0xFF);
    uint8_t green = (uint8_t)((argInts[3] == -1 ? 0 : argInts[3]) & 0xFF);
    uint8_t blue = (uint8_t)((argInts[4] == -1 ? 0 : argInts[4]) & 0xFF);
  
    Serial.print("Setting LED Color RGBA (");
    Serial.print(argInts[2], DEC);
    Serial.print(",");
    Serial.print(argInts[3], DEC);
    Serial.print(",");
    Serial.print(argInts[4], DEC);
    Serial.print(",");
    Serial.print(argFloats[0], DEC);
    Serial.println(")");

    switch(argInts[0])
    {
      case 1:
        bank1Line.setPixelColor(argInts[1], red, green, blue);
        break;

      case 2:
        bank2Line.setPixelColor(argInts[1], red, green, blue);
        break;
     }
  }

  controller->println("DONE");
}


