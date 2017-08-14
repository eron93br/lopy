/*
   CODIGO EXCLUSIVO PARA O ATMEGA 32u4
*/

#include <SPI.h>              
#include <LoRa.h>

#define LIMIAR 35
#define localAddress 0xBB
#define destination 0xFF

// ATMEGA 32U4 - ADAFRUIT FEATHER BOARD -------------------------
const int csPin = 8;          // LoRa radio chip select
const int resetPin = 4;       // LoRa radio reset
const int irqPin = 7;         // change for your board; must be a hardware interrupt pin */

// ------------- CONTADORES DO DEBUG ------------------------
int cont_ab = 0;
int cont_bb = 0;
int cont_57 = 0;
int cont_55 = 0;
int cont_22 = 0;

String outgoing;              // outgoing message

byte msgCount = 0;            // count of outgoing messages
//byte localAddress = 0x1B;     // address of this device
//byte destination = 0xFF;      // destination to send to
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

uint8_t count_vetor[8]; 

void setup() 
{
  Serial.begin(9600);                   // initialize serial
  //while (!Serial);

  Serial.println("LoRa Duplex");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(915E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
  pinMode(8, OUTPUT);
}

void loop() 
{ 
  digitalWrite(8, LOW);
  
  if (millis() - lastSendTime > interval) 
  {
    String message = "HeLoRa World!";   // send a message
    sendMessage(message);
    Serial.println("Sending " + message);
    lastSendTime = millis();            // timestamp the message
    interval = random(20000) + 1000;    // 2-3 seconds
  }
  
  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
}

void sendMessage(String outgoing) 
            {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

// aceitar apenas os pacotes menores que o LIMIAR!
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
  // a partir daqui ja recebeu o pacote LoRa! <<<------------

 
  
  /*if( abs(LoRa.packetRssi()) >  LIMIAR )
  {
    Serial.println(" pacote nao aceito! ");
    return;
  }*/

  if (incomingLength != incoming.length())
  {   // check length for error
    Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }

  // CODIGO PARA LISTAR EM UMA LINHA OS RECEBIDOS
  /*
  Serial.print("Recebido de  0x" + String(sender, HEX));
  Serial.print(" | ");
  Serial.print("Message ID: " + String(incomingMsgId)); 
  Serial.print(" | ");
  Serial.print("RSSI: " + String(LoRa.packetRssi()));
  */

  // CODIGO PARA LISTAR EM LISTA E CONTADOR OS RECEBIDOS!!!
  Serial.print(" Contador 0xAB  " + String(cont_ab) + " | ");
  Serial.print(" Contador 0xBB  " + String(cont_bb) + " | ");
  Serial.print(" Contador 0x57  " + String(cont_57) + " | ");
  Serial.print(" Contador 0x55  " + String(cont_55) + " | ");
  Serial.print(" Contador 0x22  " + String(cont_22) + " | ");
  // if message is for this device, or broadcast, print details:

  switch (sender) 
  {
    case 0xAB:
      cont_ab = cont_ab + 1;
      break;
    case 0xBB:
      cont_bb = cont_bb + 1;
      break;
    case 0x57:
      cont_57 = cont_57 + 1;
      break;
    case 0x55:
      cont_55 = cont_55 + 1;
      break;    
    case 0x22:
      cont_22 = cont_22 + 1;
      break;
  }
  
  /*
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  */
  Serial.println();
}
