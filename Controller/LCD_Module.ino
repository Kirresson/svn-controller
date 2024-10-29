#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display



void clearRow(int row) {
  // clear the first row if 1 and second row if 2. Delay is necessary, otherwise it won't print correctly
  if(row == 1){
    lcd.setCursor(0,0);
    lcd.print("                ");
  }
  if(row == 2){
    lcd.setCursor(0,1);
    lcd.print("                ");
  }
  delay(100);
}

void printLCD1(String stringtoprint1) {
  // clear both rows and then print the delivered string in the first row
  clearRow(1);
  clearRow(2);
  lcd.setCursor(0,0);
  lcd.print(stringtoprint1);
}

void printLCD2(String stringtoprint2) {
  // clear only the second row and print the delivered string in the second row
  clearRow(2);
  lcd.setCursor(0,1);
  lcd.print(stringtoprint2);
}

void setup_lcd()
{
  lcd.init(); // initialize the lcd 
  lcd.clear();
  lcd.backlight();
}