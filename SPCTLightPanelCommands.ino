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
