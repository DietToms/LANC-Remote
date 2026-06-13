#include "LancRemote.h"

LancRemote::LancRemote(uint8_t pin) {
    _pin = pin;
    _lastFrameTimeUs = 0;

    // Standard Camera Telemetry Base Payload
    _currentFrame[0] = 0xFF;
    _currentFrame[1] = 0xFF;
    _currentFrame[2] = 0xB6;
    _currentFrame[3] = 0xFF;
    _currentFrame[4] = CMD_STANDBY; // Default boot state
    _currentFrame[5] = 0xFF;
    _currentFrame[6] = 0xFF;
    _currentFrame[7] = 0xFF;
}

void LancRemote::begin() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, HIGH); // Bus idles HIGH
    
    delay(10); // Allow hardware bus to settle

    // Wake-up sequence: Broadcast 3 blank frames to sync the monitor
    uint8_t blankFrame[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for (int i = 0; i < 3; i++) {
        sendFrame(blankFrame);
        delay(20);
    }
}

void LancRemote::setCommand(LancCommand cmd) {
    _currentFrame[4] = (uint8_t)cmd;
}

void LancRemote::setCustomPayloadByte(uint8_t byteIndex, uint8_t hexValue) {
    if (byteIndex < 8) {
        _currentFrame[byteIndex] = hexValue;
    }
}

void LancRemote::sendByte(uint8_t b) {
    // Disable interrupts to prevent background OS tasks (WiFi, USB CDC, timers) 
    // from stretching the 104us bit-banging window.
    noInterrupts();
    
    digitalWrite(_pin, LOW); // Start bit
    delayMicroseconds(_bitDurationUs);
    
    for (int i = 0; i < 8; i++) {
        digitalWrite(_pin, (b & 0x01) ? HIGH : LOW);
        b >>= 1;
        delayMicroseconds(_bitDurationUs);
    }
    
    digitalWrite(_pin, HIGH); // Stop bit
    delayMicroseconds(_bitDurationUs);
    
    interrupts();
}

void LancRemote::sendFrame(const uint8_t* frame) {
    for (int i = 0; i < 8; i++) {
        sendByte(frame[i]);
    }
}

void LancRemote::loop() {
    uint32_t currentUs = micros();
    
    // Evaluate if it is time to clock out the next video frame.
    // Unsigned subtraction inherently handles the 70-minute micros() rollover.
    if (currentUs - _lastFrameTimeUs >= _frameIntervalUs) {
        _lastFrameTimeUs = currentUs;
        sendFrame(_currentFrame);
    }
}