#include <Arduino.h>
#include <common.h>

// Basic needed library
#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
#include <WiFiUDP.h>
#include <Ticker.h>

// Define all system - The used must me define in common.h
//#include <Breach.h>


////////////////////////

/// @brief Communication object
MyComm comm;

/// @brief Serial input string buffer
String InputBuffer;

/// @brief Send more information to the server
bool debug = false;
//bool debug = true;

// Wifi
#include <secret.h>
boolean wifiConnected = false;
boolean masterConnected = false;

///////////////////////////////////  DO NOT TOUCH UNDER THIS  ///////////////////////////////////////////////

char Party = 'n';

// UDP
IPAddress Dest;
unsigned int PORT = 8888;
WiFiUDP UDP;
int len;
boolean udpConnected = false;
char packetBuffer[8192]; // buffer to hold incoming packet,
String str;

// Timmer
Ticker Time_Sec;

///////////////////////////////////////  Wifi function  ///////////////////////////////////////
#pragma region "Wifi function"


/// @brief Connect to WiFi
/// @return True if successful or false if not
boolean connectWifi()
{
    boolean state = true;
    int i = 0;
    WiFi.begin(ssid, password);

#ifdef LOG
    Serial.println("");
    Serial.println("Connecting to WiFi");
#endif

// Wait for connection
#ifdef LOG
    Serial.print("Connecting");
    Serial.print("Connecting");
#endif

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);

#ifdef LOG
        Serial.print(".");
#endif

        if (i > 10)
        {
            state = false;
            break;
        }
        i++;
    }

#ifdef LOG
    if (state)
    {
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("");
        Serial.println("Connection failed.");
    }
#endif

    // Save current IP as destination
    Dest = WiFi.localIP();

    // Change IP adresse to broadcast
    Dest[3] = 255;

    return state;
}


/// @brief Create UDP server
/// @return True if successful or False if not
boolean connectUDP()
{
    boolean state = false;

#ifdef LOG
    Serial.println("");
    Serial.println("Starting UDP server...");
#endif

    // Check if UDP is open
    if (UDP.begin(PORT) == 1)
    {
        state = true;
    }

#ifdef LOG
    if (state)
    {
        Serial.println("UDP running.");
    }
    else
    {
        Serial.println("Fail to open UDP server.");
    }
#endif

    return state;
}


/// @brief Send broadcast to try to connect to server
void BOTParam()
{
    comm.start("BOT;");
    comm.add(NUMBER);

    
    #ifdef BTN_R 
    comm.add(";BR");
    #endif
    #ifdef BTN_B
    comm.add(";BB");
    #endif
    #ifdef BTN_O
    comm.add(";BO");
    #endif

    #ifdef LED_R 
    comm.add(";LR");
    #endif
    #ifdef LED_B
    comm.add(";LB");
    #endif

    #ifdef Flash_R
    comm.add(";FR");
    #endif
    #ifdef Flash_B
    comm.add(";FB");
    #endif
    
    #ifdef Buzzer
    comm.add(";BUZ");
    #endif
    

    #ifdef LCD
    comm.add(";LCD");
    #endif


    comm.sendForced();
}


/// @brief Every seconde, try to reach server
void T_1s()
{

    if (!masterConnected)
    {
        // Send start
        BOTParam();
    }
}


/// @brief Get current wifi level
void WifiLevel()
{

#ifdef LOG
    Serial.println("");
    Serial.print("Wifi level : ");
    Serial.println(WiFi.RSSI());
#endif

    char mystr[7];
    sprintf(mystr, "%6d", WiFi.RSSI());

    comm.start("LVL;");
    comm.add(mystr);
    comm.send();
}

#pragma endregion



///////////////////////////////////////  Wifi function  ///////////////////////////////////////
#pragma region "Wifi function"


// Reset all proprety of module
void ResetModule()
{

  // Reset party ID
  Party = 'n';
  PopUp = false;
  
  // Reset timer of respawn
  ResetBaseTime();

  // Reset timer of bomb
  ResetBomb();


  #ifdef BTN_R
  SetLed('R', '0');
  #endif

  #ifdef BTN_B
  SetLed('B', '0');
  #endif
}


#pragma endregion

////////////////////////////////////////   SETUP   ////////////////////////////////////////
void setup()
{

#ifdef LOG
    // Initialise Serial connection
    Serial.begin(115200);
#endif


    // Wait until wifi is connected
    do
    {
        wifiConnected = connectWifi();
    } while (wifiConnected == false);

    // Create UDP server
    udpConnected = connectUDP();

    // Run timmer to try to connect Unity server
    Time_Sec.attach(1, T_1s);

    // Put module in reset state
    ResetModule();
}


/////////////////////////////////////////   LOOP   ////////////////////////////////////////

/// @brief asfdasfdsadf
void loop()
{

#ifdef LOG
    // Check if text is available on serial port
    if (Serial.available() > 0)
    {
        // Read the received char
        char inChar = (char)Serial.read();

        // Check if it's an "Enter" char
        if (inChar == '\n')
        {
            // Save received string, and test if valide
            if (comm.ReceiveIntern(InputBuffer))
            {
                // Call received function
                comm.callReceiveFunction();
            }

            // Clean received string buffer
            InputBuffer.clear();
        }
        else
        {
            // Userfriendly: Show the current char writed
            Serial.print(inChar);

            // Sinon, ajoute le caractère entrant à la chaîne d'entrée
            InputBuffer += inChar;
        }
    }
#endif

    // check if the WiFi and UDP connections were successful
    if (wifiConnected and udpConnected)
    {

        // if there’s data available, read a packet
        int packetSize = UDP.parsePacket();
        if (packetSize)
        {

            // read the packet into packetBufffer
            int len = UDP.read(packetBuffer, packetSize);

            // and put a null char at the end of the string
            if (len > 0)
            {
                packetBuffer[len] = 0;
            }

            // Test si message de réception du master

            str = String(packetBuffer);

            // Check if connection to Unity server is done
            if (masterConnected)
            {

                // Parse received datas and check if it's for us
                if (comm.Receive(str))
                {

                    // Convert code in string
                    str = comm.GetCode();

                    // Server sak for BOT info
                    if (str == "RBT")
                    {
                        BOTParam();
                    }

                    // Server ask for a reset of module
                    else if (str == "RST")
                    {
                        ResetModule();
                    }

                    // Server ask for a PING request
                    else if (str == "PIG")
                    {
                        comm.sendForced("POG");
                    }

                    // Return wifi quality
                    else if (str == "LVL")
                    {
                        WifiLevel();
                    }

                    // But module in debug mode
                    else if (str == "DBG")
                    {
                        switch (comm.GetParameter(1)[0])
                        {
                        case '0':
                            debug = false;
                            break;

                        case '1':
                            debug = true;
                            break;

                        case '?':
                            if (debug)
                                comm.send("DBG;1");
                            else

                                comm.send("DBG;0");
                            break;
                        }
                    }
                }
            }
            else
            {
                // If we are not connected to unity, we wait for a welcom message
                if (str == "Y;BVN")
                {
                    // Timer
                    Time_Sec.detach();

                    masterConnected = true;
                    Dest = UDP.remoteIP();

                    comm.Started();

#ifdef LOG
                    Serial.println("");
                    Serial.println("Connected to server : ");
                    Serial.println(Dest);
#endif
                }
            }
        }

        delay(10);
    }

}
