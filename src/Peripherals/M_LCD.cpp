#include "Peripherals/M_LCD.h"
#include <Arduino.h>
#include <common.h>

void M_LCD::begin()
{
	_lcd.begin(16, 2);
}

void M_LCD::Clear(char c[])
{
	char *ptr = c;
	bool endChar = false;
	// Remplire avec des espaces
	for (uint8_t i = 0; i < 16; i++)
	{
		if (*ptr == NULL)
		{
			// Terminer le tableau
			endChar = true;
		}
		if (endChar)
		{
			// Terminer le tableau
			*ptr = ' ';
		}
		ptr++;
	}
}

void M_LCD::Write_Msg(String L1, String L2, char color)
{

	// Effacer le texte
	//Clear(L1);
	//Clear(L2);

	// Backup des valeures
	//memcpy(_backupLine1, L1, 16);
	//memcpy(_backupLine2, L2, 16);

	if (_popup)
	{
		// Chagement de la couleur
		if (color)
		{
			if (color != 'N')
			{
				_backupColor = color;
			}
		}
	}
	else
	{
		// Ecrire le texte
		_lcd.setCursor(0, 0);
		_lcd.print(L1);
		_lcd.setCursor(0, 1);
		_lcd.print(L2);

		// Chagement de la couleur
		if (color)
		{
			if (color != 'N')
			{
				_backupColor = color;
				SetColor(color);
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

void M_LCD::Write_Pop(String L1, String L2, char color, char t[])
{
	// Status popup
	_popup = true;

	// Effacer le texte
	//Clear(L1);
	//Clear(L2);

	// Ecrire le texte
	_lcd.setCursor(0, 0);
	_lcd.print(L1);
	_lcd.setCursor(0, 1);
	_lcd.print(L2);

	// Chagement de la couleur
	if (color)
	{
		SetColor(color);
	}

	// Set timmer
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
	Serial.println(Line1_backup);
	Serial.print("L2 : ");
	Serial.println(Line2_backup);
	Serial.print("Color : ");
	Serial.println(Color_backup);
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
