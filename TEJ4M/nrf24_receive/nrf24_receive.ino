#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <MPU6050.h>
#include <Servo.h>

#define CE_PIN 7
#define CSN_PIN 8
#define LEFT_EN 5
#define RIGHT_EN 6
#define LEFT_1 A0
#define LEFT_2 A1
#define RIGHT_1 A2
#define RIGHT_2 A3

const byte thisSlaveAddress[5] = {'R','x','A','A','A'};

RF24 radio(CE_PIN, CSN_PIN);

int dataReceived; // this must match dataToSend in the TX
bool newData = false;

bool leftReverse = true;
bool rightReverse = false;

int deadZone = 6;
int deadZonePWM = 50;

int gyroDeadZone = 5;

int pitchOffset = 11;
int rollOffset = 1;

int leftPWM;
int rightPWM;

MPU6050 mpu;
Servo pitchServo;
Servo rollServo;

//===========

void setup() {

    Serial.begin(9600);

    //set pin modes
    pinMode(LEFT_EN, OUTPUT);
    pinMode(RIGHT_EN, OUTPUT);
    pinMode(LEFT_1, OUTPUT);
    pinMode(LEFT_2, OUTPUT);
    pinMode(RIGHT_1, OUTPUT);
    pinMode(RIGHT_2, OUTPUT);

    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);

    pitchServo.attach(9);
    rollServo.attach(10);


    Serial.println("SimpleRx Starting");
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.openReadingPipe(1, thisSlaveAddress);
    radio.startListening();

    Serial.println("Initialize MPU6050");

    
    while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
    {
      Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
      delay(500);
    }
}

//=============

void loop() {
    getData();
    showData();

    runMotors();
    getMPU();

    //Serial.println("running");
    delay(80);

}

//==============
void getMPU(){
  Vector normAccel = mpu.readNormalizeAccel();

  int pitch = -(atan2(normAccel.XAxis, sqrt(normAccel.YAxis*normAccel.YAxis + normAccel.ZAxis*normAccel.ZAxis))*180.0)/M_PI;
  int roll = (atan2(normAccel.YAxis, normAccel.ZAxis)*180.0)/M_PI;

  if(abs(pitch - pitchOffset) > gyroDeadZone){
    pitchServo.write(180 - (pitch - pitchOffset + 90));
  } else {
    pitchServo.write(90);
  }
  
  if(abs(roll - rollOffset) > gyroDeadZone){
    rollServo.write(roll - rollOffset + 90);
  } else {
    rollServo.write(90);
  }
  
  /*
  Serial.print(pitchServo.read());
  Serial.print("  ");
  Serial.print(rollServo.read());
  
  Serial.print(" Pitch = ");
  Serial.print(pitch);
  Serial.print(" Roll = ");
  Serial.print(roll);
  
  
  Serial.println();*/
}

void getData() {
    if ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
        newData = true;

        int x = dataReceived / 100 - 46;
        int y = dataReceived % 100 - 45;

        if(abs(x) < deadZone){
          x = 0;
        }

        if(abs(y) < deadZone){
          y = 0;
        }

        x *= -1;

        if(x < -40){
          x = -40;
        }
        if(x > 40){
          x = 40;
        }

        leftReverse = true;
        rightReverse = false;
        if((y - x) * 2 <0){
          leftReverse = false;
        }

        if((y + x) * 2 <0){
          rightReverse = true;
        }
        
        leftPWM = abs((y - x) * 2);
        rightPWM = abs((y + x) * 2);

        
        if(leftPWM < deadZonePWM){
          leftPWM = 0;
        } else {
          leftPWM = map(leftPWM, 0, 200, 125, 200);
        }

        if(rightPWM < deadZonePWM){
          rightPWM = 0;
        } else {
          rightPWM = map(rightPWM, 0, 200, 125, 200);
        }

        //leftPWM = map(leftPWM, 25, 200, 125, 200);
        //rightPWM = map(rightPWM, 25, 200, 125, 200);
        
        /*
        Serial.print("test ");
        Serial.print(leftPWM);
        Serial.print(" ");
        Serial.print(rightPWM);
        */
    } else {
      //leftPWM = 0;
      //rightPWM = 0;
    }
}

void runMotors(){
  analogWrite(LEFT_EN, leftPWM);
  analogWrite(RIGHT_EN, rightPWM);
  if(leftReverse){
    digitalWrite(LEFT_1, HIGH);
    digitalWrite(LEFT_2, LOW);
  } else {
    digitalWrite(LEFT_1, LOW);
    digitalWrite(LEFT_2, HIGH);
  }

  if(rightReverse){
    digitalWrite(RIGHT_1, LOW);
    digitalWrite(RIGHT_2, HIGH);
  } else {
    digitalWrite(RIGHT_1, HIGH);
    digitalWrite(RIGHT_2, LOW);
  }
}

void showData() {
    if (newData == true) {
        Serial.print("Data received ");
        Serial.println(dataReceived);
        newData = false;
    }
}