#ifdef Flag

// make some custom characters:
byte Full[8] = {0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111};
byte Empty[8] = {0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111};
byte Flag_Empty[8] = {0b11111, 0b10001, 0b10001, 0b11111, 0b10000, 0b10000, 0b10000, 0b10000};
byte Flag_Full[8] = {0b11111, 0b11111, 0b11111, 0b11111, 0b10000, 0b10000, 0b10000, 0b10000};


void InitFlag() {
  // Create custom CHAR
  lcd.createChar(0, Flag_Empty);
  lcd.createChar(1, Flag_Full);
  lcd.createChar(2, Full);   // create a full char
  lcd.createChar(3, Empty);
}

// Timmer for blink
Ticker Time_Flag;
char _flagColor= '-';
bool _moveToBLU = false;
bool _moveToRED = false;
bool _moveToNeutre = false;
int _PosFlag = 0;

// Set config of flag (Startup)
//     c:   'R' Set flag to RED team
//          'B' Set flag to BLU team
//          'N' Set flag to no team
void SetFlag(char Id, char c) {

  // Save party Id
  Party = Id;
 
  // Send status of parameter receive
  UDP.beginPacket(Dest, 8888);
  UDP.write("PFG;");
  UDP.write(Party);
  UDP.endPacket();
  
  // Chagement de la couleur
  switch (c) {

    case 'R':    // Flag setted to RED team
      Take_RED();
      break;
      
    case 'B':    // Flag setted to BLU team
      Take_BLU();
      break;
      
    case 'N':    // Flag setted to free
      Take_FREE();
      break;
  }


  // Reset movement
  _moveToBLU = false;
  _moveToRED = false;
  _moveToNeutre = false;

}


// Move order of flag.
//     c:   'R' Move flag to RED side
//          'B' Move flag to BLU side
//          'S' Stop flag movement
//          'N' Move to neutral position
void MoveFlag(char c) {

  // Chagement de la couleur
  switch (c) {

    // Move flag to RED team
    case 'R':   
      // Check if flag is already RED
      if (_flagColor == 'R' and _PosFlag <= -16) {
        // If flag is RED, trigger an event "Flag RED is taken"
        UDP.beginPacket(Dest, 8888);
        UDP.write("SFG;R;T");
        UDP.endPacket();
        
        // Reset order
        _moveToBLU = false;
        _moveToRED = false;
        _moveToNeutre = false;
      }
      else {
        // Set order
        _moveToBLU = false;
        _moveToRED = true;
        _moveToNeutre = false;

        // Run ticker
        Time_Flag.attach(0.5, Flag_Tick);
        
        // Write change on LCD
        lcd.setCursor(6,1);
        lcd.print("<<<<");    

        if (_flagColor == 'B') {
          // If flag is BLU, trigger an event "Flag BLU goes down"
          UDP.beginPacket(Dest, 8888);
          UDP.write("SFG;B;D");
          UDP.endPacket();
        }
        if (_flagColor == 'R') {
          // If flag is RED, trigger an event "Flag RED goes up"
          UDP.beginPacket(Dest, 8888);
          UDP.write("SFG;R;U");
          UDP.endPacket();          
        }
        if (_flagColor == 'N') {
          // If flag is FREE, trigger an event "Flag RED goes up"
          UDP.beginPacket(Dest, 8888);
          UDP.write("SFG;N;R");
          UDP.endPacket();          
        }
      }
      break;

    // Move flag to BLU team
    case 'B':
      // Check if flag is already BLU
      if (_flagColor == 'B' and _PosFlag >= 16) {
        // If flag is BLU, trigger an event "Flag BLU is taken"
        UDP.beginPacket(Dest, 8888);
        UDP.write("SFG;B;T");
        UDP.endPacket();
        
        // Reset order
        _moveToBLU = false;
        _moveToRED = false;
        _moveToNeutre = false;
      }
      else {
        // Set order
        _moveToBLU = true;
        _moveToRED = false;
        _moveToNeutre = false;

        // Run ticker
        Time_Flag.attach(0.5, Flag_Tick);
        
        // Write change on LCD
        lcd.setCursor(6,1);
        lcd.print(">>>>");    

        if (_flagColor == 'R') {
          // If flag is RED, trigger an event "Flag RED goes down"
          UDP.beginPacket(Dest, 8888);
          UDP.write("SFG;R;D");
          UDP.endPacket();
        }
        if (_flagColor == 'B') {
          // If flag is BLU, trigger an event "Flag BLU goes up"
          UDP.beginPacket(Dest, 8888);
          UDP.write("SFG;B;U");
          UDP.endPacket();          
        }
        if (_flagColor == 'N') {
          // If flag is FREE, trigger an event "Flag BLU goes up"
          UDP.beginPacket(Dest, 8888);
          UDP.write("SFG;N;B");
          UDP.endPacket();          
        }
      }
      break;

    // Stop flag
    case 'S':
      // Set order
      _moveToBLU = false;
      _moveToRED = false;
      _moveToNeutre = false;

      // Stop ticker
      Time_Flag.detach();
        
      // Write change on LCD
      lcd.setCursor(6,1);
      lcd.print("    ");    

      // Trigger an event "Flag Stop"
      UDP.beginPacket(Dest, 8888);
      UDP.write("SFG;");
      UDP.write(_flagColor);
      UDP.write(";S");
      UDP.endPacket();
      break; 
  }
}

