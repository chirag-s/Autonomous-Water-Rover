# /*
#   *
#   * Project Name:     Autonomous Water Rover
#   * Author List:      Chirag Shah
#   * Filename:        dataAquisition.ino
#   * Functions:        averagearray(), phRead(),rainRead(),turbRead(),tempRead()
#   * Global Variables: pH,turb,rain,Offset, samplingInterval, printInterval, ArrayLenth,
                        LED ,trigPin1,echoPin1,trigPin2,echoPin2 ,duration,distance,duration2,distance2;
                        pHArray[ArrayLenth],pHArrayIndex=0,turbArray[ArrayLenth],turbArrayIndex=0,tempArray[ArrayLenth],tempArrayIndex,
                        rainArray[ArrayLenth],rainArrayIndex, rainVoltage,pHValue,turbVoltage,temp, ONE_WIRE_BUS 2
#   * Description: This file is used to read sensor data from the sensors and apply an averaging filter to them
# */
#include <OneWire.h> 
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2 //Data of temp sensor at pin2

OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

#define pH A0            //pH meter Analog output to Arduino Analog Input 0
#define turb A1   //turbidity meter Analog output to Arduino Analog Input 1
#define rain A2  //conductivity meter Analog output to Arduino Analog Input 2
#define Offset 0.13            //deviation compensate for pH
#define samplingInterval 20 //Time after which reading is noted
#define printInterval 8000 //Time after which data is sent to pi
#define ArrayLenth  40    //times of collection
#define LED 13
#define MinPulseLength 1100  // these are RC output obtained fromm Navio2
#define MaxPulseLength 1900

int RCinputPinRev = 3;//physical pin mapping on uno
int pin3interruptRev = 1; //ext interrupt 1
int PWM_PinRev = 5; // PWM output pins
unsigned long volatile RCpulseLastChangeTime = 0; //Timeout constant
int pulseLengthRev=0;

const int trigPin1 = 8;//m1/Left side 
const int echoPin1 = 10;
const int trigPin2 = 11;//m2/Right side
const int echoPin2 = 12;
long duration; //Left distance sensor pulse duration
int distance; //Left distance sensor reading in cm after computation
long duration2; //Right distance sensor pulse duration
int distance2; ////Right distance reading in cm after computation
int dL=4; //distance pin indicator
int dR=6; //distance pin indicator

int pHArray[ArrayLenth];   //All arrays store the average value of the sensor feedback
int pHArrayIndex=0;       //All index vars store the index at which next reading is to be added
int turbArray[ArrayLenth];
int turbArrayIndex=0; 
int tempArray[ArrayLenth];
int tempArrayIndex=0;
int rainArray[ArrayLenth];
int rainArrayIndex=0; 
float rainVoltage,pHValue,turbVoltage,temp;  //Final readings should be stored


void setup(void)
{
  pinMode(dL, OUTPUT);
  digitalWrite(dL,LOW);
  pinMode(dR, OUTPUT);
  digitalWrite(dR,LOW);
  pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin1, INPUT); // Sets the echoPin as an Input
  pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin2, INPUT); // Sets the echoPin as an Input
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  Serial.begin(9600); 
  pinMode(PWM_PinRev, OUTPUT);
  attachInterrupt(pin3interruptRev, pinChangeISRRev, CHANGE); // call the function pinChangeISR each time the input changes state 
  sensors.begin();
 }
void loop(void)
{
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  //After 20 ms get sensor data also send distance sensor readings 
  if(millis()-samplingTime > samplingInterval)
  {
     phRead();
     turbRead();
     tempRead();
     rainRead();
     samplingTime=millis();
  }
  //After 80 ms send all sensor readings
  if(millis() - printTime > printInterval)   //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
  {
        d1();
        d2();
        Serial.print(distance);
        Serial.print(' ');
        Serial.println(distance2);
        Serial.print(temp);
        Serial.print(' ');
        Serial.print(pHValue);
        Serial.print(' ');
        Serial.print(turbVoltage);
        Serial.print(' ');
        Serial.println(rainVoltage);
        digitalWrite(LED,digitalRead(LED)^1);
        printTime=millis();
  }
  if(pulseLengthRev<1200)
  { 
    //Serial.println(pulseLengthRev);
    analogWrite(PWM_PinRev, 255);
  }
  else if(pulseLengthRev>1800)
  { 
    //Serial.println(pulseLengthRev);
    analogWrite(PWM_PinRev,0);
  }
  else
  { 
   // Serial.println(pulseLengthRev);
    analogWrite(PWM_PinRev, 255);
  }
     
    if(distance<70)
    { 
      //Serial.println(distance);
      digitalWrite(dL, HIGH);
    }
    else 
    { 
      //Serial.println(distance);
      digitalWrite(dL, LOW);
    }
  
    if(distance2<70)
    { 
      //Serial.println(distance2);
      digitalWrite(dR, HIGH);
    }
    else 
    { 
      //Serial.println(distance2);
      digitalWrite(dR, LOW);
    }
    if(distance2<50 && distance<50)
    {
        analogWrite(PWM_PinRev,0);
    }
      
}
/*
  *
  * Function Name: d1()
  * Input:    none
  * Output:     none
  * Logic:   Trigger the ultrasound sensor on left side, read the hight time of echo pin and accordingly compute distance using 
  *          Speed=distance/time
  * Example Call:   d1()
  *
  */ 
