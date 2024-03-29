#include <OneWire.h>
#define pin 10

OneWire ibutton (pin); // I button connected on PIN 2.

byte addr[8]; //array to store the Ibutton ID.
byte newID0 [8] = {0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2F}; // - проверен (метаком, цифрал, визит).
byte newID1 [8] = {0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x9B}; // - проверен (кондор, цифрал, визит) - выходит в сервис!!!.
byte newID2 [8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x14}; // - проверен (метаком, цифрал).
byte newID3 [8] = {0x01, 0x53, 0xD4, 0xFE, 0x00, 0x00, 0x7E, 0x88}; // - проверен (метаком, цифрал).
byte newID4 [8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3D}; // - проверен (цифрал).
byte newID5 [8] = {0x01, 0x53, 0xD4, 0xFE, 0x00, 0x00, 0x00, 0x6F}; // - проверен (визит).
byte newID6 [8] = {0x01, 0xBE, 0x40, 0x11, 0x5A, 0x36, 0x00, 0xE1}; // - проверен (визит).
byte newID7 [8] = {0x01, 0x00, 0x36, 0x5A, 0x11, 0x40, 0xBE, 0xB1}; // - проверен (визит).
byte newID8 [8] = {0x01, 0x00, 0x00, 0x00, 0xFE, 0xD4, 0x53, 0x1E}; // - проверен (визит).
byte newID9 [8] = {0x01, 0xBE, 0x40, 0x11, 0x0A, 0x00, 0x00, 0x1D}; // - проверен работает Визит иногда КЕЙМАНЫ
//byte newID[8] = {0x01, 0x00, 0xBE, 0x11, 0xAA, 0x00, 0x00, 0xFB); //???-домофоны Кейман (KEYMAN)
//byte newID[8] = {0x01, 0x76, 0xB8, 0x2E, 0x0F, 0x00, 0x00, 0x5C); //???-домофоны Форвард
//byte newID[8] = {0x01, 0xBE, 0x40, 0x11, 0x5A, 0x56, 0x00, 0xBB}; //- проверен работает
//byte newID[8] = {0x01, 0xBE, 0x40, 0x11, 0x00, 0x00, 0x00, 0x77}; //- проверен работает
//byte newID[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x90, 0x19, 0xFF); //???-Отлично работает на старых домофонах
//byte newID[8] = {0x01, 0x6F, 0x2E, 0x88, 0x8A, 0x00, 0x00, 0x4D); //???-Открывать что-то должен
    
void setup(){
 Serial.begin(115200); 
}

void loop(){
  if (!ibutton.search (addr)){//read attached ibutton and asign value to buffer
    ibutton.reset_search();
    delay(200);
    return;
 }
 
  Serial.print(millis()/1000);
  Serial.print("> ");
  for (byte x = 0; x<8; x++) {  
    Serial.print(addr[x],HEX); //print the buffer content in LSB. For MSB: for (int x = 8; x>0; x--) 
     Serial.print(" ");
  }
 
  //compute crc//
  byte crc;
  crc = ibutton.crc8(addr, 7);
  Serial.print("CRC: ");
  Serial.println(crc,HEX);
  char input = Serial.read();
 
  switch(input) {
    case '0': writeKey(newID0); break;
    case '1': writeKey(newID1); break;
    case '2': writeKey(newID2); break;
    case '3': writeKey(newID3); break;
    case '4': writeKey(newID4); break;
    case '5': writeKey(newID5); break;
    case '6': writeKey(newID6); break;
    case '7': writeKey(newID7); break;
    case '8': writeKey(newID8); break;
    case '9': writeKey(newID9); break;
  }
}

void writeKey(byte newID[8]) {
    ibutton.skip();
    ibutton.reset();
    ibutton.write(0x33);
    Serial.print("  ID before write:");
    for (byte x=0; x<8; x++){
      Serial.print(' ');
      Serial.print(ibutton.read(), HEX);
    }
    // send reset
    ibutton.skip();
    ibutton.reset();
    // send 0xD1
    ibutton.write(0xD1);
    // send logical 0
    digitalWrite(pin, LOW); pinMode(pin, OUTPUT); delayMicroseconds(60);
    pinMode(pin, INPUT); digitalWrite(pin, HIGH); delay(10);
    
    Serial.print('\n');
    Serial.print("  Writing iButton ID:    ");
    writeV1(newID);                  
} 

void writeV1(byte newID[8]) {
    ibutton.skip();
    ibutton.reset();
    ibutton.write(0xD5);
    for (byte x = 0; x<8; x++){
      writeByte(newID[x]);
      Serial.print(newID[x], HEX);
      Serial.print(" ");
    }
    Serial.print('\n');
    ibutton.reset();
    // send 0xD1
    ibutton.write(0xD1);
    //send logical 1
    digitalWrite(10, LOW); pinMode(10, OUTPUT); delayMicroseconds(10);
    pinMode(10, INPUT); digitalWrite(10, HIGH); delay(10);
}

void writeV2(byte key_to_write[8]) {
  for (uint8_t i = 0; i < 8; i++)
  {
    byte data[4];
    ibutton.reset();
    data[0] = 0x3C; // "WRITE" command
    data[1] = i; // programming i-th byte
    data[2] = 0;
    data[3] = key_to_write[i];
    ibutton.write_bytes(data, 4);
    Serial.print(".");
  
    uint8_t crc = ibutton.read();
    
    if (OneWire::crc8(data, 4) != crc) {
        Serial.print("error!\r\n");
        return;
    }
    else
      Serial.print(".");
    
    //send_programming_impulse();
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH); 
    delay(60);
    digitalWrite(pin, LOW); 
    delay(5);
    digitalWrite(pin, HIGH); 
    delay(50); 
  }
  
  Serial.println("done!");
}

int writeByte(byte data){
  int data_bit;
  for(data_bit=0; data_bit<8; data_bit++){
    if (data & 1){
      digitalWrite(pin, LOW); pinMode(pin, OUTPUT);
      delayMicroseconds(60);
      pinMode(pin, INPUT); digitalWrite(pin, HIGH);
      delay(10);
    } else {
      digitalWrite(pin, LOW); pinMode(pin, OUTPUT);
      pinMode(pin, INPUT); digitalWrite(pin, HIGH);
      delay(10);
    }
    data = data >> 1;
  }
  return 0;
}
