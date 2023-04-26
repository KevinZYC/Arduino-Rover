#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10
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

int deadZone = 50;

int leftPWM;
int rightPWM;

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


    Serial.println("SimpleRx Starting");
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.openReadingPipe(1, thisSlaveAddress);
    radio.startListening();
}

//=============

void loop() {
    getData();
    showData();

    runMotors();

}

//==============

void getData() {
    if ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
        newData = true;

        int x = dataReceived / 100 - 46;
        int y = dataReceived % 100 - 45;

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

        if(leftPWM < deadZone){
          leftPWM = 0;
        } else {
          leftPWM = map(leftPWM, 0, 200, 100, 200);
        }

        if(rightPWM < deadZone){
          rightPWM = 0;
        } else {
          rightPWM = map(rightPWM, 0, 200, 100, 200);
        }
        
        Serial.print("test ");
        Serial.print(leftPWM);
        Serial.print(" ");
        Serial.print(rightPWM);
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
        //Serial.print(dataReceived[0]);
        Serial.println(dataReceived);
        newData = false;
    }
}