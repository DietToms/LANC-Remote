/*
 * LancRemote Example: Pushbutton Toggle
 * Demonstrates basic hardware control of a LANC recorder.
 * * Hardware Setup:
 * - D9: Connect to the Tip of the 2.5mm LANC jack.
 * - D2: Connect to a momentary pushbutton. Connect the other side to GND.
 */

#include "LancRemote.h"

#define LANC_TX_PIN 9
#define BUTTON_PIN  2
#define LED_PIN     13

LancRemote lanc(LANC_TX_PIN);

bool isRecording = false;
bool lastButtonState = HIGH; // Tracks previous loop state for edge detection

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Boot the bus and send the wake-up sequence
    lanc.begin();
}

void loop() {
    // 1. Maintain the 60Hz PHY heartbeat 
    lanc.loop();

    // 2. Read the physical button
    bool currentButtonState = digitalRead(BUTTON_PIN);

    // Detect falling edge (button pressed to GND)
    // NOTE: Add software debouncing in production to prevent contact-bounce triggers
    if (lastButtonState == HIGH && currentButtonState == LOW) {
        
        isRecording = !isRecording; // Toggle state

        if (isRecording) {
            lanc.setCommand(CMD_RECORD);
            digitalWrite(LED_PIN, HIGH);
        } else {
            lanc.setCommand(CMD_STANDBY);
            digitalWrite(LED_PIN, LOW);
        }
    }

    lastButtonState = currentButtonState;
}
