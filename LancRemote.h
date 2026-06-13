#ifndef LANC_MASTER_H
#define LANC_MASTER_H

#include <Arduino.h>

// Standard Camera Telemetry Commands (Injected into Byte 4)
enum LancCommand {
    CMD_STANDBY = 0xEB,
    CMD_RECORD  = 0xFB
};

class LancRemote {
private:
    uint8_t _pin;
    uint32_t _lastFrameTimeUs;
    const uint32_t _frameIntervalUs = 16666; // ~60Hz continuous frame clock
    const uint16_t _bitDurationUs = 104;     // 9600 baud
    
    uint8_t _currentFrame[8];

    void sendByte(uint8_t b);
    void sendFrame(const uint8_t* frame);

public:
    LancRemote(uint8_t pin);
    
    // Initializes pins and broadcasts the wake-up sequence
    void begin();
    
    // Core timing loop. Must be called continuously in void loop()
    void loop(); 

    // API for standard known commands
    void setCommand(LancCommand cmd);

    // API for extensibility (allows overriding any of the 8 bytes)
    void setCustomPayloadByte(uint8_t byteIndex, uint8_t hexValue);
};

#endif