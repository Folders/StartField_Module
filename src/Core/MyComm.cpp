#include "Core/MyComm.h"
#include "common.h"

MyComm::MyComm()
    : _deviceID(0), _featureCount(0), _udpPort(0), _tcpPort(0),
      _paramCount(0), _newCommand(false), _lastHeartbeat(0), _lastTCPAttempt(0) {
    _bufferIn[0] = '\0';
    _bufferOut[0] = '\0';
    _code[0] = '\0';
}

void MyComm::setID(uint8_t id) {
    _deviceID = id;
}

void MyComm::addFeature(const char* feature) {
    if (_featureCount < MAX_FEATURES) {
        strncpy(_features[_featureCount], feature, sizeof(_features[_featureCount]) - 1);
        _features[_featureCount][sizeof(_features[_featureCount]) - 1] = '\0';
        _featureCount++;
    }
}

void MyComm::begin(uint16_t udpPort, uint16_t tcpPort) {
    _udpPort = udpPort;
    _tcpPort = tcpPort;
    _udp.begin(_udpPort);

#ifdef LOG
    Serial.printf("UDP started on %d, TCP target port %d\n", _udpPort, _tcpPort);
#endif
}

void MyComm::handle() {

    if (!_tcp.connected())
    {
        unsigned long now = millis();

        if (now - _lastHeartbeat >= _heartbeatInterval) {
            _sendBOT();
            _lastHeartbeat = now;
        }
    }


    _newCommand = false;

    _handleUDP();
    _handleTCP();
}

void MyComm::_handleUDP() {
    int packetSize = _udp.parsePacket();
    if (packetSize) {
        int len = _udp.read(_bufferIn, BUFFER_SIZE - 1);
        if (len > 0) _bufferIn[len] = '\0';

#ifdef LOG
        Serial.print("UDP Received: ");
        Serial.println(_bufferIn);
#endif
        _serverIP = _udp.remoteIP();
        _processMessage(false);
    }
}

void MyComm::_handleTCP() {
    if (_tcp.connected()) {
        while (_tcp.available()) {
            int len = _tcp.readBytesUntil('\n', _bufferIn, BUFFER_SIZE - 1);
            _bufferIn[len] = '\0';
#ifdef LOG
            Serial.print("TCP Received: ");
            Serial.println(_bufferIn);
#endif
            _processMessage(true);
        }
    } else {
        _reconnectTCP();
    }
}

void MyComm::_reconnectTCP() {
    unsigned long now = millis();
    if (_tcpPort > 0 && _serverIP && (now - _lastTCPAttempt > 5000)) {
        _lastTCPAttempt = now;
        if (_tcp.connect(_serverIP, _tcpPort)) {
#ifdef LOG
            Serial.println("TCP Connected");
#endif
        }
    }
}

void MyComm::_sendBOT() {
    snprintf(_bufferOut, BUFFER_SIZE, "BOT;%u", _deviceID);
    for (uint8_t i = 0; i < _featureCount; i++) {
        strncat(_bufferOut, ";", BUFFER_SIZE - strlen(_bufferOut) - 1);
        strncat(_bufferOut, _features[i], BUFFER_SIZE - strlen(_bufferOut) - 1);
    }
    send(_bufferOut);
}

void MyComm::send(const char* message) {
#ifdef LOG
    if (_tcp.connected()) 
        Serial.print("Sending TCP: ");
    else
        Serial.print("Sending UDP: ");
    Serial.println(message);
#endif
    if (_tcp.connected()) {
        _tcp.print(message);
        _tcp.print("\n");
    } else {
        _udp.beginPacket(_serverIP ? _serverIP : IPAddress(255,255,255,255), _udpPort);
        _udp.write(message);
        _udp.endPacket();
    }
}

void MyComm::_processMessage(bool fromTCP) {
    char* token = strtok(_bufferIn, ";");
    if (!token) return;

    // Sauvegarde du code
    strncpy(_code, token, sizeof(_code) - 1);
    _code[sizeof(_code) - 1] = '\0';

    // Récupérer les paramètres
    _paramCount = 0;
    while ((token = strtok(NULL, ";")) && _paramCount < MAX_PARAMS) {
        _params[_paramCount++] = token;
    }


    if (strcmp(_code, "PIG") == 0) {
        _respond("POG", fromTCP);
        return;
    }
    if (strcmp(_code, "LVL") == 0) {
        char resp[32];
        snprintf(resp, sizeof(resp), "LVL;%d", WiFi.RSSI());
        _respond(resp, fromTCP);
        return;
    }
    if (strcmp(_code, "RBT") == 0) {
        _sendBOT();
        return;
    }

    _newCommand = true;
}

void MyComm::_respond(const char* msg, bool viaTCP) {
    if (viaTCP && _tcp.connected()) {
        _tcp.print(msg);
        _tcp.print("\n");
    } else {
        _udp.beginPacket(_serverIP, _udpPort);
        _udp.write(msg);
        _udp.endPacket();
    }
}

bool MyComm::hasNewCommand() const {
    return _newCommand;
}

const char* MyComm::GetCode() const {
    return _code;
}

const char* MyComm::GetParameter(uint8_t index) const {
    if (index < _paramCount) return _params[index];
    return "-";
}

uint8_t MyComm::GetSize() const {
    return _paramCount;
}

#ifdef LOG
void MyComm::handleSerialDebug() {
    static String inputBuffer;
    if (Serial.available() > 0) {
        char inChar = (char)Serial.read();
        if (inChar == '\n') {
            if (inputBuffer.length() > 0) {
                // Copier dans _bufferIn
                strncpy(_bufferIn, inputBuffer.c_str(), BUFFER_SIZE - 1);
                _bufferIn[BUFFER_SIZE - 1] = '\0';

                _processMessage(false); // Utilise la même logique que UDP

#ifdef LOG
                Serial.printf("\n[DEBUG] Injected: %s\n", _bufferIn);
#endif
                inputBuffer = "";
            }
        } else {
            Serial.print(inChar);
            inputBuffer += inChar;
        }
    }
}
#endif
