#include "Type/T_Flag.h"

#ifdef FLAG

void InitFlag()
{
    // make some custom characters:
    byte _full[8] = {0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111};
    byte _empty[8] = {0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111};
    byte _flagEmpty[8] = {0b11111, 0b10001, 0b10001, 0b11111, 0b10000, 0b10000, 0b10000, 0b10000};
    byte _flagFull[8] = {0b11111, 0b11111, 0b11111, 0b11111, 0b10000, 0b10000, 0b10000, 0b10000};

    // Create custom CHAR
    lcd.createChar(0, _flagEmpty);
    lcd.createChar(1, _flagFull);
    lcd.createChar(2, _full);
    lcd.createChar(3, _empty);
}

// Timmer for blink
Ticker _tickFlag;
char _flagColor = '-';
bool _moveToBLU = false;
bool _moveToRED = false;
bool _moveToNeutre = false;
int _PosFlag = 0;

// Set config of flag (Startup)
//     c:   'R' Set flag to RED team
//          'B' Set flag to BLU team
//          'N' Set flag to no team
void SetFlag(char Id, char c)
{

    // Save party Id
    party = Id;

#ifdef LOG
    Serial.println("");
    Serial.print("Set flag - Id ");
    Serial.print(Id);
    Serial.print(" with setted value ");
    Serial.println(c);
#endif

    // Send status of parameter receive
    char msg[] = "PFG;X"; // 8 octets, indices 0..7 + '\0'
    msg[4] = Id;          // remplace X
    comm.send(msg);

    // Chagement de la couleur
    switch (c)
    {
    case 'R': // Flag setted to RED team
        takeRED();
        break;

    case 'B': // Flag setted to BLU team
        takeBLU();
        break;

    case 'N': // Flag setted to free
        takeFREE();
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
void MoveFlag(char c)
{
    // Chagement de la couleur
    switch (c)
    {

    // Move flag to RED team
    case 'R':
        // Check if flag is already RED
        if (_flagColor == 'R' and _PosFlag <= -16)
        {
            // If flag is RED, trigger an event "Flag RED is taken"
            comm.send("SFG;R;T");

            // Reset order
            _moveToBLU = false;
            _moveToRED = false;
            _moveToNeutre = false;
        }
        else
        {
            // Set order
            _moveToBLU = false;
            _moveToRED = true;
            _moveToNeutre = false;

            // Run ticker
            _tickFlag.attach(0.5, tickFlag);

            // Write change on LCD
            lcd.setCursor(6, 1);
            lcd.print("<<<<");

            // If flag is BLU, trigger an event "Flag BLU goes down"
            if (_flagColor == 'B')
                comm.send("SFG;B;D");

            // If flag is RED, trigger an event "Flag RED goes up"
            else if (_flagColor == 'R')
                comm.send("SFG;R;U");

            // If flag is FREE, trigger an event "Flag RED goes up"
            else if (_flagColor == 'N')
                comm.send("SFG;N;R");
        }
        break;

    // Move flag to BLU team
    case 'B':
        // Check if flag is already BLU
        if (_flagColor == 'B' and _PosFlag >= 16)
        {
            // If flag is BLU, trigger an event "Flag BLU is taken"
            comm.send("SFG;B;T");

            // Reset order
            _moveToBLU = false;
            _moveToRED = false;
            _moveToNeutre = false;
        }
        else
        {
            // Set order
            _moveToBLU = true;
            _moveToRED = false;
            _moveToNeutre = false;

            // Run ticker
            _tickFlag.attach(0.5, tickFlag);

            // Write change on LCD
            lcd.setCursor(6, 1);
            lcd.print(">>>>");

            // If flag is RED, trigger an event "Flag RED goes down"
            if (_flagColor == 'R')
                comm.send("SFG;R;D");

            // If flag is BLU, trigger an event "Flag BLU goes up"
            if (_flagColor == 'B')
                comm.send("SFG;B;U");

            // If flag is FREE, trigger an event "Flag BLU goes up"
            if (_flagColor == 'N')
                comm.send("SFG;N;B");
        }
        break;

    // Stop flag
    case 'S':
        // Set order
        _moveToBLU = false;
        _moveToRED = false;
        _moveToNeutre = false;

        // Stop ticker
        _tickFlag.detach();

        // Write change on LCD
        lcd.setCursor(6, 1);
        lcd.print("    ");

        // Trigger an event "Flag Stop"

        char msg[] = "SFG;x;S";
        msg[4] = _flagColor; // remplace X
        comm.send(msg);
        break;
    }
}


void tickFlag()
{
    // If move to BLU, increase position
    if (_moveToBLU)
    {
        _PosFlag = _PosFlag + 1;
    }

    // If move to RED, increase position
    if (_moveToRED)
    {
        _PosFlag = _PosFlag - 1;
    }

#ifdef DEBUG
    Serial.println("");
    Serial.print("Indice drapeau : ");
    Serial.println(_PosFlag);
#endif

    ///////  Check position of flag  ///////

    // Flag taken by RED
    if (_PosFlag < -15)
    { 
        // Flag taken by RED
        takeRED();         
        _tickFlag.detach();
        _moveToBLU = false;
        _moveToRED = false;
        _moveToNeutre = false;
    }
    else if (_PosFlag < 0)
    { 
        // Flag move between RED and FREE
        lcd.setCursor(0, 0);
        for (int i = -16; i <= 0; i++)
        {
            if (i < _PosFlag)
            {
                lcd.write(byte(3));
            }
            else
            {
                lcd.write(byte(2));
            }
        }
    }
    else if (_PosFlag == 0)
    { // Flag taken by NULL

        takeFREE(); // Flag move to red RED

        // If flag move to BLU, trigger an event "Flag BLU goes up"
        if (_moveToBLU)
            comm.send("SFG;N;B");
        
            // If flag move to RED, trigger an event "Flag RED goes up"
        else if (_moveToRED)
            comm.send("SFG;N;R");
    }
    else if (_PosFlag < 16)
    {
        // Flag move between BLU and FREE
        lcd.setCursor(0, 0);
        for (int i = 1; i <= 16; i++)
        {
            if (i <= _PosFlag)
            {
                lcd.write(byte(2));
            }
            else
            {
                lcd.write(byte(3));
            }
        }
    }
    else
    {
        // Flag taken by BLU
        takeBLU();         // Flag taken by BLU
        _tickFlag.detach(); // Stop ticker
        _moveToBLU = false;
        _moveToRED = false;
        _moveToNeutre = false;
    }
}


// When RED flag is taken
void takeRED()
{
    // Change background LCD
    lcd.SetColor('R');

    // Set full flag to line 1
    lcd.setCursor(0, 0);
    for (int i = 1; i <= 16; i++)
    {
        lcd.write(byte(2));
    }

    // Set texte to line 2
    lcd.setCursor(0, 1);
    lcd.write((byte)1);
    lcd.print(":RED      BLU:");
    lcd.write((byte)0);

    // Save flage color
    _flagColor = 'R';

    // Save position of flag
    _PosFlag = -16;

    // Trigger an event "Flag RED is taken"
    comm.send("SFG;R;T");
}

// When BLU flag is taken
void takeBLU()
{

    // Change background LCD
    lcd.SetColor('T');

    // Set full flag to line 1
    lcd.setCursor(0, 0);
    for (int i = 1; i <= 16; i++)
    {
        lcd.write(byte(2));
    }

    // Set texte to line 2
    lcd.setCursor(0, 1);
    lcd.write((byte)0);
    lcd.print(":RED      BLU:");
    lcd.write((byte)1);

    // Save flage color
    _flagColor = 'B';

    // Save position of flag
    _PosFlag = 16;

    // Trigger an event "Flag BLU is taken"
    comm.send("SFG;B;T");
}

void takeFREE()
{
    // Change background LCD
    lcd.SetColor('G');

    // Set full flag to line 1
    lcd.setCursor(0, 0);
    for (int i = 1; i <= 16; i++)
    {
        lcd.write(byte(3));
    }

    // Set texte to line 2
    lcd.setCursor(0, 1);
    lcd.write((byte)0);
    lcd.print(":RED      BLU:");
    lcd.write((byte)0);

    // Write ">>>>" animation if flag move
    if (_moveToBLU)
    {
        // Write change on LCD
        lcd.setCursor(6, 1);
        lcd.print(">>>>");
    }
    if (_moveToRED)
    {
        // Write change on LCD
        lcd.setCursor(6, 1);
        lcd.print("<<<<");
    }

    // Save flage color
    _flagColor = 'N';

    // Save position of flag
    _PosFlag = 0;

    // Trigger an event "Flag Neutre is taken"
    comm.send("SFG;N;T");
}

#endif