/*
 * The Arduino Source for a distance sensing collar
 *
 * A project done during the Fashion Tech in Berlin 2016.04.30
 *
 * Two sonars connect to a vibration each to give a physical sense
 * of the distance in two directions. 
 *
 * The project is a Cooperation between Stefanie Muehlbacher, Rasmus Hedin and Sonja Rattay
 * stefanie.muehlbacher gmx.de
 * rasmus blockzero.se
 * sonja blockzero.se
 * respectively
 *
 * For more information and connected scheme www.blockzero.se/echotech
 *
 * This code is in the public domain.
 */


#define ACTIVATE_VIBRATORS 1  //< Use vibrators (as opposed to only printout) 
#define TIME_FACTOR 5         //< A multiplication factor to convert from distance input to milliseconds vibration
#define TIME_ON 20            //< A relative time the vibrator is on during a 'spur'
#define NBR_VIBRATORS 2       //< Number of vibrators used
#define NBR_SONARS 2          //< Number of sonars used


int timeElapsedSinceSwitch[NBR_VIBRATORS];    //< Time since we switched the vibrator on/off
bool currentStates[NBR_VIBRATORS];            //< Is the Vibrator on or off


int vibratorPins[NBR_VIBRATORS]   = { 4,  5}; //< The pins used for the vibrators    
int sonarTrigPins[NBR_SONARS]     = {11,  7}; //< The pins used to trigger the sonars
int sonarResponsePins[NBR_SONARS] = {12,  8}; //< THe pins used to read from the sonars

/*
  TODO:

  Make structs 


*/


/*
 * Ping a particular Sonar for it's distance value
 */
int calculateDistance(int sonarId){ 

  int duration, distance;
  if( sonarId >= NBR_SONARS ){
    //If the sonar doesn't exist reply a default "pretty far away"
    return 400;
  }
  
  //Set trigger to low
  digitalWrite(sonarTrigPins[sonarId], LOW); 
  delayMicroseconds(2);

  //Set trigger to high
  digitalWrite(sonarTrigPins[sonarId], HIGH); 
  delayMicroseconds(10);

  //Set trigger to low again
  digitalWrite(sonarTrigPins[sonarId], LOW);

  //Send a high pulse in 
  duration = pulseIn(sonarResponsePins[sonarId], HIGH); 
  distance= duration*0.034/2;
  
#if 0
  Serial.print("****************");
  Serial.println(distance);
#endif

  return distance;
} 

void setup() {
  int i;

  Serial.begin(9600);
  Serial.println("SetupStart");

  for(i = 0; i < NBR_SONARS; i++){
    pinMode(sonarTrigPins[i], OUTPUT); 
    pinMode(sonarResponsePins[i], INPUT); 
  }

  for(i = 0; i < NBR_VIBRATORS; i++){
    timeElapsedSinceSwitch[i] = 0;
    setVibratorState( i, false);
  }

  Serial.println("SetupDone");

}


int startTime = millis();
int lastTime = 0;

#define MIN_VIBRATION_TO_NOT_FRY (255*0.2)

void setVibratorState( int vibId, bool newState ){
  int newPinValue;

  if( vibId > NBR_VIBRATORS){
    Serial.println("WARNING: TRying to set a pin not in use!");
    return;
  }

  if( vibId > NBR_VIBRATORS){
    Serial.println("WARNING: Trying to set a pin not in use!");
    return;
  }

  if(newState){
    newPinValue = MIN_VIBRATION_TO_NOT_FRY;  }
  else {
    newPinValue = 255;
  }

  Serial.print("Set Pin ");
  Serial.print(vibratorPins[vibId]);
  Serial.print(" to ");
  Serial.println(newPinValue);


#if ACTIVATE_VIBRATORS
  if(newPinValue >= MIN_VIBRATION_TO_NOT_FRY){
    analogWrite(vibratorPins[vibId], newPinValue); 
  }  
#endif       

  currentStates[vibId] = newState;
}


void evaluateVibrator(int vibId){
  int adjustedTime  = timeElapsedSinceSwitch[vibId] / TIME_FACTOR;

  //Use the vibrator ID as ID 
  int distance = calculateDistance(vibId);

  Serial.print(distance);      
  Serial.print(":");      
  Serial.println(vibId);
 
  //Check current status
  if(currentStates[vibId]){
    if( adjustedTime > TIME_ON ){
      Serial.print("turn off");
      Serial.println(vibId);

      setVibratorState( vibId, false);
      timeElapsedSinceSwitch[vibId] = 0;          
    }
  }
  else {
    if( adjustedTime > distance ){
      Serial.print("turn on");    
      Serial.println(vibId);
            
      setVibratorState( vibId, true);
      timeElapsedSinceSwitch[vibId] = 0;          
    }    
  }  
}

void loop() {
  int deltaTime;
  int i;
  
  startTime = millis();

  deltaTime = startTime - lastTime;
  lastTime = startTime;

  for( i = 0; i <  NBR_VIBRATORS; i++ ){
    timeElapsedSinceSwitch[i] += deltaTime;
    evaluateVibrator( i );
  }
}

