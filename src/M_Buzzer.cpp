#include <common.h>

#ifdef Buzzer

// Timmer
Ticker BuzzerEnd;
int BuzzerStep = 0;

void SetBuzzer(char t[]){
  // Start buzzer
  digitalWrite(D5, 1);

  // Compute time in 100ms
  float Time = CharToFlt(t);
  Time = Time / 10;
  
  // Set timmer
  BuzzerEnd.once(Time, T_BuzzerEnd);

  #ifdef DEBUG
  Serial.println("");
  Serial.print("Buzzer sonne pendant : ");
  Serial.print(t);
  Serial.println(" x100ms");
  #endif 
}


void T_BuzzerEnd(){
  // Stop buzzer
  digitalWrite(D5, 0);
  
  #ifdef DEBUG
  Serial.println("");
  Serial.println("Fin du buzzer !");
  #endif 
}


void BuzzerFinsih(){

  // Reset step
  BuzzerStep = 0;
  
  // Start buzzer
  digitalWrite(D5, 1);

  // Set timmer
  BuzzerEnd.once(0.4, T_BuzzerStep);

  #ifdef DEBUG
  Serial.println("");
  Serial.println("Buzzer sonne la fin de partie");
  #endif 
}


void T_BuzzerStep(){

  // Increase step
  BuzzerStep = BuzzerStep + 1;
  
  #ifdef DEBUG
  Serial.println("");
  Serial.print("Etape du buzzer : ");
  Serial.println(BuzzerStep);
  #endif 
  
  switch (BuzzerStep){
    case 1:
      // Stop buzzer
      digitalWrite(D5, 0);
      // Set timmer
      BuzzerEnd.once(0.4, T_BuzzerStep);
      break;
      
    case 2:
      // Start buzzer
      digitalWrite(D5, 1);
      // Set timmer
      BuzzerEnd.once(0.4, T_BuzzerStep);
      break;    

    case 3:
      // Start buzzer
      digitalWrite(D5, 0);
      // Set timmer
      BuzzerEnd.once(0.4, T_BuzzerStep);
      break;    
   
    case 4:
      // Start buzzer
      digitalWrite(D5, 1);
      // Set timmer
      BuzzerEnd.once(2, T_BuzzerStep);
      break;    
      
    case 5:
      // Start buzzer
      digitalWrite(D5, 0);
      break;       

    default:
      digitalWrite(D5, 0);
      break;
  }
  
}


#endif
