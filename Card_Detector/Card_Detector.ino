
 /*  RFID RC522          Arduino Uno
 *  SS/SDA              D10
 *  SCK                 D13
 *  MOSI                D11
 *  MISO                D12
 *  IRQ                 Not Connected
 *  GND                 GND
 *  RST                 D9
 *  3.3V                3.3V
*/

//Include sections
#include <RFID.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include "pitches.h"

#define SS_PIN 10
#define RST_PIN 9

#define SERVO_PIN A2

#define Red_LED A0
#define Green_LED A1

#define Buzzer 8

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
Servo DoorLock;
RFID rfid(SS_PIN, RST_PIN );

//Unique ID of RFID Tag, which you want to give access. 
int My_RFID_Tag[5] = {0xE7,0x5D,0x1C,0x52,0xF4};

//variable to hold your Access_card
boolean My_Card = false;  
  
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void setup() 
{
 
  pinMode(Red_LED,OUTPUT);
  pinMode(Green_LED,OUTPUT);
  pinMode(Buzzer,OUTPUT);
  
  DoorLock.attach(SERVO_PIN);
  DoorLock.write(0); //Servo at 0 Position, Door is Closed.
  Serial.begin(9600); 
  lcd.begin(16,2);
  //Initialise RFID
  SPI.begin();
  rfid.init();
}

void loop() 
{
  My_Card = true; 
  lcd.clear();
  lcd.print("Mihai Negru's");
  lcd.setCursor(0,1);
  lcd.print("gang! BMW, MAMA!");
  
  //Check if any RFID Tags Detected or not
  if( rfid.isCard() )
  {
      //if RFID Tag is detected, check for the Unique ID,
      if( rfid.readCardSerial() )
      {   
          lcd.clear();      
          lcd.print("Nr matricol:- ");
          delay(500);          
          lcd.setCursor(0,1);
  
      //Unique id is 5 Digit Number.
          //Printing in HEX
          for( int i = 0; i < 5; i++ )
          {
              Serial.print(rfid.serNum[i], HEX);
              Serial.print(" ");              
              lcd.print(rfid.serNum[i],HEX);
              lcd.print(" ");                            
          }  
          delay(500);
          
      //Compare this RFID Tag Unique ID with My_RFID_Tag's Unique ID
          for(int i = 0; i < 5; i++)
          {   
              if( My_RFID_Tag[i] != rfid.serNum[i] )
              {
                My_Card = false;
                break;                
              }           
          }
          Serial.println(); 
          delay(1000); 

          //If RFID Tag is My_Card then give access to enter into room
          //else don't open the door.
          if(My_Card)
          {
            Serial.println("\nBun venit la poli!");
            lcd.clear();
            lcd.print("Bun venit la ");
            lcd.setCursor(0,1);
            lcd.print("poli! Miau!");
            delay(2000);                        
            
            //Turn on the Green LED as an indication of permission is given 
            //to access the room.
            digitalWrite(Green_LED,HIGH);
            int i = 0;
            while(i < 2)
            {
              for (int thisNote = 0; thisNote < 12; thisNote++) 
              {          
                int noteDuration = 1000 / noteDurations[thisNote];
                tone(8, melody[thisNote], noteDuration);                        
                // to distinguish the notes, set a minimum time between them.
                int pauseBetweenNotes = noteDuration * 1.30;
                delay(pauseBetweenNotes);              
                // stop the tone playing:
                noTone(8);
              }
              i =  i + 1;
              delay(500);              
            }
            delay(1000);            

            //Now, Open the Door with the help of Servo Motor
            DoorLock.write(179);            
            delay(200);
            lcd.clear();                                    
            lcd.print("Deschid usa.");
            lcd.setCursor(0,1);
            lcd.print("Acuma!");
            delay(2000);
            lcd.clear();

            //Give 10 Sec delay to enter into room
            //After that door will be again closed!
            for(int i = 10; i > 0; i--)
            {
              lcd.print("Inchid usa in  ");
              lcd.print(i);
              lcd.setCursor(0,1);
              lcd.print("Hai mai repede!");
              delay(1000);
              lcd.clear();
            }

            //Now,Door is closed and Green LED is Turned-Off.
            DoorLock.write(1);
            digitalWrite(Green_LED,LOW);
            delay(200);
            lcd.clear();
            lcd.print("Ne vedem");
            lcd.setCursor(0,1);
            lcd.print("la toamna!");
            delay(2000);                            
          }
          
          // If RFID Tag is not My_Card then
          // Do not open the Door and 
          //Turn-On Red LED and Buzzer as an indication of Warning:
          else
          {
            Serial.println("\nMai incearca si in toamna !");
            lcd.clear();
            lcd.print("Mai incearca si");
            lcd.setCursor(0,1);
            lcd.print("la toamna ;)");

            for(int i = 0; i < 7; i++)
            {
              digitalWrite(Buzzer, HIGH);
              digitalWrite(Red_LED,HIGH);
              delay(500);
              digitalWrite(Buzzer, LOW);
              digitalWrite(Red_LED,LOW);
              delay(500);              
            }
            delay(1000);            
          }                 
      }      
   }
  //Put RFID Reader into Halt, untill it not detects any RFID Tag.
  rfid.halt();
}