void Flag_Tick(){

  // If move to BLU, increase position
  if (_moveToBLU){
    _PosFlag = _PosFlag + 1;
  }
  
  // If move to RED, increase position
  if (_moveToRED){
    _PosFlag = _PosFlag - 1;
  }

  #ifdef DEBUG
  Serial.println("");
  Serial.print("Indice drapeau : ");
  Serial.println(_PosFlag);
  #endif 
  
  // Check position of flag
  if (_PosFlag < -15){              // Flag taken by RED
   
    Take_RED();                           // Flag taken by RED      
    Time_Flag.detach();                   // Stop ticker
    _moveToBLU = false;
    _moveToRED = false;
    _moveToNeutre = false;
    
  } else if (_PosFlag < 0){         // Flag move between RED and FREE
    
    lcd.setCursor(0, 0);
    for (int i=-16; i <= 0; i++){
      if (i < _PosFlag){
        lcd.write(byte(3));
      } else {
        lcd.write(byte(2));
      }
    }

  } else if (_PosFlag == 0){          // Flag taken by NULL
    
    Take_FREE();                        // Flag move to red RED
    
    if (_moveToBLU){                    // If flag move to BLU, trigger an event "Flag BLU goes up"    
      UDP.beginPacket(Dest, 8888);
      UDP.write("SFG;N;B");
      UDP.endPacket();
    }   
    if (_moveToRED){                    // If flag move to RED, trigger an event "Flag RED goes up"
      UDP.beginPacket(Dest, 8888);
      UDP.write("SFG;N;R");
      UDP.endPacket();
    }
    
  } else if (_PosFlag < 16){         // Flag move between BLU and FREE
    
    lcd.setCursor(0, 0);
    for (int i=1; i <= 16; i++){
      if (i <= _PosFlag){
        lcd.write(byte(2));
      } else {
        lcd.write(byte(3));
      }
    }    
    
  } else {                            // Flag taken by BLU
   
    Take_BLU();                           // Flag taken by BLU      
    Time_Flag.detach();                   // Stop ticker
    _moveToBLU = false;
    _moveToRED = false;
    _moveToNeutre = false;
  
  }
  
}


// When RED flag is taken
void Take_RED(){
  
  // Change background LCD
  LCD_Color('R');

  // Set full flag to line 1
  lcd.setCursor(0, 0);
  for (int i=1; i <= 16; i++){
    lcd.write(byte(2));
  }

  // Set texte to line 2
  lcd.setCursor(0,1);
  lcd.write((byte)1);
  lcd.print(":RED      BLU:");
  lcd.write((byte)0);

  // Save flage color
  _flagColor = 'R';
  
  // Save position of flag
  _PosFlag = -16;

  // Trigger an event "Flag RED is taken"
  UDP.beginPacket(Dest, 8888);
  UDP.write("SFG;R;T");
  UDP.endPacket();
}

// When BLU flag is taken
void Take_BLU(){
  
  // Change background LCD
  LCD_Color('T');

  // Set full flag to line 1
  lcd.setCursor(0, 0);
  for (int i=1; i <= 16; i++){
    lcd.write(byte(2));
  }

  // Set texte to line 2
  lcd.setCursor(0,1);
  lcd.write((byte)0);
  lcd.print(":RED      BLU:");
  lcd.write((byte)1);
  
  // Save flage color
  _flagColor = 'B';
  
  // Save position of flag
  _PosFlag = 16;
  
  // Trigger an event "Flag BLU is taken"
  UDP.beginPacket(Dest, 8888);
  UDP.write("SFG;B;T");
  UDP.endPacket();
}

void Take_FREE(){
  // Change background LCD
  LCD_Color('G');

  // Set full flag to line 1
  lcd.setCursor(0, 0);
  for (int i=1; i <= 16; i++){
    lcd.write(byte(3));
  }

  // Set texte to line 2
  lcd.setCursor(0,1);
  lcd.write((byte)0);
  lcd.print(":RED      BLU:");
  lcd.write((byte)0);

  // Write ">>>>" animation if flag move
  if (_moveToBLU){ 
    // Write change on LCD
    lcd.setCursor(6,1);
    lcd.print(">>>>");   
  }   
  if (_moveToRED){
    // Write change on LCD
    lcd.setCursor(6,1);
    lcd.print("<<<<");    
  }

  // Save flage color
  _flagColor = 'N';
  
  // Save position of flag
  _PosFlag = 0;
  
  // Trigger an event "Flag Neutre is taken"
  UDP.beginPacket(Dest, 8888);
  UDP.write("SFG;N;T");
  UDP.endPacket();
}
#endif