
// Définition des boutons
#ifdef BTN_R 
PA_Bouton red = PA_Bouton(D3);
#endif

#ifdef BTN_B
PA_Bouton blue = PA_Bouton(D4);
#endif

#ifdef BTN_O
PA_Bouton orange = PA_Bouton(D5); 
#endif

void Bouton()
{

  
  #ifdef BTN_R

    #ifdef LCD
    red.readLCD(lcd.read(D3));
    #else
    red.read();
    #endif
  
    if (red.up())
    {
      UDP.beginPacket(Dest, 8888);
      UDP.write("BTN;R;1");
      UDP.endPacket();
    }
  
    if (red.down())
    {
      UDP.beginPacket(Dest, 8888);
      UDP.write("BTN;R;0");
      UDP.endPacket();     
    } 
  #endif

  #ifdef BTN_B
    
    #ifdef LCD
    blue.readLCD(lcd.read(D4));
    #else
    blue.read();
    #endif
    
    if (blue.up())
    {
      UDP.beginPacket(Dest, 8888);
      UDP.write("BTN;B;1");
      UDP.endPacket();
    }
  
    if (blue.down())
    {
      UDP.beginPacket(Dest, 8888);
      UDP.write("BTN;B;0");
      UDP.endPacket();     
    }
  #endif
  
  #ifdef BTN_O
    #ifdef LCD
    orange.readLCD(lcd.read(D5));
    #else
    orange.read();
    #endif
       
    if (orange.up())
    {
      UDP.beginPacket(Dest, 8888);
      UDP.write("BTN;O;1");
      UDP.endPacket();
    }
  
    if (orange.down())
    {
      UDP.beginPacket(Dest, 8888);
      UDP.write("BTN;O;0");
      UDP.endPacket();     
    }
  #endif
}
