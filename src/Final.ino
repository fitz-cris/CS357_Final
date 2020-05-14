#include <lock.h>

#include <Servo.h>
#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <IRremote.h>
#include <LiquidCrystal.h>

int red_light_pin= 10;
int green_light_pin = 9;
int IR_PIN = 12; //remote input pin
lock combo;
Servo lockArm;
#define ServoPin 11

IRrecv irDetect(IR_PIN);
decode_results irIn;
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

int index = 0;
int enteredCombination[4] = {-1,-1,-1,-1};
String dis = "";
bool setting = false;
String msg = "Please input:";

void setup() {
  Serial.begin(9600);
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  lockArm.attach(ServoPin);
  lockArm.write(0);
  irDetect.enableIRIn(); // Start the Receiver
  lcd.begin( 16, 2 );
}

void loop() {
  lcd.setCursor(0,0);
  lcd.print(msg);
  lcd.setCursor(0, 1);
  digitalWrite(green_light_pin, HIGH);
  digitalWrite(red_light_pin, HIGH);

  //if index is 4, then they have entered a full combination. 
  if(index == 4 && setting ) // Setting a new combination
  {
    combo.resetCombination(enteredCombination);
    delay(1500);
    lcd.clear(); 
    msg = "New code:";
    lcd.print(msg);
    lcd.setCursor(0,1);
    lcd.print(dis);
    delay(3000);
    index=0;
      for(int i = 0; i < 4; i++)
          {
            enteredCombination[i] = -1; //reset combination
          }
      
    msg = "Please input:";
    lcd.clear();
    dis = "";
    setting = false;
  }
  else if(index == 4 && !setting ) //Not setting, therefore must test the combination against the internal combination
    {
      int result = combo.testCombination(enteredCombination); //returns -1 if doesnt match, 1 if it does
      delay(500); 
      if(result < 0) //doesnt match
      {
          lcd.clear();
          msg = "Incorrect";
          lcd.print(msg);
          lcd.setCursor(0, 1);
          digitalWrite(red_light_pin, LOW);
          delay(1500);
          lcd.clear();
          digitalWrite(red_light_pin, HIGH);
          msg = "Please Input:";
      }
      else if(result > 0 && !combo.getState()) //Does match, and lock is currently closed, then open
      {
          lcd.clear();
          msg = "Correct";
          lcd.print(msg);
          lcd.setCursor(0, 1);
          lcd.print("Opening...");
          digitalWrite(green_light_pin, LOW);
          combo.setState(1);
          lockArm.write(180);
          delay(1500);
          digitalWrite(green_light_pin, HIGH);
          lcd.clear();
          msg = "Please Input:";
      }
      else if (result > 0 && combo.getState()) //Does match, and lock is currently open, then close
      {
          lcd.clear();
          msg = "Correct";
          lcd.print(msg);
          lcd.setCursor(0, 1);
          lcd.print("Locking...");
          digitalWrite(green_light_pin, LOW);
          combo.setState(false);
          lockArm.write(0);
          delay(1500);
          digitalWrite(green_light_pin, HIGH);
          lcd.clear();
          msg = "Please Input:";
      }
      //clear combination
      index=0;
      for(int i = 0; i < 4; i++)
          {
            enteredCombination[i] = -1; //reset combination
          }
      
      lcd.clear();
      lcd.setCursor(0, 1);
      dis = "";
    }
    //detect input from remote
  if (irDetect.decode(&irIn)) 
  {
    int current = decodeIR();
    if (current == -2) //# was pressed, want to input new combination
    {
      lcd.clear();
      msg = "Enter new:";
      lcd.print(msg);
      setting = true;
      index=0;
      for(int i = 0; i < 4; i++)
          {
            enteredCombination[i] = -1; //reset combination
          }
      
      dis = "";
    }
    else if(current == -1) //* was pressed, clear input
      {
        index=0;
        lcd.clear();
        dis = "";
        lcd.clear();
        lcd.setCursor(0, 1);
        msg = "Please input:";
      }
    else if( current != -10) // was a number, add to current combination guess
        {
          enteredCombination[index] = current;
          index+=1;
          if(setting)
          {
            dis.concat(current);
            dis.concat(" ");
          }
          else
          {
            dis.concat("* ");
          }
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print(dis);
        }
    
    irDetect.resume(); // Receive the next value
  }
}

int decodeIR() // Indicate what key is pressed
{

  switch(irIn.value)

  {

  case 0xFF629D:  
    Serial.println("Up Arrow"); 
    return -10;
    break;

  case 0xFF22DD:  
    Serial.println("Left Arrow"); 
    return -10;
    break;

  case 0xFF02FD:  
    Serial.println("OK");
    return -10; 
    break;

  case 0xFFC23D:  
    Serial.println("Right Arrow"); 
    return -10;
    break;

  case 0xFFA857:  
    Serial.println("Down Arrow"); 
    return -10;
    break;

  case 0xFF6897:  
    Serial.println("1"); 
    return 1;
    break;

  case 0xFF9867:  
    Serial.println("2"); 
    return 2;
    break;

  case 0xFFB04F:  
    Serial.println("3"); 
    return 3;
    break;

  case 0xFF30CF:  
    Serial.println("4"); 
    return 4;
    break;

  case 0xFF18E7:  
    Serial.println("5"); 
    return 5;
    break;

  case 0xFF7A85:  
    Serial.println("6"); 
    return 6;
    break;

  case 0xFF10EF:  
    Serial.println("7");
    return 7; 
    break;

  case 0xFF38C7:  
    Serial.println("8");
    return 8; 
    break;

  case 0xFF5AA5:  
    Serial.println("9"); 
    return 9;
    break;

  case 0xFF42BD:  
    Serial.println("*");
    return -1; 
    break;

  case 0xFF4AB5:  
    Serial.println("0"); 
    return 0;
    break;

  case 0xFF52AD:  
    Serial.println("#");
    return -2; 
    break; 

  default: 
   break;

  }
}
