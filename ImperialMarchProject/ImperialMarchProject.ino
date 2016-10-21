//Title: Robotics Lab 2: Jam Band
//Description: This code will play the Imperial March on command from the web server 
//Team: Tophat - Nathan Pilbrough, Pedro Sierra, Quixin Li

//General #defines 
#include "Timer.h"
#include <Servo.h>
#define echoPin D1 // Echo Pin
#define trigPin D2 // Trigger Pin
#define LEDPin D3 // Onboard LED
#define buttonPin D4 // Button on D4
#define position1 80
#define position2 100
#define BUZZER_PIN  D7

//defines for the first part of the melody
#define G4    392
#define Eb4   311
#define Bb4   466
#define D5    587
#define Eb5   622
#define Fs4   370  
#define NO_NOTE    100

//Instantiate variables and objects 
Servo servo1;
Servo servo2;
int maximumRange = 15; // Maximum range needed
int minimumRange = 1; // Minimum range needed
long duration, distance; // Duration used to calculate distance
char str[4]; 
int buttonState=0;
int i = 0;
int j = 0;
long int globalTime = 0;
long int noteTime = 0;
long int servoNoteTime = 0;
long int rangeFinderTime = 0;
long int rangeFinderPeriod = 10;
long int debounceTime = 0;
long int debouncePeriod = 50;
double modDistance = 0.0;
int runThrough = 0;
Timer t;
bool playingTone = false;
bool buzzerFlag = false;
bool servoFlag = false;
bool playCommand = false; 
float scaler = 1;
float noteScaler = 1;

//Meldogy array
int note[ ][2] = {
  {G4, 80},
  {NO_NOTE,28},
  {G4, 80},
  {NO_NOTE,28},
  {G4, 80},
  {NO_NOTE,25},
  {Eb4, 47},
  {NO_NOTE,25},
  {Bb4, 12},
  {NO_NOTE,9},
  {G4, 80},
  {NO_NOTE,28},
  {Eb4, 47},
  {NO_NOTE,25},
  {Bb4, 12},
  {NO_NOTE,9},
  {G4, 187},
  {NO_NOTE,16}, //18th note = 818
  
  {D5, 80},
  {NO_NOTE,28},
  {D5, 80},
  {NO_NOTE,28},
  {D5, 80},
  {NO_NOTE,25}, //24
  {Eb5, 47},
  {NO_NOTE,25},
  {Bb4, 12},
  {NO_NOTE,9},
  {Fs4, 80},
  {NO_NOTE,28},
  {Eb4, 47},
  {NO_NOTE,25},
  {Bb4, 12},
  {NO_NOTE,9},
  {G4, 187},
  {NO_NOTE,16}, //36th note
};

//UnderTone array
int servoNote[][2]{
  {position2,84}, //note1 
  {position1,59}, //note2
  {position2,25}, //note3
  {position1,25}, //note4
  {position2,49}, //note5
  {position1,25}, //note6
  {position2,25}, //note7
  {position1,25}, //note8
  {position2,25}, //note9
  {position1,67}, //note10
};

//int arraysize = (sizeof(note) - 2);

void setup() {
  //Initial setup:
  //create a timer object that calls increment timer every 10 ms
  t.every(10,IncrementTimer);
  pinMode(4, OUTPUT);
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LEDPin, OUTPUT); // Use LED indicator (if required)
  pinMode(buttonPin, INPUT);   
  servo1.attach(D8);
  servo2.attach(D6); 
  distance=5;
  servo1.write(70);
  servo2.write(70);
  
}

//Function to play a note on a pin at a given frequency 
void toneDIY(uint8_t _pin, unsigned int frequency) {
analogWriteFreq(frequency);
analogWrite(_pin,500);
}

//Function to stop note playback
void noToneDIY(uint8_t _pin) {
analogWrite(_pin,0);
}

