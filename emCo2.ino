// Code written by Felix Dolf in cooperation with the Q1 computer science course of AEG Kaarst (Dec 2020)
#include <MHZ19.h>
//Pin setup
const int buzzer=12,green=11,orange=10,red=9,button=6;

//Co2value setup
int co2value;
const int co2low = 1000;
const int co2high = 2000;

//warmup
long warmuptime=60000;
boolean warmupstate=true;

//Buzzer with debounce
int buzzerstate=LOW;
boolean buzzerbool= true,OrangechangeRed=false; 
int buttonState;
int lastButtonState=LOW;
unsigned long lastDebounceTime; 
unsigned long debounceDelay = 50;

//Interval beeping
unsigned long previousMillis=0;
long intervalon =100;
long intervaloff =9900;

//Sensor declaration
MHZ19 myMHZ19;

void setup(){
  //Set pins
  pinMode(buzzer,OUTPUT);
  pinMode(green,OUTPUT);
  pinMode(orange,OUTPUT);
  pinMode(red,OUTPUT);
  pinMode(button,INPUT);

  //Sensor setup
  Serial.begin(9600);
  Serial1.begin(9600);                                // MH-Z19 serial start
  myMHZ19.begin(Serial1);                             // *Serial(Stream) refence must be passed to library begin().
  myMHZ19.autoCalibration();                          // Turn auto calibration ON (OFF autoCalibration(false))
}

void loop(){
  //Store millis
  unsigned long currentMillis = millis();
  
  startwarmup();
  
  //Sensor initialization
  co2value = myMHZ19.getCO2(); 
  Serial.println(co2value);
  
  //Green light if co2value low|reset mute|button unenabled
  if(co2value<co2low){
    setColor(0);
    OrangechangeRed=false;
    buzzerbool=true;
    buzzerstate=LOW;
    digitalWrite(buzzer,buzzerstate);
  }

  //Orange light and beepings per interval if co2value between low and high|possibility to mute buzzer till co2value low
  if(co2value>=co2low&&co2value<co2high){
    reactToButton();
    setColor(1);
    if(buzzerbool==true){     
      if(buzzerstate==LOW&&currentMillis-previousMillis >=intervaloff){
        previousMillis=currentMillis; 
        buzzerstate=HIGH;        
      }
      
      if(buzzerstate==HIGH&&currentMillis-previousMillis >=intervalon){
        previousMillis=currentMillis; 
        buzzerstate=LOW;             
      }
     
      digitalWrite(buzzer,buzzerstate);
      
    } else{
      digitalWrite(buzzer,LOW);
      
    }
  }

  //Red light and constant beeping if co2value high|possibility to mute buzzer till co2value low
  if(co2value>=co2high){
     if(OrangechangeRed==true){
      OrangechangeRed=false;
      buzzerbool=true;;
    }
    reactToButton();
    setColor(2);
    if(buzzerbool==true){
      digitalWrite(buzzer,HIGH);
      
    } else{
      digitalWrite(buzzer,LOW);
      
    }
  }
}

//Debounce
void reactToButton(){
  int reading = digitalRead(button);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
    
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        buzzerbool=false;
        if(co2value>=co2low&&co2value<co2high){
        OrangechangeRed=true;
        }
      }
    }
  }
  lastButtonState = reading;
}

void setColor(int type){
  if(type == 0){
    digitalWrite(green,HIGH);
    digitalWrite(orange,LOW);
    digitalWrite(red,LOW);
    
  } else if(type == 1){
    digitalWrite(green,LOW);
    digitalWrite(orange,HIGH);
    digitalWrite(red,LOW);
    
  } else if(type == 2){
    digitalWrite(green,LOW);
    digitalWrite(orange,LOW);
    digitalWrite(red,HIGH);
  }
}

void startwarmup(){
 unsigned long startmls = millis();
 unsigned long previousmls;
 int lightstate=LOW; 
while(warmupstate==true){
  unsigned long currentmls = millis();
  co2value = myMHZ19.getCO2(); 
  Serial.println(co2value);
  if(currentmls-previousmls >=1000){
    previousmls=currentmls; 
    if(lightstate==LOW){       
        lightstate=HIGH;        
      }else{
         lightstate=LOW;  
      }
  }
  digitalWrite(green,lightstate);
  digitalWrite(orange,lightstate);
  digitalWrite(red,lightstate);
   
  if((currentmls-startmls)>=warmuptime){
    warmupstate=false;
    digitalWrite(buzzer,HIGH);
    delay(500);
    digitalWrite(buzzer,LOW);
    delay(500);
  }
 }
}
