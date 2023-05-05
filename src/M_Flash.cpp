
void LampFlash(char Color, char Value)
{
  // Set output lamps
  #ifdef Flash_R 
  if (Color == 'R'){
    if (Value == '1'){
      digitalWrite(D6, HIGH); 
    } 
    else{
      digitalWrite(D6, LOW);             
    }
  }
  #endif
  
  #ifdef Flash_B
  if (Color == 'B'){
    if (Value == '1'){
      digitalWrite(D7, HIGH); 
    } 
    else{
      digitalWrite(D7, LOW);             
    }  
  }
  #endif
}