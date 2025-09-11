#include "Type/T_Safe.h"

#ifdef SAFE

void SetSafe(char Id)
{
    // Save party Id
    party = Id;
    
    #ifdef LOG
    Serial.println("");
    Serial.print("Set safezone - Id ");
    Serial.println(Id);
    #endif

    // Confirmation de la réception de paramètres
    char msg[] = "PSZ;X";   // 8 octets, indices 0..7 + '\0'
    msg[4] = Id;           // remplace X
    comm.send(msg);
}

  #endif