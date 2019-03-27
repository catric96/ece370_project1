//pins for ir read and motor control
#define IR_OUTER 11
#define IR_INNER 12
#define BACK_PIN 5
#define FORW_PIN 9

//class variables
int t1;
int t2;
float angleRequested = 0.0;
float currentAngle = 0.0;
float angleChange = 0.0;
float errorCount = 0.0;
int ticks;
int ticksRequested;
int outerIR;
int innerIR;
float kp = 4;
char byteArray[6];

void setup(){
  Serial.begin(9600); //output to main computer serial 
  Serial1.begin(9600); //pi UART communication serial input
  pinMode(FORW_PIN, OUTPUT); //servo pins for motor
  pinMode(BACK_PIN, OUTPUT);
  pinMode(IR_OUTER, INPUT_PULLUP);//outer and inner IR sensors setup
  pinMode(IR_INNER, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IR_OUTER), tickInterrupt, CHANGE);//attach interrupt to outer IR sensor (main sensor)
  currentAngle = 0; //reset so whatever angle is current is 0
  t1 = millis(); //start timer
}

void loop(){
  //take from UART serial six bytes:
  //(0xFF) (0xFF) (direction) (magnitude/256) (magnitude%256) (checkSum)
  while(true){
    if(Serial1.available() >= 6){
      for ( char i = 0; i < 7; i++ ){
        byteArray[i] = Serial1.read(); //read six bytes from serial
      }
      
      if(byteArray[0] == char(0xFF) && byteArray[1] == char(0xFF)){ //if first two bytes read are FF, proceed
        char checkSum = (byteArray[2] + byteArray[3] + byteArray[4])%256; //calculate checkSum
        if(checkSum == byteArray[5]){ //check checkSum
          angleRequested = (byteArray[3] * 256) + byteArray[4]; //calculate angle
          if(1 == byteArray[2])
            angleRequested *= -1;
          Serial.println(angleRequested);
        }
        else{
          Serial.write("errorCount in checkSum.");
        }
      }
      else{
        Serial.write("errorCount in header.");
      }  
    }
    //check if angle is out of bounds
    if(angleRequested > 720)
      angleRequested = 720;
    else if(angleRequested < -720)
      angleRequested = -720;
    //convert ticks to angle
    currentAngle = getAngle(ticks);
    //update the timer and run motors
    updateTimer();
  }
}
//set the velocity of the motors based on forward or backward movement request
void setVelocity(float velocity) {
  if (velocity >= 127) {
    analogWrite(BACK_PIN, 0);
    analogWrite(FORW_PIN, (velocity - 127)*2);
  }
  if (velocity < 127) {
    analogWrite(FORW_PIN, 0);
    analogWrite(BACK_PIN, (127 - velocity)*2);
  }
}
//if the outer sensor and inner sensor are not equal, increment ticks
void tickInterrupt(){
  //when the ir sensors are equal, it means a backwards mark is being hit
  //this is proven by the quadrature encoder model
  innerIR = digitalRead(IR_INNER);
  outerIR = digitalRead(IR_OUTER);
  if(outerIR != innerIR)
    ticks++;
  else
    ticks--; 
}
//update the timer
void updateTimer(){
  t1 = millis();
  if(t1 - t2 >= 25){//if difference greater than 25ms, set velocity
    //this removes potential error (like debouncing)
    setVelocity(pController(angleRequested, currentAngle) + 127);
    t2 = t1;
  }
}
//get the angle, conversion is 1.2 degrees per tick
float getAngle(int count){
  return count * (90 / 75.7); //1.2 degrees ~ 90/75
}
//pController code, kp factor multiplied by required angle - current angle
float pController(float angleReq, float angleCur){
  return kp * (angleReq - angleCur);
}
