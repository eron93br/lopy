

// Include libraries ---
#include <SPI.h>             
#include <LoRa.h>
#include <SD.h>
#include "TimerOne.h"
#include "LowPower.h"
// ---------------------

// ATMEGA 328P
const int csPin = 7;          // LoRa radio chip select
const int resetPin = 6;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

// VARIABLES
String outgoing;              // outgoing message
byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0x55;     // address of this device
byte destination = 0xFF;      // destination to send to, 0xFF is a broadcast!!!!
long lastSendTime = 0;        // last send time

File myFile;

void setup()
{
  Serial.begin(9600);
  //Timer1.initialize(1000000);          // Manda mensagem a cada 05 segundos
  //Timer1.pwm(9, 512);               // setup pwm on pin 9, 50% duty cycle
  //Timer1.attachInterrupt(callback);   // attaches callback() as a timer overflow interrupt
  pinMode(13,OUTPUT);

  pinMode(10, OUTPUT);
  if (!SD.begin(10)) 
  {
    Serial.println("initialization failed!");
    return;
  }
  //Serial.println("LoRa Duplex Endpoint");
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin
  if (!LoRa.begin(915E6)) 
  {             // initialize ratio at 915 MHz
    //Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  //Serial.println("LoRa init succeeded.");
  pinMode(8, OUTPUT);
  
  delay(5000); 
}

bool flag = false;

// Função interrupcao para mandar informacoes via LoRa
void callback()
{
    // String message = String( a + b + a);
    uint8_t message[5] = {'E', 's', 't', 'o','u'};
    sendMessage(message);
}
 
void loop()
{
   onReceive(LoRa.parsePacket());
} 

// ---------------------Message format ---------------------- //
// |                                                          //
// | destADDRESS - localADDRESS - msgID - #payload - "MSG"    //
// |                                                          //
// ---------------------------------------------------------- //

void sendMessage(uint8_t* outgoing) 
{
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(4);                        // add payload length
  LoRa.write(outgoing,4);               // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

void onReceive(int packetSize) 
{
  if (packetSize == 0)
  { 
    digitalWrite(13, LOW);
    return;          // if there's no packet, return
  }

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available()) 
  {
    incoming += (char)LoRa.read();
    digitalWrite(8, HIGH);
  }

  if (incomingLength != incoming.length())
  {   // check length for error
    //Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }
  // if message is for this device, or broadcast, print details:

   myFile = SD.open("test.txt", FILE_WRITE);

  if (myFile) 
  {
         // CODIGO PARA LISTAR EM UMA LINHA OS RECEBIDOS
        myFile.print("Recebido de  0x" + String(sender, HEX));
        myFile.print(" | ");
        myFile.print("Message ID: " + String(incomingMsgId)); 
        myFile.print(" | ");
        myFile.print("RSSI: " + String(LoRa.packetRssi()));
        // close the file:
        myFile.println(" ");
        myFile.close();          
  }
  else
  {
    Serial.print(F("INVALID"));
  }
  
  /*
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
  */
}
