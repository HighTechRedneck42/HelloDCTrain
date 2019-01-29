//www.elegoo.com
//2016.12.9

/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 37
 * LCD Enable pin to digital pin 38
 * LCD D4 pin to digital pin 39
 * LCD D5 pin to digital pin 40
 * LCD D6 pin to digital pin 41
 * LCD D7 pin to digital pin 42
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 */

// include the library code:
#include <LiquidCrystal.h>
#include "IRremote.h"

#define BREAK 9
#define DIR 12
#define SPEED 3
#define CURRENT 0
#define TRAVELSPEED 200

int receiver = 11; // Signal Pin of IR receiver to Arduino Digital Pin 11

/*-----( Declare objects )-----*/
IRrecv irrecv(receiver);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'
int i;
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(37, 38, 39, 40, 41, 42);
char buff[16];
unsigned keypadInput = 0;

const unsigned long interval = 1000; 
unsigned long previousMillis = 0; 
unsigned direction = 0;
unsigned speed = 0;
boolean controlledStop = false;
boolean startup = false;



void setup() {
  Serial.begin(115200);
  while (! Serial); // Wait untilSerial is ready - Leonardo
  Serial.println("Enter direction(0,1) speed(0-100)");
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  // Print a message to the LCD.
  irrecv.enableIRIn(); // Start the receiver
    //---set pin direction
  pinMode(SPEED,OUTPUT);
  pinMode(DIR,OUTPUT);
  pinMode(BREAK,OUTPUT);

}

void loop() {
  String text;
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):0 0
  
  lcd.setCursor(0, 1);
  unsigned long currentMillis = millis();
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    translateIR(); 
    changeState();
    irrecv.resume(); // receive the next value
  }  
  if (Serial.available())
  { 
    text = Serial.readString();
    direction = text.substring(0,1).toInt();
    speed = normalize(text.substring(2,text.length()).toInt());
    changeState();
  }
  
  if (currentMillis - previousMillis >= interval) {
    
    previousMillis = currentMillis;
    int current = analogRead(CURRENT);
    int n = sprintf(buff,"%d",current);
    lcdPrint(13,1,buff);
    if(startup){
      if(speed <= TRAVELSPEED) {
        speed += 20;
      } else {
        speed = TRAVELSPEED;
        startup = false;
      }
      changeState();
    }
    if(controlledStop){
      if(speed >= 60) {
        speed -= 20;
      } else {
        speed = 0;
        controlledStop = false;
      }
      changeState();
    }
  }
}



void lcdPrint(int col, int row, String text) {
  if ((row == 0 || row == 1) && (col >= 0 && col <= 15)){
    lcd.setCursor(col,row);
    lcd.print(text);
  }
}

void changeState(){
  if (speed == 0) {
      digitalWrite(BREAK,HIGH);         
    } else {
      digitalWrite(BREAK,LOW);
    }
    if(direction == 0){
       digitalWrite(DIR, HIGH); 
       lcdPrint(0,0,"Dir: 0");
    } else {
      digitalWrite(DIR,LOW); 
      lcdPrint(0,0,"Dir: 1");

    }
    
    analogWrite(SPEED,speed); 

    lcdPrint(0,1,"                ");
    int n = sprintf(buff,"%d",speed);
    lcdPrint(0,1,"Speed: ");
    lcdPrint(7,1,buff);
    n = sprintf(buff,"%d",keypadInput);
    lcdPrint(12,0,"     ");
    lcdPrint(12,0,buff);
}

unsigned normalize(unsigned i){
  return (255 * (i/100.0));
}

/*-----( Function )-----*/
void translateIR() // takes action based on IR code received

// describing Remote IR codes 

{

  switch(results.value)

  {
  case 0xFFA25D: Serial.println("POWER"); speed=0; break;
  case 0xFFE21D: Serial.println("FUNC/STOP"); controlledStop = true; break;
  case 0xFF629D: Serial.println("VOL+"); break;
  case 0xFF22DD: Serial.println("FAST BACK");    direction=1;   break;
  case 0xFF02FD: Serial.println("PAUSE");    break;
  case 0xFFC23D: Serial.println("FAST FORWARD"); direction=0;   break;
  case 0xFFE01F: Serial.println("DOWN"); if(speed >= 10) speed -= 10;   break;
  case 0xFFA857: Serial.println("VOL-");    break;
  case 0xFF906F: Serial.println("UP");   if(speed <= 245) speed += 10;  break;
  case 0xFF9867: Serial.println("EQ"); speed = normalize(keypadInput); keypadInput = 0;   break;
  case 0xFFB04F: Serial.println("ST/REPT");   startup = true; break;
  case 0xFF6897: Serial.println("0"); keypadInput *=10;    break;
  case 0xFF30CF: Serial.println("1"); keypadInput *=10; keypadInput +=1;   break;
  case 0xFF18E7: Serial.println("2"); keypadInput *=10; keypadInput +=2;   break;
  case 0xFF7A85: Serial.println("3"); keypadInput *=10; keypadInput +=3;   break;
  case 0xFF10EF: Serial.println("4"); keypadInput *=10; keypadInput +=4;   break;
  case 0xFF38C7: Serial.println("5"); keypadInput *=10; keypadInput +=5;   break;
  case 0xFF5AA5: Serial.println("6"); keypadInput *=10; keypadInput +=6;   break;
  case 0xFF42BD: Serial.println("7"); keypadInput *=10; keypadInput +=7;   break;
  case 0xFF4AB5: Serial.println("8"); keypadInput *=10; keypadInput +=8;   break;
  case 0xFF52AD: Serial.println("9"); keypadInput *=10; keypadInput +=9;   break;
  case 0xFFFFFFFF: Serial.println(" REPEAT");break;  

  default: 
    Serial.println(" other button   ");

  }// End Case
  Serial.println(keypadInput);
  delay(500); // Do not get immediate repeat


} //END translateIR 
