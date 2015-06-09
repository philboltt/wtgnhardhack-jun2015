#include <SoftwareSerial.h>
#include <Wire.h>

// Include the libraries required for triggering alarms
#include <Time.h>
#include <TimeAlarms.h>

// Include the GPRS/GSM library and perform basic configuration
#include <GPRS_Shield_Arduino.h>
#include <sim900_Suli.h>
#include <Suli.h>

#define PIN_TX    7
#define PIN_RX    8
#define BAUDRATE  9600
#define PHONE_NUMBER "#####" //!! Change this to the phone receiving the alert
#define MESSAGE  "Oh no!  You forgot your pills"
GPRS gprsTest(PIN_TX,PIN_RX,BAUDRATE);//RX,TX,BaudRate

// Assign the data pins
const int buttonPin = 9;     // the number of the pushbutton pin
const int buzzerPin = 10;    // the number of the buzzer pin
const int ledPin1 =  11; // the number of the LED pin
const int ledPin2 =  12;
const int ledPin2 =  13;

int buttonState = 0;
int volume = 0; // volume of the buzzering

// Alarm structure - a bit leftover from the attempts to get the RTC brick working
struct Plarm {
  int hour;
  int minute;
  int duration;
  int LED;
};
Plarm pAlarms[3];

void setup() {
  // initialize the LED pins as OUTPUT:
  pinMode(ledPin1, OUTPUT);  
  pinMode(ledPin2, OUTPUT); 
  pinMode(ledPin3, OUTPUT); 
  // initialize the pushbutton pin as an INPUT:
  pinMode(buttonPin, INPUT);  
  // initialize buzzer pin as OUTPUT
  pinMode(buzzerPin,OUTPUT);
  // initialize clock
  Serial.begin(9600);
  setTime(12,0,0,6,6,15); // set time to saturday 6 June midday
  // create the dummy alarms 
  Alarm.alarmRepeat(12,1,0, PillAlarm1);  // morning
  Alarm.alarmRepeat(12,2,0, PillAlarm2);  // midday
  Alarm.alarmRepeat(12,3,0, PillAlarm3);  // evening
  // Set the alarms metadata (hardcoded) - hour and minute are ignored in this version
  Plarm a1 = {16,8,10,1};
  Plarm a2 = {16,9,10,2};
  Plarm a3 = {16,10,10,3};
  pAlarms[0] = a1;
  pAlarms[1] = a2;
  pAlarms[2] = a3;
  // initialize the GPRS/GSM
  while(0 != gprsTest.init()) {
      delay(1000);
      Serial.print("init error\r\n");
  }  
  Serial.println("gprs init success");
  Serial.println("start to send message ...");
}

void loop(){
  digitalClockDisplay();
  Alarm.delay(1000); // wait one second between clock display
}

void PillAlarm1() {
  PillAlarm(1);
}

void PillAlarm2() {
  PillAlarm(2);
}

void PillAlarm3() {
  PillAlarm(3);
}

void PillAlarm(int alarmId) {
  Plarm pAlarm = pAlarms[alarmId];
  bool triggerAlarm = true;
  if (triggerAlarm) {
    // this loop continues for the alarm duration while checking if the button
    // has been pressed.  If so, it disables the buzzer while keeping the LED lit.
    // If not, it sends an SMS alert.
    if (pAlarm.LED == 3) {
      digitalWrite(ledPin3, HIGH);
    } else if (pAlarm.LED == 2) {
      digitalWrite(ledPin2, HIGH);
    }  else if (pAlarm.LED == 1) {
      digitalWrite(ledPin1, HIGH);
    }
    int i=0;
    while (i < pAlarm.duration) {
      if (triggerAlarm) {
        digitalWrite(buzzerPin,HIGH);
        delay(500);
        digitalWrite(buzzerPin,LOW);
        delay(500);
        buttonState = digitalRead(buttonPin);
        if (buttonState == HIGH) {
          triggerAlarm=false;
        }
      } else {
        delay(1000);
      }
      i++;
    }
    // triggerAlarm should be false if the patient has pressed the button.
    if (triggerAlarm) {
      gprsTest.sendSMS(PHONE_NUMBER,MESSAGE);
    }
    // switch the LED off
    if (pAlarm.LED == 3) {
      digitalWrite(ledPin3, LOW);
    } else if (pAlarm.LED == 2) {
      digitalWrite(ledPin2, LOW);
    }  else if (pAlarm.LED == 1) {
      digitalWrite(ledPin1, LOW);
    }
  }
}

void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println(); 
}


void printDigits(int digits)
{
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
 
