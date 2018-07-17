# /*
#   *
#   * Project Name:     Autonomous Water Rover
#   * Author List:      Chirag Shah
#   * Filename:         RCtoPWM.py
#   * Functions:        None Interrpt service routine is used.
#   * Global Variables:  MinPulseLength, MaxPulseLength, RCinputPinL, pin3interruptL,
#   *                    RCinputPinR, pin3interruptR, PWM_PinL, PWM_PinR, RCpulseLastChangeTime 
#   * Description: Converts RC output i.e. pulse between 1100 microseconds to 1900 microseconds \
#   *              from navio to normal PWM i.e. 0 to 255 (0V to 5V) 
# */
 
#define MinPulseLength 1100  // these are RC output obtained fromm Navio2
#define MaxPulseLength 1900

int RCinputPinL = 3;//physical pin mapping on uno
int pin3interruptL = 1; //ext interrupt 1
int RCinputPinR = 2; //physical pin mapping on uno
int pin3interruptR = 0; //ext interrupt 0

int PWM_PinL = 6; // PWM output pins
int PWM_PinR = 5;

int pulseLengthL=1100; 
int pulseLengthR =1100;

int lobstacle=10;
int robstacle=11;

unsigned long volatile RCpulseLastChangeTime = 0; //Timeout constant

void setup() {
  Serial.begin(9600);
  pinMode(lobstacle, INPUT);
  pinMode(robstacle, INPUT);  
  pinMode(PWM_PinL, OUTPUT);
  attachInterrupt(pin3interruptL, pinChangeISRL, CHANGE); // call the function pinChangeISR each time the input changes state
  pinMode(PWM_PinR, OUTPUT);
  attachInterrupt(pin3interruptR, pinChangeISRR, CHANGE); // call the function pinChangeISR each time the input changes state
}

void loop() {
  
  // signal drop safety check
  if(micros() - RCpulseLastChangeTime > 500000) // check if it has been more then 0.5s since last pulse was seen
  {
    analogWrite(PWM_PinL, 255); // set PWM outputs to 255
    analogWrite(PWM_PinR, 255);
  }
  if(pulseLengthR < pulseLengthL)
  {
    if(pulseLengthR==1100 && pulseLengthL==1900 )
    { 
        Serial.println("Hard Right");
        analogWrite(PWM_PinR, 255);
        analogWrite(PWM_PinL,10);
    }
    else
    {
        Serial.println("Soft Right");
        analogWrite(PWM_PinR,240);
        analogWrite(PWM_PinL,0);
           
    }
  }
  else if(pulseLengthL < pulseLengthR)
  {
    if(pulseLengthL==1100 && pulseLengthR==1900 )
    { 
        Serial.println("Hard Left");
        analogWrite(PWM_PinL, 255);
        analogWrite(PWM_PinR,10);
    }
    else
    {
        Serial.println("Soft Left");
        analogWrite(PWM_PinL, 250);
        analogWrite(PWM_PinR,10);
      
    }
  }
  else
  {
    if(pulseLengthL==1100 && pulseLengthR==1100 )
    { 
        Serial.println("Stop or reverse");
        analogWrite(PWM_PinL, 255);
        analogWrite(PWM_PinR,255);
    }
    else if(pulseLengthL==1900 && pulseLengthR==1900 )
    { 
        Serial.println("Forward");
        analogWrite(PWM_PinL, 0);
        analogWrite(PWM_PinR,60);
    }
  }
  if(digitalRead(lobstacle)==HIGH && digitalRead(robstacle)==HIGH)
  {
   // Serial.println("obstacle fwd");
   /* analogWrite(PWM_PinL, 255);
    analogWrite(PWM_PinR, 255);*/
  }
  else
  {
    if(digitalRead(lobstacle)==HIGH)
    {
       //Serial.println("obstacle left");
       /*
       analogWrite(PWM_PinL, 0);
       analogWrite(PWM_PinR, 255);*/
    }
    if(digitalRead(robstacle)==HIGH)
    {
       //Serial.println("obstacle right");
     /*  analogWrite(PWM_PinR, 0);
       analogWrite(PWM_PinL, 255);*/
    }
  }
  /*
    Serial.print(pulseLengthR);
    Serial.print("----->R");
    Serial.println(map(pulseLengthR, MinPulseLength, MaxPulseLength,255, 0));
    Serial.print(pulseLengthL);
    Serial.print("----->L");
    Serial.println(map(pulseLengthL, MinPulseLength, MaxPulseLength, 255, 0));*/
  delay(100); // wait 0.1s before checking again
}

void pinChangeISRL()  //ISR to get servo compatible pulse width and convert to mosfet compatible pwm 
{
  unsigned long timeNow = micros(); // record time
  if(digitalRead(RCinputPinL)) // pulse now high
  {
    RCpulseLastChangeTime = timeNow; // save the time when the pulse went high
  }
  else // pulse now low
  {
    pulseLengthL = int(timeNow - RCpulseLastChangeTime); // measure pulse length
    pulseLengthL  = constrain(pulseLengthL, MinPulseLength, MaxPulseLength); // correct bad readings
    
    //analogWrite(PWM_PinL, map(pulseLengthL, MinPulseLength, MaxPulseLength, 255, 0)); // update PWM signal
    //analogWrite(PWM_PinL, 255);
  }
}
void pinChangeISRR()    //ISR to get servo compatible pulse width and convert to mosfet compatible pwm 
{
  unsigned long timeNow = micros(); // record time
  
  if(digitalRead(RCinputPinR)) // pulse now high
  {
    RCpulseLastChangeTime = timeNow; // save the time when the pulse went high
  }
  else // pulse now low
  {
    pulseLengthR = int(timeNow - RCpulseLastChangeTime); // measure pulse length
    pulseLengthR  = constrain(pulseLengthR, MinPulseLength, MaxPulseLength); // correct bad readings
    //analogWrite(PWM_PinR, map(pulseLengthR, MinPulseLength, MaxPulseLength, 255,0)); // update PWM signal
    //analogWrite(PWM_PinR, 255);
  }
}
