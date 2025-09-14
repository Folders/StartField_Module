#include "Peripherals/M_LCD.h"
#include <Arduino.h>
#include <common.h>

void M_LCD::begin()
{
	_lcd.begin(16, 2);
}


void M_LCD::Write_Msg(const char* L1, const char* L2, const char color)
{

    // Copier et remplir d'espaces
    strncpy(_backupLine1, L1, 16);
    strncpy(_backupLine2, L2, 16);
    _backupLine1[16] = '\0';
    _backupLine2[16] = '\0';

    // Compléter avec espaces
    for (uint8_t i = strlen(_backupLine1); i < 16; i++) _backupLine1[i] = ' ';
    for (uint8_t i = strlen(_backupLine2); i < 16; i++) _backupLine2[i] = ' ';

    // Écriture sur LCD
    if (_popup) {
        if (color && color != 'N') {
            _backupColor = color;
        }
    } 
	else 
	{
        _lcd.setCursor(0, 0);
        _lcd.print(_backupLine1);
        _lcd.setCursor(0, 1);
        _lcd.print(_backupLine2);

        if (color && color != 'N') {
            _backupColor = color;
            SetColor(color);
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

void M_LCD::Write_Pop(const char* L1, const char* L2, const char color, const char* t) 
{
    _popup = true;

    // Créer des lignes de 16 caractères complètes
    char line1[17];
    char line2[17];

    strncpy(line1, L1, 16);
    strncpy(line2, L2, 16);
    line1[16] = '\0';
    line2[16] = '\0';

    for (uint8_t i = strlen(line1); i < 16; i++) line1[i] = ' ';
    for (uint8_t i = strlen(line2); i < 16; i++) line2[i] = ' ';

    // Afficher le texte
    _lcd.setCursor(0, 0);
    _lcd.print(line1);
    _lcd.setCursor(0, 1);
    _lcd.print(line2);

    // Couleur
    if (color) {
        SetColor(color);
    }

    // Timer pour fermer le popup
    _ticker.attach(CharToFloat(t), std::bind(&M_LCD::_endPopup, this));

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

void M_LCD::_endPopup()
{
	// Status popup
	_popup = false;
	_ticker.detach();

	// Ecrire le texte
	_lcd.setCursor(0, 0);
	_lcd.print(_backupLine1);
	_lcd.setCursor(0, 1);
	_lcd.print(_backupLine2);

	// Chagement de la couleur
	SetColor(_backupColor);

#ifdef Bomb
	// Popup is finish
	ResetPopup();
#endif

#ifdef DEBUG
	Serial.println("");
	Serial.println("Retour ancien texte sur LCD : ");
	Serial.print("L1 : ");
	Serial.println(_backupLine1);
	Serial.print("L2 : ");
	Serial.println(_backupLine2);
	Serial.print("Color : ");
	Serial.println(_backupColor);
#endif
}

void M_LCD::SetColor(char c)
{

	// Chagement de la couleur
	switch (c)
	{
	case 'R': // Rouge
		_lcd.setBacklight(C_RED);
		break;
	case 'Y': // Jaune
		_lcd.setBacklight(C_YELLOW);
		break;
	case 'G': // Vert
		_lcd.setBacklight(C_GREEN);
		break;
	case 'T': // Bleu claire
		_lcd.setBacklight(C_TEAL);
		break;
	case 'B': // Blue
		_lcd.setBacklight(C_BLUE);
		break;
	case 'V': // Violet
		_lcd.setBacklight(C_VIOLET);
		break;
	case 'W': // Blanc
		_lcd.setBacklight(C_WHITE);
		break;
	case 'K': // No update
		_lcd.setBacklight(C_BLACK);
		break;
	case 'N': // No update
		break;
	}
}


bool M_LCD::AsPopup()
{
	return _popup;
}


void M_LCD::setCursor(uint8_t col, uint8_t row)
{
	_lcd.setCursor(col, row);
}

void M_LCD::print(const char * text)
{
	_lcd.print(text);
}

void M_LCD::print(int n)
{
	_lcd.print(n);
}


void M_LCD::createChar(uint8_t ind, uint8_t charmap[])
{
	_lcd.createChar(ind, charmap);
}

void M_LCD::write(uint8_t value)
{
	_lcd.write(byte(value));
}