#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


#define CE_PIN   4
#define CSN_PIN 5

const byte slaveAddress[5] = {'R','x','A','A','A'};

const int joypinX = 27;
const int joypinY = 26;

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

//char dataToSend[10] = "Message 0";
int dataToSend = 0;
char txNum = '0';


unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 100; // send once per second


void setup() {

    Serial.begin(115200);

    Serial.println("SimpleTx Starting");

    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.setRetries(3,5); // delay, count
    radio.openWritingPipe(slaveAddress);
}

//====================

void loop() {
    currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {
        send();
        prevMillis = millis();
    }
}

//====================

void send() {

    bool rslt;
    rslt = radio.write( &dataToSend, sizeof(dataToSend) );
        // Always use sizeof() as it gives the size as the number of bytes.
        // For example if dataToSend was an int sizeof() would correctly return 2

    Serial.print("Data Sent ");
    //Serial.print(dataToSend);
    updateMessage();
    if (rslt) {
        Serial.println("  Acknowledge received");
    }
    else {
        Serial.println("  Tx failed");
    }
}

//================

void updateMessage() {
        // so you can see that new data is being sent
    
    /*
    txNum += 1;
    if (txNum > '9') {
        txNum = '0';
    }
    dataToSend[8] = txNum; */

    int x = analogRead(joypinX);
    int y = analogRead(joypinY);

    x = map(x, 0, 4095, 0, 98);
    y = map(y, 0, 4095, 0, 98);

    dataToSend = x * 100 + y;
    Serial.print("   data updated    ");
    Serial.print(dataToSend);
}