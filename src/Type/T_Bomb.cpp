#include <common.h>

#ifdef BOMB

void InitBomb()
{
    // make some custom characters:
    byte BluUp[8] = {0b00000, 0b01111, 0b00011, 0b00101, 0b01001, 0b10000, 0b00000, 0b11111};
    byte BluDown[8] = {0b00000, 0b10000, 0b01001, 0b00101, 0b00011, 0b01111, 0b00000, 0b11111};
    byte RedDown[8] = {0b00000, 0b00001, 0b10010, 0b10100, 0b11000, 0b11110, 0b00000, 0b11111};
    byte RedUp[8] = {0b00000, 0b11110, 0b11000, 0b10100, 0b10010, 0b00001, 0b00000, 0b11111};

    // Create custom CHAR
    lcd.createChar(4, BluUp);
    lcd.createChar(5, BluDown);
    lcd.createChar(6, RedDown);
    lcd.createChar(7, RedUp);
}

Ticker _tickBomb;
char _password[16];
char _bombColor = '-';
int _bombDiff = 0;
int _bombTime = 0;
int _posStart = 0;
int _minBomb = 0;      // Time left of bomb (Min)
int _secBomb = 0;      // Time left of bomb (Sec)
bool _lock = false;    // Lock time update when bomb is ON
bool _bombSet = false; // Bomb is setted


void SetBomb(char Id, char team, const char Difficulty[])
{
    // Save party Id
    party = Id;

    // Sauvegarde des paramètres
    _bombColor = team;
    _bombDiff = atoi(Difficulty);

    // Save pos start
    _posStart = 8 - (_bombDiff / 2);


#ifdef LOG
    Serial.println("");
    Serial.print("Set bomb - Id ");
    Serial.print(Id);
    Serial.print(" with setted value of difficulty :");
    Serial.println(Difficulty);
#endif

    // Send status of parameter receive
    char msg[] = "PBO;X"; // 8 octets, indices 0..7 + '\0'
    msg[4] = Id;          // remplace X
    comm.send(msg);
}

void SetPassword(const char Psw[])
{
    // Set full flag to line 1
    lcd.setCursor(_posStart, 1);
    for (int i = 0; i < _bombDiff; i++)
    {

        switch (Psw[i])
        {
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

void BombPlanted(const char Psw[], const char Min[], const char Sec[])
{

    // Sauvegarde des paramètres
    memcpy(_password, Psw, 16);
    _minBomb = atoi(Min);
    _secBomb = atoi(Sec);

    // Create ticker
    _tickBomb.attach(1, tickBomb);
    _bombSet = true;
}

void tickBomb()
{

    // Update time
    if (_secBomb > 0)
    {
        _secBomb = _secBomb - 1;
    }
    else
    {
        if (_minBomb > 0)
        {
            _minBomb = _minBomb - 1;
            _secBomb = 59;
        }
        else
        {
            _tickBomb.detach();
        }
    }

    // Update time
    if (!lcd.AsPopup() && !_lock)
    {

        // Set password
        lcd.setCursor(_posStart, 1);

        for (int i = 0; i < _bombDiff; i++)
        {

            switch (_password[i])
            {
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
        lcd.setCursor(10, 0);

        char msg[6];
        snprintf(msg, sizeof(msg), "%02u:%02u", (unsigned)_minBomb, (unsigned)_secBomb);
        lcd.print(msg);
    }
}

void SetDefuse(const char Psw[])
{

    bool endChar = false;

    // Lock update of time
    if (_lock == false)
    {
        _lock = true;
        lcd.setCursor(0, 0);
        lcd.print("                ");
    }

    // Set full flag to line 1
    lcd.setCursor(_posStart, 0);
    for (int i = 0; i < _bombDiff; i++)
    {

        if (endChar)
        {
            lcd.print("_");
        }
        else
        {

            switch (Psw[i])
            {
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


void ResetDefuse()
{
    // Lock update of time
    _lock = false;
}


void ResetPopup()
{
    if (_bombSet)
    {
        // Set text
        lcd.setCursor(10, 0);

        char msg[6];
        snprintf(msg, sizeof(msg), "%02u:%02u", (unsigned)_minBomb, (unsigned)_secBomb);
        lcd.print(msg);
    }
}

void ResetBomb()
{
    // Lock update of time
    _lock = false;

    // Lock update of time
    _tickBomb.detach();
    _bombSet = false;
}
#endif