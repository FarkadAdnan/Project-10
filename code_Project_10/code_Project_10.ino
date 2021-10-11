/*
 By:Farkad Adnan
 E-mail: farkad.hpfa95@gmail.com
 inst : farkadadnan
 #farkadadnan , #farkad_adnan , فرقد عدنان#
 FaceBook: كتاب عالم الاردوينو
 inst : arduinobook
 #كتاب_عالم_الاردوينو  #كتاب_عالم_الآردوينو 
 */
#include <Servo.h>
const int MaxSensors = 2;                      
const int ServoPins[MaxSensors] = {10, 11};   
const int RangingPins[MaxSensors] = {3, 2};   
const int ReadingsPerSensor = 5;              
const int TimePerDegree = 9;         
const int MinimumTurnDistance = 3;     

Servo ServoList[MaxSensors];                       
int sensorReadings[MaxSensors][ReadingsPerSensor];  
int calculatedSenorReadings[MaxSensors];             
int latestReading = 0;                              
int servoLocations[MaxSensors];                    
int SenorClose = 500;                
int SensorFar = 14000;                             
int ServoClose[MaxSensors] = {0, 160};              
int ServoFar[MaxSensors] = {70,110};          

void setup() {
  for (int i = 0; i < MaxSensors; i++){
     ServoList[i].attach(ServoPins[i]);
     delay(10);
     ServoList[i].write(ServoClose[i]);
     delay(500);
     ServoList[i].write(ServoFar[i]);
     delay(500);
     ServoList[i].detach();
   }
   delay(100);
}

void loop(){
  int i, j, oldLocation;
  unsigned long delayTime;
  for (i = 0; i < MaxSensors; i++){
    sensorReadings[i][latestReading] = getDistance(i);
    calculatedSenorReadings[i] = calculateNewDistace(i);

    oldLocation = servoLocations[i];
    servoLocations[i] = map(calculatedSenorReadings[i], 0, 100, ServoClose[i], ServoFar[i]);
    if (latestReading >= ReadingsPerSensor-1){    
      if (abs(servoLocations[i]-oldLocation) >= MinimumTurnDistance){   
        ServoList[i].attach(ServoPins[i]);
      delay(10);
      ServoList[i].write(servoLocations[i]);
      delayTime = (TimePerDegree * (abs(servoLocations[i]-oldLocation))+20); 
      if (abs(delayTime)>500){
        delayTime=500;       
        }
      delay(delayTime);
      ServoList[i].detach();
    } else {                        
      ServoList[i].attach(ServoPins[i]); 
      delay(10);
      ServoList[i].write(oldLocation);
      delay(50);         
      ServoList[i].detach();   
        servoLocations[i]=oldLocation;
    }
    }
    delay(20);
    }

  latestReading++; 
  if (latestReading >= ReadingsPerSensor){ 
    latestReading = ReadingsPerSensor-1;
    for (i = 0; i < MaxSensors; i++){
      for (j=0; j < ReadingsPerSensor-1; j++){
        sensorReadings[i][j] = sensorReadings[i][j+1];
      }
    }
  }
}
int calculateNewDistace(int sensorNumber){
  int output = SensorFar;                    
  float weightingFactor = 0.5;              
  float flickerFactor = 30;              
  if (latestReading >= ReadingsPerSensor-1) {  
    int total = 0;                   
    float currentWeight = 1; 
    float percentagePossible = 0;
    boolean flickered = false;
    for (int i=ReadingsPerSensor-1; i >=0 ;i--){  
      flickered = false;
      if (i==ReadingsPerSensor-1){
        if ((abs(sensorReadings[sensorNumber][i])-abs(sensorReadings[sensorNumber][i-1]) > flickerFactor) &&
           (abs(sensorReadings[sensorNumber][i-1])-abs(sensorReadings[sensorNumber][i-2]) > flickerFactor)){
          flickered = true;
        }
      }
      if (flickered==false){
        total += (sensorReadings[sensorNumber][i] * currentWeight);
        percentagePossible += currentWeight;
        currentWeight *= weightingFactor;
      } }
    output = total / percentagePossible;
  }
  return output;
}
int getDistance(int sensorNumber){
  long duration;  
  int out;      
  pinMode(RangingPins[sensorNumber], OUTPUT);
  digitalWrite(RangingPins[sensorNumber], LOW);
  delayMicroseconds(2);
  digitalWrite(RangingPins[sensorNumber], HIGH);
  delayMicroseconds(5);
  digitalWrite(RangingPins[sensorNumber], LOW);

 pinMode(RangingPins[sensorNumber], INPUT);
  duration = pulseIn(RangingPins[sensorNumber], HIGH);

   duration = constrain(duration, SenorClose, SensorFar);
  out = map(duration,  SenorClose, SensorFar, 0, 100);
  return out;
}
