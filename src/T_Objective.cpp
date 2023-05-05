#include <common.h>

#ifdef OBJECTIVE 

/// @brief Set module as objective
/// @param Id Party Id (0..9)
void SetObjective(char Id){

  // Save party Id
  Party = Id;
  
  // Confirmation de la réception de paramètres
  UDP.beginPacket(Dest, 8888);
  UDP.write("POB;");
  UDP.write(Id);
  UDP.endPacket();

}

#endif

