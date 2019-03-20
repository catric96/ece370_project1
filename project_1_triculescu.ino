//this code takes an angle input from positive to negative 720 and rotates the dc motor
//connected by the h-bridge to the desired angle. new angle inputs and adjustments can be sent continuously.
//this is accomplished by reading how many ticks the encoder wheel has completed, translating them into rotations of the drive shaft. 

int inputPin = 12; 
int fwdPin = 10; 
int bwdPin = 9;
int isObstacle = LOW; //low means obstacle present
int countStates = 0;
int countTicks = 0;
int totalTicks;
int inputAngle;
float totalRotations;

void setup(){ //setup pins and serial
  pinMode(fwdPin, OUTPUT);
  pinMode(bwdPin, OUTPUT);
  pinMode(inputPin, INPUT);
  Serial.begin(9600);
}
void loop(){
  if(Serial.available() > 0){//if serial is available, parse angle input
    inputAngle = Serial.parseInt();
    if((inputAngle > 720)||(inputAngle < -720)){
      Serial.println("Angle out of bounds, -721<angle<721");
    }
    else{
      totalRotations = ((float)inputAngle)/360.0; //calculate how many total rotations there will be done
      //720/360=2 would mean a maximum of two full rotations
      numberOfTicks = (int)(totalRotations*75); //multiply the number of rotations by 75, the number of ticks for one full rotation
      //75*2 = maximum of 150 full ticks of encoder wheel, representing two full rotations of drive shaft
    }
  }
  //stop motors and set ticks and number of ticks back to zero when limit is reached
  //both are zero by default so this will keep them zero until a numberofTicks value is set
  if(countTicks == abs(numberOfTicks)){
    analogWrite(fwdPin, 127);//stop motors
    analogWrite(bwdPin, 127);
    countTicks = 0;//reset variables
    numberOfTicks = 0;
  }
  //otherwise, either move the motors backwards or forwards
  else if(numberOfTicks != 0){
    //move motors forward and stop backwards mosfets if numberOfTicks >0
    if(numberOfTicks > 0){
      analogWrite(bwdPin, 127);
      analogWrite(fwdPin, 250);
    }
    //otherwise, stop fwd mosfets and move backwards
    else if(numberOfTicks < 0){
      analogWrite(fwdPin, 127);
      analogWrite(bwdPin, 250);
    }
  }
  //read the sensor input
  isObstacle = digitalRead(inputPin);
  if(numberOfTicks !=0){//if an angle input is actually set, then increment states if object detected (white part of wheel)
    if (isObstacle){
      //do nothing if object is not detected
    }
    else{
      countStates++;
    }
  }
  //after white part of wheel has been detected twice, a rotation of the small shaft has completed, increment ticks
  if(countStates == 2){
    countTicks++;//increment ticks, as one tick has finished (full rotation of encoder wheel)
    countStates = 0;
  }
  delay(5);// small delay as to not flood serial
}
