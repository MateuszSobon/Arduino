#include "DHT.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
//#include <SPI.h>
//#include <SD.h>


#define DHTPIN 2    //declare pin number connected to dht2
#define DHTPIN2 3   //declare pin number connected to dht2

#define DHTTYPE DHT11   // Type of sensor

byte bell[8]  = {0b00010, 0b00101, 0b00010, 0b00000, 0b00000, 0b00000, 0b00000};
//File myFile;

DHT dht(DHTPIN, DHTTYPE);   //create object dht sensor
DHT dht2(DHTPIN2, DHTTYPE); //create object dht2 sensor
LiquidCrystal_I2C lcd(0x27, 16, 2); //create object LCD monitor

void setup() 
{
  Serial.begin(9600);

  dht.begin(); //setting sensor
  dht2.begin(); 
  lcd.begin(); 

  //lcd.noBacklight();
  lcd.createChar(0, bell);
/*
  if (!SD.begin(4)) 
  {
    Serial.println("initialization failed!");
  }
  
  Serial.println("Konfiguracja zakonczona");
  //lcd.print("Konfiguracja ok");
*/
}

void loop() {

  delay(2000);

  float h = dht.readHumidity();
  float h2 = dht2.readHumidity();  

  float t = dht.readTemperature();
  float t2 = dht2.readTemperature();

  if (isnan(h) || isnan(t) || isnan(h2) || isnan(t2)) // Check if any reads failed and exit early (to try again).
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    //lcd.home();
    //lcd.print("Blad odczytu");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%  Temperature: ");
  Serial.print(t);
  Serial.print("°C ");

  Serial.print("Humidity2: ");
  Serial.print(h2);
  Serial.print("%  Temperature2: ");
  Serial.print(t2);
  Serial.print("°C  ");
  
  Serial.print("\n");

  lcd.home();
  
  lcd.print(h);
  lcd.print(" % ");
  lcd.print(t);
  lcd.write(0);
  lcd.print("C ");
  
  lcd.setCursor(0, 1);
  
  lcd.print(h2);
  lcd.print(" % ");
  lcd.print(t2);
  lcd.write(0);
  lcd.print("C ");

  /*myFile = SD.open("test.txt", FILE_WRITE);
    
  if (myFile) 
  {
    Serial.print("Zapisywanie do pliku...");
    myFile.println(h,t);
    
    myFile.close(); // close the file:
    
    Serial.println("Zapisano");
    lcd.home();
    lcd.print("Zapisano");
  } 
  else 
  {
    Serial.println("Blad podczas zapisywania");  // jezeli blad to daj komunikat
    lcd.home();
    lcd.print("Blad zapisu");
  }
  */
}
