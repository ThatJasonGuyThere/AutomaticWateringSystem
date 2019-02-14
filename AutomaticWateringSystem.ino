/* -----------------------------------------------------------------
  Automatic Watering System for Willowbank School 

  Created July 2017 as part of SouthSci - Curious Minds
 
  With help from Fisher & Paykel Healthcare

  ---------

  This automatic watering system monitors the soil moisture of plants and waters them 
  when they becomes dry. 

  Questions: vicky.gao@fphcare.co.nz
             andrew.fan@fphcare.co.nz
             alicia.evans@fphcare.co.nz
             matt.stephenson@fphcare.co.nz
             sarah.morgan@cometauckland.org.nz
-------------------------------------------------------------------- */ 

//Define the pins for the moisture sensors
int sensorPins[] = {A10, A11, A12, A13, A14, A15};

//Define an array for the moisture value of each plant, and their positions for the servo motor
int moistureValues[6];
int motorPositions[] = {0, 25, 50, 75, 100, 120};   //Angle positions of the plants. Change this to adjust for any changes in positions of the plants 

//int sensorPower = 7;   //This is the output pin for powering the moisture sensors
int sensorPower[] = {42, 44, 46, 48, 50, 52};
int pumpPin = 2;   //This is the output pin for powering the water pump

//long modeTimer = 7200000;   //Time delay (in milliseconds). Set the timer to wait 2 hours before taking another moisture reading. Change this number to change time delay
long modeTimer = 5000;

#include <Servo.h>  //Servo library

int thresholdDown = 600;  //The lower moisture reading limit (below this number means the plant is dry)

Servo plantServo;  //Create servo object to control the servo motor

void setup() {
  Serial.begin(9600);   //Opens serial over USB
  delay(500);           //Wait for arduino to boot up
  
  for (int i = 0; i < 6; i++) {
    pinMode(sensorPower[i], OUTPUT);     //Sets the sensor power pin as an output
    digitalWrite(sensorPower[i], LOW);   //Sets the sensor power to LOW so there is no power flowing through the sensor
  }
  pinMode(pumpPin, OUTPUT);     //Sets the pump pin as an output 
  digitalWrite(pumpPin, LOW);   //Sets the pump pin to LOW so there is no power to the water pump

  plantServo.attach(9);

  initialiseMode();             //Decide whether the system will run on "demonstration mode" or "normal mode"
}

void loop() {
  moistureSampling();   //Run the moisture sampling function to take moisture readings from the plants
  for (int i = 0; i < 6; i++)
  {
    if (moistureValues[i] < thresholdDown)  //The plant is dry and needs to be watered
    {
      Serial.println("Plant " + String(i+1) + " is thirsty, needs watering. The moisture value is " + String(moistureValues[i]));
      automaticWatering(i);   //Run the automatic watering function to move the servo to the plant's position and start the water pump
    }
    else  //The plant is wet, nothing needs to be done
    {
      Serial.println("Plant " + String(i+1) + " is wet, does not need watering. The moisture value is " + String(moistureValues[i]));
    }
  }
  delay(modeTimer);  //Time delay (in milliseconds) between taking readings
}

/*This reads the value of the soil moisture and gets an average sample reading */
void moistureSampling()  
{
  delay(500);
  for (int i = 0; i < 6; i ++) {
    digitalWrite(sensorPower[i], HIGH); //Turn the sensor power pin "on"
  }
  delay(500);  //Wait for sensor
  for(int j = 0; j < 6; j++)
  {
    int moistureValue_temp = 0;
    for(int i = 0; i < 30; i++)   //Sample 30 readings in 3 seconds
    {
      moistureValue_temp = moistureValue_temp + analogRead(sensorPins[j]);
      delay(10);
    }
    moistureValues[j] = moistureValue_temp / 30;      //Divide by 30 to get the average reading
  }
  delay(500);
  
  for (int i = 0; i < 6; i ++) {
    digitalWrite(sensorPower[i], LOW); //Turn the sensor power pin "off"
  }
  
  delay(500);
}

/*This sets the automatic watering system into "demonstration mode" when the system is first turned on */
void initialiseMode()   
{
  moistureSampling();   //Take in moisture readings
  if (moistureValues[0] < 100)  //If moisture sensor no. 1 is dry, the system will be put in demo mode
  {
    modeTimer = 5000;   //Sets the time delay between taking readings to 5 seconds (instead of 2 hours) to demonstrate how the system works
  }
}

/*This runs the automatic watering*/
void automaticWatering(int plantNumber)   
{
  moveMotor(motorPositions[plantNumber]);   //Function to move the servo motor to position
  startWatering();  //Function to water the plant
}

/*This moves the servo motor to the position of the plant that needs watering*/
void moveMotor(int Angle)   
{
  plantServo.attach(9);
  plantServo.write(Angle);  //Move the servo motor to the angle of the plant
  delay(2000);
  plantServo.detach();  //Detach the servo motor  
}

/*This turns the water pump on to water the plant*/
void startWatering()  
{
  delay(1000);
  digitalWrite(pumpPin, HIGH);  //Turn the pump "on"
  Serial.println("Pump has started");
  delay(1000);  //Amount of time that the pump is turned on for (change this to change the amount that the plant gets watered)
  digitalWrite(pumpPin, LOW);  //Turn the pump "off"
  delay(2000);
}

