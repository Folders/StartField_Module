#pragma once

#include "common.h"
#include <Arduino.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#define BUFFER_SIZE 256
#define MAX_PARAMS 8
#define MAX_FEATURES 10

class MyComm {
public:
    MyComm();

    void setID(uint8_t id);
    void addFeature(const char* feature);
    void begin(uint16_t udpPort);
    void handle();
    void send(const char* message);

    bool hasNewCommand() const;
    const char* GetCode() const;
    const char* GetParameter(uint8_t index) const;
    uint8_t GetSize() const;

#ifdef LOG
    void handleSerialDebug();  // Debug: injecte des commandes via Serial
#endif

private:
    uint8_t _deviceID;
    char _features[MAX_FEATURES][8];
    uint8_t _featureCount;
    uint16_t _udpPort;

    WiFiUDP _udp;
    IPAddress _serverIP;

    char _bufferIn[BUFFER_SIZE];
    char _bufferOut[BUFFER_SIZE];
    char _code[8];
    char _messageId[8];
    char* _params[MAX_PARAMS];
    uint8_t _paramCount;

    bool _connected;
    bool _newCommand;
    unsigned long _lastHeartbeat;
    const unsigned long _heartbeatInterval = 1000; // 1s
    unsigned long _lastTCPAttempt;

    static constexpr uint8_t MAX_QUEUE = 10;
    static constexpr uint8_t MAX_CMD_LEN = 64;

    // Buffer datas
    char _commandQueue[MAX_QUEUE][MAX_CMD_LEN];
    uint8_t _queueStart = 0;
    uint8_t _queueEnd = 0;
    // ... with and persistant queue
    char _rxFrameBuffer[BUFFER_SIZE];
    uint16_t _rxFrameLen = 0;


    void _handleUDP();
    void _queueMessage();

    void _sendBOT();
    void _processMessage(char* msg);
    void _sendRaw(const char* payload, bool forceBroadcast, const char* messageId);
    void _sendACK();
};