/*
 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>

const int OFF_Btn = 5;        
const int ON_Speed_Btn = 6;
const int RECV_PIN = 7;      //receiving IR signal
//const int Timer_Btn = 8;
const int Buzzer = 9;
const int Speed_1 = 10;
const int Speed_2 = 11;
const int Speed_3 = 12;
const int Timer_LED = 0;  //changed from 13 to 0 port
unsigned int interval = 0;         // timer incremental 65535:3 = 21845; 21845 x 82 = 1.791.290
                          // buoc nhay hen gio 30p ~ 1.800.000 ms
unsigned long real_interval = 0;    // real incremental for timer
boolean timer_flag = false;        // flag for timer status
unsigned long startTimer = 0;      // thoi gian bat dau tinh de hen gio
//int TimerOff = 0;    //variable for timer to OFF
int CurrentSpeed = 1;   // variable for the last #speed 1, 2 or 3)
boolean running = false;
int ON_Btn_State = 0;         // current state of the ON button
int last_ON_Btn_State = 0;     // previous state of the ON button
int OFF_Btn_State = 0;
int last_OFF_Btn_State = 0;
int count = 0;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  Serial.begin(9600);
  pinMode(OFF_Btn, INPUT);    //inputs
  pinMode(ON_Speed_Btn, INPUT);
  pinMode(RECV_PIN, INPUT);
  //pinMode(Timer_Btn, INPUT);
  pinMode(Buzzer, OUTPUT); //outputs
  pinMode(Speed_1, OUTPUT); 
  pinMode(Speed_2, OUTPUT);
  pinMode(Speed_3, OUTPUT);
  pinMode(Timer_LED, OUTPUT);
  irrecv.enableIRIn(); // Start the receiver
}

void SetSpeed ()
{
  switch (CurrentSpeed) 
  {
   case 1:    
    if (running == true) //then turns ON #2
    {
      digitalWrite(Speed_1, LOW);
      digitalWrite(Speed_3, LOW);
      digitalWrite(Speed_2, HIGH);
      CurrentSpeed = 2;
    }
    else    //turns ON #1
    {
      digitalWrite(Speed_2, LOW);
      digitalWrite(Speed_3, LOW);
      digitalWrite(Speed_1, HIGH);
      CurrentSpeed = 1;
      running = true;  
    }
    break;
    
   case 2:    
    if (running == true) //then turns ON #3
    {
      digitalWrite(Speed_1, LOW);
      digitalWrite(Speed_2, LOW);
      digitalWrite(Speed_3, HIGH);
      CurrentSpeed = 3;
    }
    else    //turns ON #2
    {
      digitalWrite(Speed_1, LOW);
      digitalWrite(Speed_3, LOW);
      digitalWrite(Speed_2, HIGH);
      CurrentSpeed = 2;
      running = true;  
    }
    break;

   case 3:    
    if (running == true) //then turns ON #1
    {
      digitalWrite(Speed_2, LOW);
      digitalWrite(Speed_3, LOW);
      digitalWrite(Speed_1, HIGH);
      CurrentSpeed = 1;
    }
    else    //turns ON #3
    {
      digitalWrite(Speed_1, LOW);
      digitalWrite(Speed_2, LOW);
      digitalWrite(Speed_3, HIGH);
      CurrentSpeed = 3;
      running = true;  
    }
    break;
  }
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
  
  switch (results.value) {
  case 0xFFFFFFFF:
    Serial.print ("tin hieu 0xFFFFFF");
    break;
  case 0xFFA25D:    // receiving OFF signal from Remoter   
    digitalWrite(Buzzer, HIGH);      // buzzing for 15ms
    delay (15);
    digitalWrite(Buzzer, LOW);
    if (running == true) //then turns all OFF
    {
      digitalWrite(Speed_1, LOW);
      digitalWrite(Speed_2, LOW);
      digitalWrite(Speed_3, LOW);
      running = false;
      Serial.print("nhan tin hieu OFF, running = True");
      Serial.print (CurrentSpeed,DEC);
    }
    else    // breaks the loop
    {
      Serial.print("nhan tin hieu OFF, running = False");
      Serial.print (CurrentSpeed,DEC); 
    }
    break;
    //Serial.print ('Button 1, count:');
    //Serial.print (count,0);
    //count = count + 1;
  case 0xFFE21D:    // receiving ON/Speed signal from Remoter
    digitalWrite(Buzzer, HIGH);      // buzzing for 15ms
    delay (15);
    digitalWrite(Buzzer, LOW);
    //timer_flag = false;
    SetSpeed ();
    Serial.print (CurrentSpeed,DEC);
    break; 
  case 0xFF629D:    // receiving Timer signal from Remoter   
    digitalWrite(Buzzer, HIGH);      // buzzing for 15ms
    delay (15);
    digitalWrite(Buzzer, LOW);
    if (interval == 65535)
      {
      timer_flag = false;
      interval = 0;
      Serial.print ("timer_flag is:  ");
      Serial.println (timer_flag);
      digitalWrite(Timer_LED, LOW);
      break;
      }
    interval = interval + 21845;
    real_interval = interval; //FOR TEST must be *82
    timer_flag = true;
    startTimer = millis ();
    Serial.print ("interval is:  ");
    Serial.println (interval);
    Serial.print ("timer_flag is:  ");
    Serial.println (timer_flag);
    digitalWrite(Timer_LED, HIGH);
    break;
  default:
    break;
  } 
    irrecv.resume(); // Receive the next value
  }
  delay(10);          //delay for stable?

  ON_Btn_State = digitalRead(ON_Speed_Btn);  // read the ON_Btn_State input pin: 6
  if (ON_Btn_State != last_ON_Btn_State)     // compare the buttonState to its previous state
    {
    if (ON_Btn_State == HIGH)                // if the ON button state has changed from LOW to HIGH
      {
      digitalWrite(Buzzer, HIGH);      // buzzing for 15ms
      delay (15);
      digitalWrite(Buzzer, LOW);
      SetSpeed ();
      //timer_flag = false;
      count++;
      Serial.println("on");
      Serial.print("number of button pushes:  ");
      Serial.println(count);
      } 
    else 
      {
      // if the current state is LOW then the button
      // wend from on to off:
      Serial.println("off"); 
      }
    }
  last_ON_Btn_State = ON_Btn_State;            // save the current state as the last state, for next time through the loop
  
  OFF_Btn_State = digitalRead(OFF_Btn);  // read the OFF_Btn_State input pin: 5
  if (OFF_Btn_State != last_OFF_Btn_State && OFF_Btn_State == HIGH)    
    {
      digitalWrite(Speed_1, LOW);
      digitalWrite(Speed_2, LOW);
      digitalWrite(Speed_3, LOW);
      digitalWrite(Buzzer, HIGH);      // buzzing for 15ms
      delay (15);
      digitalWrite(Buzzer, LOW);
      running = false; 
    }
  last_OFF_Btn_State = OFF_Btn_State;            // save the current state as the last state, for next time through the loop
  
  if (timer_flag == true && (millis () - startTimer >= real_interval))  
    {
       digitalWrite(Speed_1, LOW);
       digitalWrite(Speed_2, LOW);
       digitalWrite(Speed_3, LOW);
       running = false;
       //Serial.println(interval);
       // Serial.println(" Timer flag:  ");
       // Serial.println(timer_flag);
       timer_flag = false;
       digitalWrite(Timer_LED, LOW);
       digitalWrite(Buzzer, HIGH);      // buzzing for 15ms
       delay (15);
       digitalWrite(Buzzer, LOW);
    }  
}
