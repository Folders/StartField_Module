#include "Type/T_Base.h"

#ifdef BASE

char _team;
char _type;
char _spawn;

Ticker _ticker;

void SetBase(char Id, char Team, char Type, char Spawn)
{
    // Save party Id
    party = Id;

    // Save party parameters
    _team = Team;
    _type = Type;
    _spawn= Spawn;              
    
    #ifdef LOG
    Serial.println("");
    Serial.print("Set base - Id ");
    Serial.print(Id);
    Serial.print(", Team ");
    Serial.print(Team);
    Serial.print(", Type ");
    Serial.print(Type);
    Serial.print(", Spawn ");
    Serial.println(Spawn);
    #endif

    // Send status of parameter receive
    char msg[] = "PST;X";   // 8 octets, indices 0..7 + '\0'
    msg[4] = Id;           // remplace X
    comm.send(msg);
}



Ticker _TimeRespawn;            // Respawn timer
int _minRespawnBackup = 0;      // Time of respawn setted to module (Min)
int _secRespawnBackup = 0;      // Time of respawn setted to module (Sec)
int _minRespawn = 0;            // Time left befor respawn (Min)
int _secRespawn = 0;            // Time left befor respawn (Sec)
bool _lockRespawn = false;      // Lock time update when bomb is ON
bool _setRespawn = false;       // Bomb is setted



void SetBaseTime(const char Min[], const char Sec[])
{
    // Get time of respawn
    _minRespawnBackup = atoi(Min);
    _secRespawnBackup = atoi(Sec);

    // And put it as current time
    _minRespawn = _minRespawnBackup;
    _secRespawn = _secRespawnBackup;

    // Create ticker
    _TimeRespawn.attach(1, Respawn_Tick);
    _setRespawn = true;

    #ifdef LOG
    Serial.println("");
    Serial.print("Set respown time to ");
    Serial.print(Min);
    Serial.print(":");
    Serial.println(Sec);
    #endif
}


void ResetBaseTime()
{
    // Get time of respawn
    _minRespawn = 0;
    _secRespawn = 0;

    // Destroy ticker
    _TimeRespawn.detach();
    _setRespawn = false;
}



 void Respawn_Tick()
 {


  // Update time
    if (_secRespawn > 0){
        _secRespawn = _secRespawn - 1;
    }
    else{
        if (_minRespawn > 0){
        _minRespawn = _minRespawn - 1;
        _secRespawn = 59;
    }
    else{
    // Restart respawn timer with backup value
    _minRespawn = _minRespawnBackup;
    _secRespawn = _secRespawnBackup;
    }    
    }


    // Update time
    if (!lcd.AsPopup())
    {
        // Set text
        lcd.setCursor(5,1);

        char msg[6];
        snprintf(msg, sizeof(msg), "%02u:%02u", (unsigned)_minRespawn, (unsigned)_secRespawn);
        lcd.print(msg);

        #ifdef LOG
        Serial.print("Tick with ");
        Serial.println(msg);
        #endif
    }
  }

  #endif