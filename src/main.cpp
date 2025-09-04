#include <Arduino.h>
#include <common.h>

// Basic needed library
#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include "core/MyComm.h"


#include "Peripherals/M_LCD.h"
M_LCD lcd;


#include "Peripherals/M_Button.h"
M_Button btnRed('R');
M_Button btnBlu('B');
M_Button btnOrg('O');

#include "Peripherals/M_Flash.h"
M_Flash flRed('R');
M_Flash flBlu('B');


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
  //Party = 'n';
  //PopUp = false;
  
  // Reset timer of respawn
  //ResetBaseTime();

  // Reset timer of bomb
  //ResetBomb();


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

    // Ligne 1 : "Module n°X"
    snprintf(line1, sizeof(line1), " - Module %03d - ", NUMBER);

    // Ligne 2 : IP (WiFi.localIP())
    IPAddress ip = WiFi.localIP();
    snprintf(line2, sizeof(line2), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    // Envoyer au LCD
    lcd.Write_Msg(line1, line2, 'W');

    comm.setID(NUMBER);

    // Ajout des features selon #define
    #ifdef LCD
    comm.addFeature("LCD");
    #endif

    comm.begin(8888, 9999); // UDP port + TCP port

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

        if (comm.hasNewCommand()) {
            const char* cmd = comm.GetCode();

            

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
                    case 'R':    // Rouge
                        btnRed.setLed(comm.GetParameter(1)[0]);
                        break;
                    
                    case 'B':	// Blue
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
                    case 'R':    // Rouge
                        flRed.setFlash(comm.GetParameter(1)[0]);
                        break;
                    
                    case 'B':	// Blue
                        flBlu.setFlash(comm.GetParameter(1)[0]);
                        break;
                    break;
                }
            }

        }

        // Update button
        btnBlu.readButton();
        btnRed.readButton();

    }

}                    

