#include <common.h>

#ifdef LCD

// These #defines make it easy to set the backlight color
#define C_BLACK 0x0
#define C_RED 0x1
#define C_YELLOW 0x3
#define C_GREEN 0x2
#define C_TEAL 0x6
#define C_BLUE 0x4
#define C_VIOLET 0x5
#define C_WHITE 0x7

char Line1_backup[16];
char Line2_backup[16];
char Color_backup;

// Timmer
Ticker PopupEnd;

void Clear(char c[]){
  char * ptr = c;
  bool endChar = false;
  
  // Remplire avec des espaces
  for (uint8_t i=0; i < 16; i++){
    if(*ptr == NULL){
      // Terminer le tableau
      endChar = true;  
    }
     if(endChar){
      // Terminer le tableau
      *ptr = ' ';  
    }   
    ptr++;
  }
}

void Write_Msg(char L1[], char L2[], char color){

  // Effacer le texte
  Clear(L1);
  Clear(L2);

  // Backup des valeures
  memcpy(Line1_backup, L1, 16);
  memcpy(Line2_backup, L2, 16);

  if (PopUp){
    // Chagement de la couleur
    if(color){
      if(color != 'N'){
        Color_backup = color;
      }
    }  
  }
  else{
    // Ecrire le texte
    lcd.setCursor(0,0);
    lcd.print(L1);
    lcd.setCursor(0,1);
    lcd.print(L2);
  
    // Chagement de la couleur
    if(color){
      if(color != 'N'){
        Color_backup = color;
        LCD_Color(color);
      }
    }    
  }
   
  #ifdef DEBUG
  Serial.println("");
  Serial.println("Message à afficher sur LCD : ");
  Serial.print("L1 : ");
  Serial.println(L1);
  Serial.print("L2 : ");
  Serial.println(L2);
  Serial.print("Color : ");
  Serial.println(color);
  #endif 
}


void Write_Pop(char L1[], char L2[], char color, char t[]){
  // Status popup
  PopUp = true;
  
  // Effacer le texte
  Clear(L1);
  Clear(L2);
  
  // Ecrire le texte
  lcd.setCursor(0,0);
  lcd.print(L1);
  lcd.setCursor(0,1);
  lcd.print(L2);

  // Chagement de la couleur
  if(color){
    LCD_Color(color);
  }

  // Set timmer
  PopupEnd.once(CharToFlt(t), T_EndPopup);

  #ifdef DEBUG
  Serial.println("");
  Serial.println("Pop-up à afficher sur LCD : ");
  Serial.print("L1 : ");
  Serial.println(L1);
  Serial.print("L2 : ");
  Serial.println(L2);
  Serial.print("Color : ");
  Serial.println(color);
  Serial.print("Temps : ");
  Serial.print(t);
  Serial.println(" s");
  #endif 
}


void T_EndPopup(){
  // Status popup
  PopUp = false;
    
  // Ecrire le texte
  lcd.setCursor(0,0);
  lcd.print(Line1_backup);
  lcd.setCursor(0,1);
  lcd.print(Line2_backup);

  // Chagement de la couleur
  LCD_Color(Color_backup);  

  #ifdef Bomb
  // Popup is finish
  ResetPopup();
  #endif 
  
  #ifdef DEBUG
  Serial.println("");
  Serial.println("Retour ancien texte sur LCD : ");
  Serial.print("L1 : ");
  Serial.println(Line1_backup);
  Serial.print("L2 : ");
  Serial.println(Line2_backup);
  Serial.print("Color : ");
  Serial.println(Color_backup);
  #endif 
}


void LCD_Color(char c){
 
  // Chagement de la couleur
  switch (c){ 
  case 'R':    // Rouge
    lcd.setBacklight(C_RED);
    break;
  case 'Y':    // Jaune
    lcd.setBacklight(C_YELLOW);
    break;
  case 'G':    // Vert
    lcd.setBacklight(C_GREEN);
    break;
  case 'T':    // Bleu claire
    lcd.setBacklight(C_TEAL);
    break;
  case 'B':    // Blue
    lcd.setBacklight(C_BLUE);
    break;
  case 'V':    // Violet
    lcd.setBacklight(C_VIOLET);
    break;
  case 'W':    // Blanc
    lcd.setBacklight(C_WHITE);
    break;
  case 'K':    // No update
    lcd.setBacklight(C_BLACK);
    break;
  case 'N':    // No update
    break;
  }
}
#endif
