
void SetBase(char Id, char Team, char Type, char Spawn){

  // Save party Id
  Party = Id;
  
  // Sauvegarde des paramètres           
  B_Team = Team;
  B_Type = Type;
  B_Spawn= Spawn;              

  // Confirmation de la réception de paramètres
  UDP.beginPacket(Dest, 8888);
  UDP.write("PST;");
  UDP.write(Id);
  UDP.endPacket();

}



Ticker _TimeRespawn;            // Respawn timer
int _minRespawnBackup = 0;      // Time of respawn setted to module (Min)
int _secRespawnBackup = 0;      // Time of respawn setted to module (Sec)
int _minRespawn = 0;            // Time left befor respawn (Min)
int _secRespawn = 0;            // Time left befor respawn (Sec)
bool _lockRespawn = false;      // Lock time update when bomb is ON
bool _setRespawn = false;       // Bomb is setted



void SetBaseTime(char Min[], char Sec[]){

  // Get time of respawn
  _minRespawnBackup = atoi(Min);
  _secRespawnBackup = atoi(Sec);

  // And put it as current time
  _minRespawn = _minRespawnBackup;
  _secRespawn = _secRespawnBackup;
  
  // Create ticker
  _TimeRespawn.attach(1, Respawn_Tick);
  _setRespawn = true;
}


void ResetBaseTime(){

  // Get time of respawn
  _minRespawn = 0;
  _secRespawn = 0;
  
  // Destroy ticker
  _TimeRespawn.detach();
  _setRespawn = false;
}



 void Respawn_Tick(){

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
  if (!PopUp){

    // Set text
    lcd.setCursor(5,1);

    lcd.print(_minRespawn / 10);
    lcd.print(_minRespawn % 10);
    lcd.print(':');
    lcd.print(_secRespawn / 10);
    lcd.print(_secRespawn % 10);
    
    }
  }