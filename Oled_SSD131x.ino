//***********************************************//
// --- WIDE.HK                                ---//
// --- Revised Date : 06/30/2013              ---//
// ---                                        ---//
// --- Rafael Camacho / Brazil                ---//
// --- github.com/rafaelcamacho/Oled-SSD131x  ---//
// --- Sketch in Arduino IDE 1.0.5            ---//
// --- Demo code for more features            ---//
// --- and correct initialization             ---//
// --- Revised Date : 01/25/2014              ---//
// ---                                        ---//
// ---                                        ---//
// --- I2C Arduino - Arduino UNO Demo         ---//
// --- SSD131x PMOLED Controller              ---//
// --- SCL, SDA, GND, VCC(3.3v -5v)           ---//
//***********************************************//

#include <Wire.h>          // *** I2C Mode 
#define OLED_Address 0x3c
#define OLED_Command_Mode 0x80
#define OLED_Data_Mode 0x40

byte cc0[8] = 
{
  B00111,
  B01111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

void setCursor(uint8_t col, uint8_t row)
{
  int row_offsets[] = { 0x00, 0x40 };
  sendCommand(0x80 | (col + row_offsets[row]));
}

void setup()
{
  Wire.begin();
  PowerDown();
  PowerUp();
  
  setCursor(0,0);
  sendData(0);
}

void loop()
{
}

// Write to CGRAM of new characters
void createChar(uint8_t location, uint8_t charmap[]) 
{
   location &= 0x7;            // we only have 8 locations 0-7
   
   sendCommand(0x40 | (location << 3));
   delayMicroseconds(30);
   
   for (int i=0; i<8; i++) 
   {
      sendData(charmap[i]);      // call the virtual write method
      delayMicroseconds(40);
   }
}

void PowerUp()
{
  // PowerUp function by Rafael Camacho Jan/2014 Brazil
  
  // Vdd/Vcc off State
  // Power up Vdd
  
  // Power Stabilized (100ms Delay Minimum)
  delay(100);

  // Initialized State (Parameters as Default)

  // Enable Internal Regulator
  sendCommand(0x2A);            // Set "RE" = 1
  sendCommand(0x08);
  sendCommand(0x71);            // Function Selection A
  sendData(0x5C);               // 0x00 - Disable        0x5C - Enable       Internal Vdd regulator at 5V I/O application mode             
  
  // Set Display OFF
  sendCommand(0x08);
  
  // Initial Settings Configuration
  
  // Set Display Clock Divide Ratio Oscilator Frequency
  sendCommand(0x2A);            // Set "RE" = 1
  sendCommand(0x79);            // Set "SD" = 1
  sendCommand(0xD5);
  sendCommand(0x70);
  sendCommand(0x78);
  
  // Set Display Mode
  sendCommand(0x08);            // Extended Function Set = Set 5-dot width -> 3 or 4 line(0x09), 1 or 2 line(0x08)
  
  // Set Re-Map
  sendCommand(0x06);            // Set Com31-->Com0  Seg0-->Seg99
  
  // CGROM/CGRAM Management
  sendCommand(0x72);            // Function Selection B
  sendData(0x01);
  
  // Set OLED Characterization
  sendCommand(0x79);            // Set "SD" = 1
  
  // Set PEG Pins Hardware Configuration
  sendCommand(0xDA);
  sendCommand(0x10);
  
  // Set Segment Low Voltage and GPIO
  sendCommand(0xDC);            // Function Selection C
  sendCommand(0x03); 
  
//  // Set Fade Out and Fade In / Out
//  sendCommand(0x23);
//  sendCommand(0x00);
  
  // Vcc Power Stabilized (100ms Delay Recommended)
  delay(100);
  
  // Set Contrast Control
  sendCommand(0x81);
  sendCommand(0xFF);
  
  // Set Pre-Charge Period
  sendCommand(0xD9);
  sendCommand(0xF1);
  
  // Set VCOMH Deselect Level
  sendCommand(0xDB);
  sendCommand(0x30);            // 0x00 - ~0.65 x Vcc   0x10 - ~0.71 x Vcc   0x20 - ~0.77 x Vcc   0x30 - ~0.83 x Vcc   0x40 - 1 x Vcc
  
  // Exiting Set OLED Characterization
  sendCommand(0x78);            // Set "SD" = 0
  sendCommand(0x28);            // Set "RE" = 0

  // Create char from 0 to 7  
  createChar(0,cc0); 
  
  // Set Display OFF
  sendCommand(0x08);
  
  // Clear Display
  sendCommand(0x01);
  
  // Set DDRAM Address
  sendCommand(0x80);
  
  // Set Display ON
  sendCommand(0x0C);
}


void PowerDown()
{
  // Normal Operation
  
  // Power down Vcc (GPIO)
  sendCommand(0x2A);
  sendCommand(0x79);
  sendCommand(0xDC);
  sendCommand(0x02);
  
  // Set Display Off
  sendCommand(0x78);
  sendCommand(0x28);
  sendCommand(0x08);
  
  // (100ms Delay Recommended
  delay(100);
  
  // Power down Vdd
  // Vdd/Vcc off State
}

void EnterSleepMode()
{
  // Normal operation

  // Power down Vcc (GPIO)
  sendCommand(0x2A);
  sendCommand(0x79);
  sendCommand(0xDC);
  sendCommand(0x02);
  
  // Set Display Off
  sendCommand(0x78);
  sendCommand(0x28);
  sendCommand(0x08);

  // Disable Internal Regulator
  sendCommand(0x2A);
  sendCommand(0x71);
  sendData(0x00);
  sendCommand(0x28);

  // Sleep Mode  
}

void ExitingSleepMode()
{
  // Sleep Mode

  // Disable Internal Regulator
  sendCommand(0x2A);
  sendCommand(0x79);
  sendCommand(0x71);
  sendCommand(0x5C);
  sendCommand(0xDC);
  sendCommand(0x03);

  // Power up Vcc (100ms Delay Recommended)
  delay(100);

  // Set Display On
  sendCommand(0x78);
  sendCommand(0x28);
  sendCommand(0x0C);
  
  // Normal Operation
}

void sendData(unsigned char data)
{
    Wire.beginTransmission(OLED_Address);  	// **** Start I2C 
    Wire.write(OLED_Data_Mode);     		// **** Set OLED Data mode
    Wire.write(data);
    Wire.endTransmission();                     // **** End I2C 
}

void sendCommand(unsigned char command)
{
    Wire.beginTransmission(OLED_Address); 	 // **** Start I2C 
    Wire.write(OLED_Command_Mode);     		 // **** Set OLED Command mode
    Wire.write(command);
    Wire.endTransmission();                 	 // **** End I2C 
    delay(10);
}

void sendString(const char *String)
{
    unsigned char i=0;
    while(String[i])
    {
        sendData(String[i]);      // *** Show String to OLED
        i++;
    }
}
