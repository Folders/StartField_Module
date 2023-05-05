#include <common.h>

// Timmer for blink
Ticker Time_Blink;
bool Blink_R = false;
bool Blink_B = false;
bool Blink_State = false;

void SetLed(char Color, char Value)
{
  
  // Check if blink is requested
  if (Value == 'B'){

    // If ticker is not enabled
    if ( (Blink_R or Blink_B) == false){
        // Create ticker
        Time_Blink.attach(0.5, T_500ms);
    }

    // Set blinker
    if (Color == 'R'){
      Blink_R = true;
    }
    if (Color == 'B'){
      Blink_B = true;
    }    
  } 
    
  // Set output lamps
  #ifdef LED_R 
  if (Color == 'R'){
    
    if (Value == '1'){
      lcd.setRed(true);
      Blink_R = false;
    } 
    
    if (Value == '0'){
      lcd.setRed(false);
      Blink_R = false;
    } 

  }
  #endif
  
  #ifdef LED_B
  if (Color == 'B'){
    
    if (Value == '1'){
      lcd.setBlu(true);
      Blink_B = false;
    } 
    
    if (Value == '0'){
      lcd.setBlu(false);
      Blink_B = false;
    } 
  }
  #endif

  // Detach if blink is not used
  if ( (Blink_R or Blink_B) == false){
    // Create ticker
    Time_Blink.detach();
  }
}

void T_500ms()
{
  // Toggle bit
  Blink_State = !Blink_State;

  // Blink red led
  if (Blink_R){
    lcd.setRed(Blink_State);
  }

  // Blink blue led
  if (Blink_B){
    lcd.setBlu(Blink_State);
  }
  
}