void d1() 
{
  // Clears the trigPin
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration = pulseIn(echoPin1,HIGH);
  // Calculating the distance
  distance= duration/29/2;
  // Prints the distance on the Serial Monitor
}
/*
  *
  * Function Name: d2()
  * Input:    none
  * Output:     none
  * Logic:   Trigger the ultrasound sensor on right side, read the hight time of echo pin and accordingly compute distance using 
  *          Speed=distance/time
  * Example Call:   d2()
  *
  */ 
void d2() 
{
  // Clears the trigPin
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  // Calculating the distance
  distance2= duration2/29/2;
  // Prints the distance on the Serial Monitor
}
/*
  *
  * Function Name: tempRead()
  * Input:    none
  * Output:     none
  * Logic:   Get temperature reading in the array and calculate its average 
  * Example Call:   tempRead()
  *
  */ 
void tempRead()
{
  sensors.requestTemperatures();
  tempArray[tempArrayIndex++]=sensors.getTempCByIndex(0)*100;
  if(tempArrayIndex==ArrayLenth)tempArrayIndex=0;
  temp = averagearray(tempArray, ArrayLenth);
  temp = temp/100; 
}
/*
  *
  * Function Name: turbRead()
  * Input:    none
  * Output:     none
  * Logic:   Get turbidity reading in the array and calculate its average. Convert ADC to voltage between 0 to 5
  * Example Call:   turbRead()
  *
  */ 
void turbRead()
{
  turbArray[turbArrayIndex++]=analogRead(turb);
  if(turbArrayIndex==ArrayLenth)turbArrayIndex=0;
  turbVoltage = averagearray(turbArray, ArrayLenth)*5.0/1024;
}
/*
  *
  * Function Name: rainRead()
  * Input:    none
  * Output:     none
  * Logic:   Get conductivity reading in the array and calculate its average. Convert ADC to voltage between 0 to 5
  * Example Call:   rainRead()
  *
  */ 
void rainRead()
{
  rainArray[rainArrayIndex++]=analogRead(rain);
  if(rainArrayIndex==ArrayLenth)rainArrayIndex=0;
  rainVoltage = averagearray(rainArray, ArrayLenth)*5.0/1024;
}
/*
  *
  * Function Name: phRead()
  * Input:    none
  * Output:     none
  * Logic:   Get ph reading in the array and calculate its average. Convert to ADC to voltage between 0 to 5
  * Example Call:   phRead()
  *
  */ 
void phRead()
{
   pHArray[pHArrayIndex++]=analogRead(pH);
   if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
   int voltage = averagearray(pHArray, ArrayLenth)*5.0/1024;
   pHValue = 3.5*voltage+Offset;
}
/*
  *
  * Function Name: averagearray()
  * Input:    array name, arraylength
  * Output:     none
  * Logic:   Get average of the entire array
  * Example Call:   averagearray(rainArray, ArrayLenth)
  *
  */ 
double averagearray(int* arr, int number)
{
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   
    //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}


void pinChangeISRRev()  //ISR to get servo compatible pulse width and convert to mosfet compatible pwm 
{
  unsigned long timeNow = micros(); // record time
  if(digitalRead(RCinputPinRev)) // pulse now high
  {
    RCpulseLastChangeTime = timeNow; // save the time when the pulse went high
  }
  else // pulse now low
  {
    pulseLengthRev = int(timeNow - RCpulseLastChangeTime); // measure pulse length
    pulseLengthRev  = constrain(pulseLengthRev, MinPulseLength, MaxPulseLength); // correct bad readings
   // Serial.print(pulseLengthRev);
   // Serial.print("----->Rev");
   // Serial.println(map(pulseLengthRev, MinPulseLength, MaxPulseLength, 255, 0));
    
  }
}
