#include "common.h"

#include <Arduino.h>

// Basic needed library
#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include "Core/MyConfig.h"
#include "core/MyComm.h"
/// @brief Communication objectPourquo
MyComm comm;

#include "Peripherals/M_LCD.h"
M_LCD lcd;

#include "Peripherals/M_Button.h"
M_Button btnRed('R');
M_Button btnBlu('B');
M_Button btnOrg('O');

#include "Peripherals/M_Flash.h"
M_Flash flRed('R');
M_Flash flBlu('B');

#include "Peripherals/M_Buzzer.h"
M_Buzzer buzzer;

////////////////////////

char party = 'n';

/// @brief Serial input string buffer
String InputBuffer;

/// @brief Send more information to the server
bool debug = false;

// Wifi
#include <secret.h>
boolean wifiConnected = false;

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

    return state;
}

///////////////////////////////////////  Reset function  ///////////////////////////////////////
#pragma region "Reset function"

// Reset all proprety of module
void ResetModule()
{

    // Reset party ID
    // Party = 'n';
    // PopUp = false;

    // Reset timer of respawn
    // ResetBaseTime();

    ResetFlag();

    // Reset timer of bomb
    ResetBomb();

#ifdef BTN_R
    btnRed.setLed('0');
#endif

#ifdef BTN_B
    btnBlu.setLed('0');
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

    // Starting LCD
    lcd.begin();
    lcd.Write_Msg("Try to connect:", ssid, 'W');

    // Starting button
    btnRed.begin(D3);
    btnBlu.begin(D4);
    btnOrg.begin(D5);

    // Starting flash lamp
    flRed.begin(D6);
    flBlu.begin(D7);

    // Start buzzer
    buzzer.begin(D5);

    // Disabled sleep mode
    WiFi.setSleep(false); 

    // Wait until wifi is connected
    do
    {
        wifiConnected = connectWifi();
    } while (wifiConnected == false);

    char line1[17];
    char line2[17];

    // Remplir avec des espaces
    memset(line1, ' ', 16);
    memset(line2, ' ', 16);
    line1[16] = '\0';
    line2[16] = '\0';

     // Get module ID
    MyConfig::begin();
    uint16_t gModuleId = MyConfig::getModuleId(); // 0 si non provisionné
    comm.setID(gModuleId);
    #ifdef LOG
        Serial.println("");
        Serial.print("Read ID : ");
        Serial.println(gModuleId);
    #endif

    // Ligne 1 : "Module n°X"
    snprintf(line1, sizeof(line1), " - Module %03d - ", gModuleId);

    // Ligne 2 : IP (WiFi.localIP())
    IPAddress ip = WiFi.localIP();
    snprintf(line2, sizeof(line2), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    // Envoyer au LCD
    lcd.Write_Msg(line1, line2, 'W');



// Ajout des features selon #define
#ifdef LCD
    comm.addFeature("LCD");
#endif
#ifdef BTN_R
    comm.addFeature("BR");
#endif
#ifdef BTN_B
    comm.addFeature("BB");
#endif
#ifdef BTN_O
    comm.addFeature("BO");
#endif
#ifdef LED_R
    comm.addFeature("LR");
#endif
#ifdef LED_B
    comm.addFeature("LB");
#endif
    comm.begin(8888, 9999); // UDP port + TCP port

#ifdef FLAG
    InitFlag();
#endif

#ifdef BOMB
    InitBomb();
#endif

    // Put module in reset state
    ResetModule();
}

/////////////////////////////////////////   LOOP   ////////////////////////////////////////

/// @brief asfdasfdsadf
void loop()
{
    // Get message
    comm.handle();

#ifdef LOG
    comm.handleSerialDebug();
#endif

    // check if the WiFi and UDP connections were successful
    if (wifiConnected)
    {

        if (comm.hasNewCommand())
        {
            const char *cmd = comm.GetCode();

#ifdef LOG
            Serial.println("");
            Serial.print("Command : ");
            Serial.println(comm.GetCode());
            Serial.print("Param 1 : ");
            Serial.println(comm.GetParameter(0));
            Serial.print("Param 2 : ");
            Serial.println(comm.GetParameter(1));
            Serial.print("Param 3 : ");
            Serial.println(comm.GetParameter(2));
            Serial.print("Param 4 : ");
            Serial.println(comm.GetParameter(3));
            Serial.print("Param 5 : ");
            Serial.println(comm.GetParameter(4));
#endif

#ifdef LCD
            if (strcmp(cmd, "TXT") == 0 && comm.GetSize() >= 3)
            {
                lcd.Write_Msg(comm.GetParameter(0), comm.GetParameter(1), comm.GetParameter(2)[0]);
            }
            else if (strcmp(cmd, "POP") == 0 && comm.GetSize() >= 4)
            {
                lcd.Write_Pop(comm.GetParameter(0), comm.GetParameter(1), comm.GetParameter(2)[0], comm.GetParameter(3));
            }
            else if (strcmp(cmd, "CLR") == 0)
            {
                lcd.SetColor(comm.GetParameter(1)[0]);
            }
#endif

            // Check for led update
            if (strcmp(cmd, "LED") == 0 && comm.GetSize() == 2)
            {
                switch (comm.GetParameter(0)[0])
                {
                case 'R': // Rouge
                    btnRed.setLed(comm.GetParameter(1)[0]);
                    break;

                case 'B': // Blue
                    btnBlu.setLed(comm.GetParameter(1)[0]);
                    break;
                    break;
                }
            }

            // Check for flash lamp update
            if (strcmp(cmd, "FLH") == 0 && comm.GetSize() == 2)
            {
                switch (comm.GetParameter(0)[0])
                {
                case 'R': // Rouge
                    flRed.setFlash(comm.GetParameter(1)[0]);
                    break;

                case 'B': // Blue
                    flBlu.setFlash(comm.GetParameter(1)[0]);
                    break;
                    break;
                }
            }

            // Check for led update
            if (strcmp(cmd, "BUZ") == 0 && comm.GetSize() == 1)
                buzzer.buzz(comm.GetParameter(0));

            // Check for led update
            if (strcmp(cmd, "SID") == 0 && comm.GetSize() == 1)
            {
                long newId = strtol(comm.GetParameter(0), nullptr, 10);
                if (newId >= 1 && newId <= 65535)
                {
                    if (MyConfig::setModuleId((uint16_t)newId))
                    {
                        char resp[32];
                        snprintf(resp, sizeof(resp), "SID;OK;%ld", newId);
                        comm.send(resp);

                        // Applique de suite à la comm
                        comm.setID((uint16_t)newId);

                        // Optionnel : reboot pour repartir propre
                        delay(300);
                        ESP.restart();
                    }
                    else
                    {
                        comm.send("SID;ERR;SAVE");
                    }
                }
                else
                {
                    comm.send("SID;ERR;RANGE");
                }
            }

#ifdef BASE
            // Paramètres de la base    "Id;Team;Type;Spawn"
            if (strcmp(cmd, "PST") == 0 && comm.GetSize() == 4)
            {
                SetBase(comm.GetParameter(0)[0], comm.GetParameter(1)[0], comm.GetParameter(2)[0], comm.GetParameter(3)[0]);
            }

            // Set timer for respawn
            if (strcmp(cmd, "STM") == 0 && comm.GetSize() == 2)
            {
                SetBaseTime(comm.GetParameter(0), comm.GetParameter(1));
            }

            // Reset timer for respawn
            if (strcmp(cmd, "RTM") == 0 && comm.GetSize() == 0)
            {
                ResetBaseTime();
            }
#endif

#ifdef SAFE

            // Paramètres de la base    "Id;Team;Type;Spawn"
            if (strcmp(cmd, "PSZ") == 0 && comm.GetSize() == 1)
            {
                SetSafe(comm.GetParameter(0)[0]);
            }
#endif

#ifdef FLAG

            // Set timer for respawn
            if (strcmp(cmd, "PFG") == 0 && comm.GetSize() == 3)
            {
                SetFlag(comm.GetParameter(0)[0], comm.GetParameter(1)[0]);
            }

            // Reset timer for respawn
            if (strcmp(cmd, "FLG") == 0 && comm.GetSize() == 1)
            {
                MoveFlag(comm.GetParameter(0)[0]);
            }
#endif

#ifdef BOMB

            // Init flag to a team
            if (strcmp(cmd, "PBO") == 0 && comm.GetSize() == 3)
            {
                SetBomb(comm.GetParameter(0)[0], comm.GetParameter(1)[0], comm.GetParameter(2));
            }

            // Set password
            if (strcmp(cmd, "PSW") == 0 && comm.GetSize() == 1)
            {
                SetPassword(comm.GetParameter(0));
            }

            // Start bomb
            if (strcmp(cmd, "BST") == 0 && comm.GetSize() == 3)
            {
                BombPlanted(comm.GetParameter(0), comm.GetParameter(1), comm.GetParameter(2));
            }

            // Set defuse code
            if (strcmp(cmd, "DIF") == 0 && comm.GetSize() == 1)
            {
                SetDefuse(comm.GetParameter(0));
            }

            // Set defuse code
            if (strcmp(cmd, "BRS") == 0 && comm.GetSize() == 0)
            {
                ResetDefuse();
            }

            // Set defuse code
            if (strcmp(cmd, "BED") == 0 && comm.GetSize() == 0)
            {
                ResetBomb();
            }
#endif
        }

        // Update button
        btnBlu.readButton();
        btnRed.readButton();
    }

    yield(); // ou delay(0);
}
