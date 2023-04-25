#include "DHT.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>
#include <virtuabotixRTC.h> 


#define DHTPIN 2    //declare pin number connected to dht2
#define DHTPIN2 3   //declare pin number connected to dht2

#define DHTTYPE DHT11   // Type of sensor

byte bell[8]  = {0b00010, 0b00101, 0b00010, 0b00000, 0b00000, 0b00000, 0b00000}; //tworze znak stopni

int w0;
int w1;
int w2;

File myFile;

virtuabotixRTC myRTC(10, 9, 8);
DHT dht(DHTPIN, DHTTYPE);   //create object dht sensor
DHT dht2(DHTPIN2, DHTTYPE); //create object dht2 sensor
LiquidCrystal_I2C lcd(0x27, 16, 2); //create object LCD monitor

void setup() 
{
  Serial.begin(9600);

  myRTC.setDS1302Time(00, 13, 14, 2, 25, 4, 2023); // ustawianie czasu poczatkowego

  dht.begin(); //setting sensor
  dht2.begin(); 
  lcd.begin(); 

  lcd.createChar(0, bell);

  if (!SD.begin(4)) 
  {
    Serial.println("Blad inicializacji");
    lcd.clear();
    lcd.home();
    lcd.print("Blad startu");
    lcd.setCursor(0, 1);
    lcd.print("Resetuj wszystko");
    while(1); 
  }
  
  Serial.println("Konfiguracja zakonczona");
  lcd.clear();
  lcd.home();
  lcd.print("Konfiguracja ok");

}

void loop() {

  delay(2000);

  float h = dht.readHumidity();
  float h2 = dht2.readHumidity();  

  float t = dht.readTemperature();
  float t2 = dht2.readTemperature();

  w0=analogRead(A0);
  w1=analogRead(A1);
  w2=analogRead(A2);

  if (isnan(h) || isnan(t) || isnan(h2) || isnan(t2)) // Check if any reads failed and exit early (to try again).
  {
    Serial.println(F("Blad przy odczytaniu DHT"));
    lcd.clear();
    lcd.home();
    lcd.print("Blad odczytu DHT");
    delay(2000);
    return;
  }
  
  Serial.print(myRTC.dayofmonth);
  Serial.print(".");
  Serial.print(myRTC.month);
  Serial.print(".");    
  Serial.print(myRTC.year);
  Serial.print(" "); 
  Serial.print(myRTC.hours);
  Serial.print(":");
  Serial.println(myRTC.minutes);
  
  
  Serial.print("Wilgotnosc  1: ");
  Serial.print(h);
  Serial.print("% 2: ");
  Serial.print(h2);
  Serial.print("%  Temperatrura  1: ");
  Serial.print(t);
  Serial.print("°C 2: ");
  Serial.print(t2);
  Serial.print("°C ");

  Serial.print("\n");

  Serial.print("Wilgotnosc gleby  1: ");
  Serial.print(w0);
  Serial.print(" 2: ");
  Serial.print(w1);
  Serial.print(" 3: ");
  Serial.print(w2);
  
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

  delay(5000);

  lcd.clear();
  
  lcd.print("Wilgotnosc gleby");
  lcd.setCursor(0, 1);
  lcd.print(w0);
  lcd.setCursor(6, 1);
  lcd.print(w1);
  lcd.setCursor(12, 1);
  lcd.print(w2);
  
  delay(5000);
  
  myFile = SD.open("test.txt", FILE_WRITE);
    
  if (myFile) 
  {
    Serial.print("Zapisywanie do pliku...");
    lcd.clear();
    lcd.home();
    lcd.print("Zapis pliku...");

    myRTC.updateTime(); 
     
    myFile.print(myRTC.dayofmonth);
    myFile.print(".");
    myFile.print(myRTC.month);
    myFile.print(".");    
    myFile.print(myRTC.year);
    myFile.print(" "); 
    myFile.print(myRTC.hours);
    myFile.print(":");
    myFile.print(myRTC.minutes);


    myFile.print("\n");
    myFile.print(h);
    myFile.print("; ");
    myFile.print(t);
    myFile.print(";  ");
    myFile.print(h2);
    myFile.print("; ");
    myFile.print(t2);
    myFile.print("; ");
    
    myFile.print(w0);
    myFile.print(";  ");
    myFile.print(w1);
    myFile.print("; ");
    myFile.print(w2); 
    
    myFile.print("\n");
    
    myFile.print((h2+h)/2);
    myFile.print("; ");
    myFile.print((t2+t)/2);
    myFile.print("\n");
       
 
    myFile.close(); // close the file:

    delay(3000);
    
    Serial.println("Zapisano");
    lcd.clear();
    lcd.home();
    lcd.print("Zapisano");

    delay(1000);
  } 
  else 
  {
    Serial.println("Blad podczas zapisywania");  // jezeli blad to daj komunikat
    lcd.clear();
    lcd.home();
    lcd.print("Blad zapisu");
    delay(2000);
  }
  
}
