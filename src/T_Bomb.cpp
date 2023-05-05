#ifdef Bomb

// make some custom characters:
byte BluUp[8] = {0b00000, 0b01111, 0b00011, 0b00101, 0b01001, 0b10000, 0b00000, 0b11111};
byte BluDown[8] = {0b00000, 0b10000, 0b01001, 0b00101, 0b00011, 0b01111, 0b00000, 0b11111};
byte RedDown[8] = {0b00000, 0b00001, 0b10010, 0b10100, 0b11000, 0b11110, 0b00000, 0b11111};
byte RedUp[8] = {0b00000, 0b11110, 0b11000, 0b10100, 0b10010, 0b00001, 0b00000, 0b11111};


void InitBomb() {
  // Create custom CHAR
  lcd.createChar(4, BluUp);
  lcd.createChar(5, BluDown);
  lcd.createChar(6, RedDown);
  lcd.createChar(7, RedUp);
}

Ticker _TimeBomb;
char _password[16];
char _bombColor= '-';
int _bombDiff = 0;
int _bombTime = 0;
int _posStart = 0;
int _minBomb = 0;           // Time left of bomb (Min)
int _secBomb = 0;           // Time left of bomb (Sec)
bool _lock = false;     // Lock time update when bomb is ON
bool _bombSet = false;  // Bomb is setted


void SetBomb(char Id, char Team, char Difficulty[]){

  // Save party Id
  Party = Id;
  
  // Sauvegarde des paramètres   
  _bombColor = Team;
  _bombDiff = atoi(Difficulty);    

  // Save pos start
  _posStart = 8 - (_bombDiff/2);
  
  // Confirmation de la réception de paramètres
  UDP.beginPacket(Dest, 8888);
  UDP.write("PBO;");
  UDP.write(Id);
  UDP.endPacket();

}


void SetPassword(char Psw[]){
  
  // Set full flag to line 1
  lcd.setCursor(_posStart, 1);
  for (int i=0; i < _bombDiff; i++){
  
    switch (Psw[i]) {
      case 'b':
        lcd.write(byte(4));
      break;
      
      case 'B':
        lcd.write(byte(5));
      break;
        
      case 'R':
        lcd.write(byte(6));
      break;
      
      case 'r':
        lcd.write(byte(7));
      break;
      
      default:
        lcd.print("_");
      break;
    }

  }
 }

 void BombPlanted(char Psw[], char Min[], char Sec[]){
  
  // Sauvegarde des paramètres   
  memcpy(_password, Psw, 16);
  _minBomb = atoi(Min);
  _secBomb = atoi(Sec);
  
  // Create ticker
  _TimeBomb.attach(1, Bomb_Tick);
  _bombSet = true;
 }

 void Bomb_Tick(){

  // Update time
  if (_secBomb > 0){
    _secBomb = _secBomb - 1;
  }
  else{
    if (_minBomb > 0){
      _minBomb = _minBomb - 1;
      _secBomb = 59;
    }
    else{
      _TimeBomb.detach();
    }    
  }
  
  // Update time
  if (!PopUp && !_lock){

    // Set password
    lcd.setCursor(_posStart,1);
  
    for (int i=0; i < _bombDiff; i++){
    
      switch (_password[i]) {
        case 'b':
          lcd.write(byte(4));
        break;
        
        case 'B':
          lcd.write(byte(5));
        break;
          
        case 'R':
          lcd.write(byte(6));
        break;
        
        case 'r':
          lcd.write(byte(7));
        break;
        
        default:
          lcd.print("_");
        break;
      }
    }
    
    // Set text
    lcd.setCursor(10,0);

    lcd.print(_minBomb / 10);
    lcd.print(_minBomb % 10);
    lcd.print(':');
    lcd.print(_secBomb / 10);
    lcd.print(_secBomb % 10);
    
    }
  }
  
 
void SetDefuse(char Psw[]){


  bool endChar = false;
  
  // Lock update of time
  if (_lock == false){
    _lock = true;
    lcd.setCursor(0, 0);
    lcd.print("                ");
  }
  
  // Set full flag to line 1
  lcd.setCursor(_posStart, 0);
  for (int i=0; i < _bombDiff; i++){

    if (endChar){
      lcd.print("_");
    }
    else{
      
      switch (Psw[i]) {
      case 'b':
        lcd.write(byte(4));
      break;
      
      case 'B':
        lcd.write(byte(5));
      break;
        
      case 'R':
        lcd.write(byte(6));
      break;
      
      case 'r':
        lcd.write(byte(7));
      break;
      
      default:
        lcd.print("_");
        endChar = true;
      break;
    }

    }
  }
 }


void ResetDefuse(){

  // Lock update of time
  _lock = false;
  
 }

 
void ResetPopup(){

  if (_bombSet){
    // Set text
    lcd.setCursor(10,0);

    lcd.print(_minBomb / 10);
    lcd.print(_minBomb % 10);
    lcd.print(':');
    lcd.print(_secBomb / 10);
    lcd.print(_secBomb % 10); 
  }
  
 }

 void ResetBomb(){
  
  // Lock update of time
  _lock = false;
  
  // Lock update of time
  _TimeBomb.detach();
  _bombSet = false;
 }
#endif