#include "DHT.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>
#include <virtuabotixRTC.h> 
#include <PID_v1.h>


#define DHTPIN 2    //declare pin number connected to dht2
#define DHTPIN2 3   //declare pin number connected to dht2
#define PUMPMPIN 6   //declare pin number connected to dht2

#define DHTTYPE DHT11   // Type of sensor

byte bell[8]  = {0b00010, 0b00101, 0b00010, 0b00000, 0b00000, 0b00000, 0b00000}; //tworze znak stopni

double SetPoint;
double Input;
double Output;
double Kp = 1, Ki = 0.1, Kd = 0.05;

int iterator = 1;

int w0;
int w1;

File myFile;

virtuabotixRTC myRTC(10, 9, 8);
DHT dht(DHTPIN, DHTTYPE);   //create object dht sensor
DHT dht2(DHTPIN2, DHTTYPE); //create object dht2 sensor
LiquidCrystal_I2C lcd(0x27, 16, 2); //create object LCD monitor
PID myPID(&Input, &Output, &SetPoint, Kp, Ki, Kd, DIRECT);

int trybPracy = 2;
char helperPracy[50] = "";

void setup() 
{
  Serial.begin(9600);

  myRTC.setDS1302Time(00, 07, 22, 2, 13, 2, 2024); // ustawianie czasu poczatkowego

  myPID.SetMode(AUTOMATIC);
  myPID.SetTunings(Kp, Ki, Kd);

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

void error()
{
    Serial.println("Blad podczas zapisywania");  // jezeli blad to daj komunikat
    lcd.clear();
    lcd.home();
    lcd.print("Blad zapisu");
    delay(2000);
}

void loop() 
{
  
  float h = dht.readHumidity();
  float h2 = dht2.readHumidity();  

  float t = dht.readTemperature();
  float t2 = dht2.readTemperature();

  w0=analogRead(A0);
  w1=analogRead(A1);


  if (isnan(h) || isnan(t) || isnan(h2) || isnan(t2)) // Check if any reads failed and exit early (to try again).
  {
    Serial.println(F("Blad przy odczytaniu DHT"));
    lcd.clear();
    lcd.home();
    lcd.print("Blad odczytu DHT");
    delay(2000);
    return;
  }
  //-----------GUI--------------
  
  Serial.print(h);
  Serial.print(",");
  Serial.print(h2);
  Serial.print(",");
  Serial.print("\n");
  Serial.print(t);
  Serial.print(",");
  Serial.print(t2);
  Serial.print(",\n");
  Serial.print(w0);
  Serial.print(",");
  Serial.print(w1);
  Serial.print(";\n");

  lcd.clear();
  lcd.home();
  
  lcd.print(h);
  lcd.print(" ");
  lcd.print(t);
  lcd.print(" ");
  lcd.print(w0);
  
  lcd.setCursor(0, 1);
  
  lcd.print(h2);
  lcd.print(" ");
  lcd.print(t2);
  lcd.print(" ");
  lcd.print(w1);

  delay(2000);
  
  //---------------ZAPIS DANYCH DO PLIKU---------------

  myFile = SD.open("Czas.txt", FILE_WRITE);
    
  if (myFile) 
  {

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
    myFile.print(":");
    myFile.print(myRTC.seconds);

    myFile.print("\n");
    myFile.close();
  } 
  else 
  {
    error();
  }

  myFile = SD.open("TempPow.txt", FILE_WRITE);
  
  if(myFile)
  {
    myFile.print(t);
    myFile.print("; ");
    myFile.print(t2);
    myFile.print("\n");
    
    myFile.close();
  }
    else 
  {
    error();
  }
 
  myFile = SD.open("Gleba.txt", FILE_WRITE);

  if(myFile)
  {
    myFile.print(w0);
    myFile.print(";"); 
    myFile.print(w1);
    myFile.print(";\n");  
    
    myFile.close();
  }
    else 
  {
    error();
  }

    myFile = SD.open("WilPow.txt", FILE_WRITE);
 
  if(myFile)
  {
    myFile.print(h);
    myFile.print("; ");
    myFile.print(h2);    
    myFile.print("\n");
    
    myFile.close(); 
  }
    else 
  {
    error();
  }

  //-----------STEROWANIE--------------
  
  myFile = SD.open("Gleba.txt", FILE_READ);
  int humData = 0;
  int target = 500;

  if(myFile)
  {
    int lines = 100; //liczba pomiarów wstecz
    long helper = myFile.size()-(9*lines); //Ustawiam kursor w pliku na last 100 lini
    
    if (helper >0)
    {    
      String buf;
      long values = 0;

      myFile.seek(helper);
  
      for (int i=0;i<lines*2;i++)
      {
        //Serial.println(i);
        buf = myFile.readStringUntil(';'); 
        //Serial.println(buf);
        //Serial.println(helper);
        values = values + buf.toInt();
        //Serial.println(values);
      }
        humData = values/(lines*2);
        //Serial.println(humData);
        if (Serial.available() > 0) 
        {
          Serial.readBytesUntil('+', helperPracy, sizeof(helperPracy));
          //Serial.println("Zawartość zmiennej");
          //Serial.println(helperPracy);
          int helperPracydlugosc = strlen(helperPracy);
          
          if(helperPracy[helperPracydlugosc-5]=='T' and
             helperPracy[helperPracydlugosc-4]=='R' and
             helperPracy[helperPracydlugosc-3]=='Y' and
             helperPracy[helperPracydlugosc-2]=='B')
             {
              trybPracy = helperPracy[helperPracydlugosc-1]-48;
              if(helperPracydlugosc>6)
              {
                target = helperPracy[helperPracydlugosc-6]-48 +(helperPracy[helperPracydlugosc-7]-48)*10 +(helperPracy[helperPracydlugosc-8]-48)*100;
              }
              
              Serial.print(target);
              Serial.print("Zmiana trybu ");
              Serial.print(trybPracy);
              Serial.print("\n");
             }
          memset(helperPracy, '\0', sizeof(helperPracy)); // clear array   
        }
        if(trybPracy==1)
        {
          Serial.println(trybPracy);
          
            if(humData > target)
            {
              Serial.println("Podlewam;");
              lcd.clear();
              lcd.home();
              lcd.print("Podlewam");    
              
              myFile.close(); 
              myFile = SD.open("Gleba.txt", FILE_WRITE);
              
              if(myFile)
              {
                myFile.print("-80000");
                myFile.print(";;\n"); 
                
                analogWrite(PUMPMPIN,255); // włącz pompę
                delay(2000);
                analogWrite(PUMPMPIN,0); // wyłącz pompe 
              }
                else 
              {
                analogWrite(PUMPMPIN,0); // wyłącz pompe 
                error();
              }
        
            }
            analogWrite(PUMPMPIN,0); // wyłącz pompe  
        }
        else if (trybPracy==2)
        {

          Serial.println(trybPracy);
          Input = ((w0+w1)/2 - 541.71)/(-1.9318); //Input = humData;
          SetPoint = (target - 541.71)/(-1.9318);
          myPID.Compute();
          Serial.println(Input);
          Serial.println(Output/50);

          Serial.println(iterator);
          Serial.println(iterator%10);
          if (iterator%10 == 0)
          {
            analogWrite(PUMPMPIN,(255));
            if (Output/50 > 1.5)
            {
              delay((Output/50)*1000);
            }
            analogWrite(PUMPMPIN,(0));
            iterator = 1;
          }
          else 
          {
            iterator++;
          }
        }
    }
  }
  myFile.close(); 
  
}