//Determine if it is necessary to change the note 
void UpdateTone(){
  //play tone
  if(runThrough >= 2){ // if it is time to play the melody, then continue
    if(playingTone == false){
    if(note[i][0] != NO_NOTE){
     //if the note is not a pause then play the note 
    toneDIY(BUZZER_PIN,note[i][0]);
    }
    playingTone = true;
    }
    //if the time for the note has elapsed then set a flag and reset the timer
    if((globalTime - noteTime) >= (note[i][1]/noteScaler))
    {
      buzzerFlag = true;
      noteTime = globalTime;
    }
    //if the flag is set then stop playing the note and increment the index
    if(buzzerFlag)
    {
      buzzerFlag = false;
      noToneDIY(BUZZER_PIN);
      playingTone = false;
      i++;
      if(i == 36){
        i = 0;
        //Serial.write("Note 36:");
        //Serial.println(globalTime);
      }
      if(i == 18){
       //Serial.write("Note 18:");
       //Serial.println(globalTime);
      }
      /// good up to here!!! 
      
    }
  }
  //if the time for the note has elapsed then set a flag and reset the timer
  if((globalTime - servoNoteTime) >= (servoNote[j][1]/scaler))
    {
      servoFlag = true;
      servoNoteTime = globalTime;
    }
//if the flag is set then stop playing the note and increment the index
if(servoFlag)
{
  servoFlag = false;
  servo1.write(servoNote[j][0]);
  servo2.write(servoNote[j][0]);
  j++;
  if(j==10)
  {
    j = 0;
    runThrough++;
  } 
}
  
}

//function to act as a tick timer, this function is called 
//every 10 ms as defined in the t.every fuction 
void IncrementTimer(){
  globalTime++;
}

void FindRange(){
  /* The following trigPin/echoPin cycle is used to determine the
 distance of the nearest object by bouncing soundwaves off of it. */ 
 digitalWrite(trigPin, LOW); 
 delayMicroseconds(2); 

 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10); 
 
 digitalWrite(trigPin, LOW);
 duration = pulseIn(echoPin, HIGH);
 
 //Calculate the distance (in cm) based on the speed of sound.
 distance = duration/58.2;
 
 if (distance >= maximumRange || distance <= minimumRange){
 /* Send a negative number to computer and Turn LED ON 
 to indicate "out of range" */
 digitalWrite(LEDPin, HIGH); 
 }
 else {
 /* Send the distance to the computer using Serial protocol, and
 turn LED OFF to indicate successful reading. */
 //Serial.println(distance);
 digitalWrite(LEDPin, LOW); 
  }
 }

void UpdateRange(){
  if((globalTime - rangeFinderTime) >= rangeFinderPeriod){
    FindRange();
    rangeFinderTime = globalTime;
  }
}
//function to update the tempo of the tune 
void UpdateTempo()
{
  if(digitalRead(buttonPin)==LOW)
  {
    if((globalTime - debounceTime) >= debouncePeriod)
    {
      if(distance > 0)
      {

       if((distance > 3) and (distance < 7))
       {
        scaler = 2;
        noteScaler =2; 
        Serial.println("scaler 2");
       }else
       {
        scaler = 1;
        noteScaler = 1;
       };

      }
      debounceTime = globalTime;
    }
  }
}
//funciton to handle communication
void UpdateComms()
{
  char message[1];
  if (Serial.available()) 
  {
 
    message[0] = Serial.read();

    if(message[0] == 'P'){
      //if a P character is read then set the play flag
      playCommand = true;
    }
    if(message[0] == 'S'){
      //if an S character is read then lower the play flag
      playCommand = false;
      noToneDIY(BUZZER_PIN);
      playingTone = false;
    }
    
  }
}

void loop() {
  while(1){
    if(playCommand)
    {
      //continually run the update funcitons
      t.update();
      UpdateTone();
      UpdateRange();
      UpdateTempo();
    }
    //Comms should always be running even if the other code is not
    UpdateComms();
  }
}
