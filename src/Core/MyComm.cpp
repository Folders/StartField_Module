#include "Core/MyComm.h"
#define SOH 0x01
#define EOT 0x04


MyComm::MyComm()
    : _deviceID(0), _featureCount(0), _udpPort(0),
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

void MyComm::begin(uint16_t udpPort) {
    _udpPort = udpPort;
    _udp.begin(_udpPort);

#ifdef LOG
    Serial.printf("UDP started on %d\n", _udpPort);
#endif
}

void MyComm::handle() {

    if (!_connected)
    {
        unsigned long now = millis();

        if (now - _lastHeartbeat >= _heartbeatInterval) {
            _sendBOT();
            _lastHeartbeat = now;
        }
    }

    _newCommand = false;

    // Check if new paquet is receice
    _handleUDP();

    
    if (_queueStart != _queueEnd) {
        const char* cmd = _commandQueue[_queueStart];
        _queueStart = (_queueStart + 1) % MAX_QUEUE;

#ifdef LOG
        //Serial.print("Command : ");
        //Serial.println(cmd);
#endif

        _processMessage((char*)cmd);  // ou false si nécessaire
    }
}

/// @brief Check if UDP receive packet.
void MyComm::_handleUDP() {

    int packetSize = _udp.parsePacket();
    if (packetSize <= 0) return;

    // Save in buffer the received text
    int len = _udp.read(_bufferIn, BUFFER_SIZE - 1);
    if (len <= 0) return;

    _bufferIn[len] = '\0';

#ifdef LOG
    Serial.print("[COMM] <- <SOH>");
    Serial.print(_bufferIn);
    Serial.println("<EOT>");
#endif

    _queueMessage();
}


/// @brief Separate commands if a message contain more than one
void MyComm::_queueMessage() {
    for (uint16_t i = 0; i < strlen(_bufferIn); i++) {
        char c = _bufferIn[i];

        if (c == SOH) {
            // reset frame
            _rxFrameLen = 0;
        }
        else if (c == EOT) {
            // fin de trame → push queue
            if (_rxFrameLen > 0 && _rxFrameLen < MAX_CMD_LEN) {
                uint8_t next = (_queueEnd + 1) % MAX_QUEUE;

                if (next != _queueStart) {
                    memcpy(_commandQueue[_queueEnd], _rxFrameBuffer, _rxFrameLen);
                    _commandQueue[_queueEnd][_rxFrameLen] = '\0';
                    _queueEnd = next;

#ifdef LOG
                    //Serial.print("[COMM] In queue: ");
                    //Serial.println(_commandQueue[(_queueEnd + MAX_QUEUE - 1) % MAX_QUEUE]);
#endif
                }
#ifdef LOG
                else {
                    Serial.println("[COMM] Queue full, message lost");
                }
#endif
            }

            // reset pour prochaine trame
            _rxFrameLen = 0;
        }
        else {
            // accumulation
            if (_rxFrameLen < BUFFER_SIZE - 1) {
                _rxFrameBuffer[_rxFrameLen++] = c;
            }
        }
    }
}

void MyComm::_processMessage(char* msg) {
    char* token = strtok(msg, ";");
    if (!token) return;

    // MessageId
    strncpy(_messageId, token, sizeof(_messageId) - 1);
    _messageId[sizeof(_messageId) - 1] = '\0';

    // Code
    token = strtok(nullptr, ";");
    if (!token) return;

    strncpy(_code, token, sizeof(_code) - 1);
    _code[sizeof(_code) - 1] = '\0';

    // Reset args count for every new message
    _paramCount = 0;

   //////////////   Internal command   //////////////
    if (strcmp(_code, "PIG") == 0) {
        _sendACK();
    }
    else if (strcmp(_code, "LVL") == 0) {
        char resp[32];
        snprintf(resp, sizeof(resp), "LVL;%d", WiFi.RSSI());
        _sendRaw(resp, false, _messageId);
    }
    else if (strcmp(_code, "RBT") == 0) {
        _sendACK();
        _sendBOT();

#ifdef LOG
        Serial.println("[COMM] RBT reçu en UDP → Send BOT");
#endif
    }
    else if (strcmp(_code, "BVN") == 0) {
        // Save the serveur IP adresse
        _serverIP = _udp.remoteIP();
        _connected = true;
        
        _sendACK();
#ifdef LOG
        Serial.println("[COMM] BVN reçu en UDP → TCP READY");
#endif
    }
    else
    {
        // Arguments
        _paramCount = 0;
        while ((token = strtok(nullptr, ";")) != nullptr && _paramCount < MAX_PARAMS) {
            _params[_paramCount++] = token;
        }

        _newCommand = true;

        _sendACK();
    }
    
}


void MyComm::_sendBOT() {
    char payload[BUFFER_SIZE];
    int len = snprintf(payload, sizeof(payload), "BOT;%u", _deviceID);
    if (len < 0 || len >= (int)sizeof(payload)) return;

    for (uint8_t i = 0; i < _featureCount; i++) {
        len += snprintf(payload + len, sizeof(payload) - len, ";%s", _features[i]);
        if (len >= (int)sizeof(payload)) return;
    }

    _sendRaw(payload, true, "---");
}
void MyComm::send(const char* msg) {
    _sendRaw(msg, false, "---");
}

void MyComm::_sendRaw(const char* payload, bool forceBroadcast, const char* messageId) {
    if (!payload || payload[0] == '\0') return;

    const char* id = (messageId && messageId[0] != '\0') ? messageId : "---";

    int len = snprintf(_bufferOut + 1, BUFFER_SIZE - 3, "%s;%s", id, payload);
    if (len <= 0 || len >= BUFFER_SIZE - 3) return;

    _bufferOut[0] = SOH;
    _bufferOut[1 + len] = EOT;

    size_t totalLen = len + 2;


#ifdef LOG
    Serial.print("[COMM] -> <SOH>");
    Serial.print(payload);
    Serial.println("<EOT>");
#endif

    // Send datas on UDP
    if (forceBroadcast || !_serverIP) {
        _udp.beginPacket(IPAddress(255, 255, 255, 255), _udpPort);
    } else {
        _udp.beginPacket(_serverIP, _udpPort);
    }
    _udp.write((const uint8_t*)_bufferOut, totalLen);
    _udp.endPacket();
}

void MyComm::_sendACK() {
    if (_messageId[0] == '\0' || _code[0] == '\0') return;

    char ack[64];
    snprintf(ack, sizeof(ack), "ACK;%s", _code);
    _sendRaw(ack, false, _messageId);
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

                _processMessage(_bufferIn); // Utilise la même logique que UDP

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
