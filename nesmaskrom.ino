#define LED PB12

/*
 * data pins should be 5V tolerant in case
 * we want to run on 5V 
 * 

              ---_---
 [+5V] A15 - |01   28| - +5V
       A12 - |02   27| - A14 [PGM]
       A7  - |03   26| - A13 [NC]
       A6  - |04   25| - A8
       A5  - |05   24| - A9
       A4  - |06   23| - A11
       A3  - |07   22| - A16
       A2  - |08   21| - A10
       A1  - |09   20| - /CE
       A0  - |10   19| - D7
       D0  - |11   18| - D6
       D1  - |12   17| - D5
       D2  - |13   16| - D4
       GND - |14   15| - D3
 * 
 */
#define NES_D0 PB13
#define NES_D1 PB14
#define NES_D2 PB15
#define NES_D3 PA8
#define NES_D4 PA9
#define NES_D5 PA10
#define NES_D6 PB11
#define NES_D7 PB10

#define NES_A00 PA15
#define NES_A01 PB3
#define NES_A02 PB4
#define NES_A03 PB5
#define NES_A04 PB6
#define NES_A05 PB7
#define NES_A06 PB1
#define NES_A07 PB0
#define NES_A08 PA7
#define NES_A09 PA6
#define NES_A10 PA5
#define NES_A11 PA4
#define NES_A12 PA3
#define NES_A13 PA2
#define NES_A14 PA1
#define NES_A15 PA0
#define NES_A16 PB9

const uint32_t nesAddress[17] = {
  NES_A00, NES_A01, NES_A02, NES_A03, NES_A04, NES_A05, NES_A06, NES_A07, 
  NES_A08, NES_A09, NES_A10, NES_A11, NES_A12, NES_A13, NES_A14, NES_A15, NES_A16
};

const uint32_t nesData[16] = {
  NES_D0, NES_D1, NES_D2, NES_D3, NES_D4, NES_D5, NES_D6, NES_D7
};

uint32_t address = 0;

uint8_t read(uint32_t address) {
  for (unsigned i = 0 ; i < 17 ; i++, address >>= 1) {
    digitalWrite(nesAddress[i], address & 1);
  }
  delayMicroseconds(4);
//  delay(1);
  uint8_t datum = 0;
  for (int i = 7 ; i >= 0 ; i--) {
    datum = (datum << 1) | digitalRead(nesData[i]);
  }

  return datum;
}

void setup() {
  for (unsigned i = 0 ; i < 17 ; i++)
    pinMode(nesAddress[i], OUTPUT);
  for (unsigned i = 0 ; i < 8 ; i++)
    pinMode(nesData[i], INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 1);
  delay(8000);
  digitalWrite(LED, 0);
}

void loop() {
  if (address >= 0x20000) return;
  uint16_t sum = 0;
  char s[6];
  sprintf(s, "%04x ", address);
  Serial.print(s);
  for (unsigned i = 0 ; i < 16 ; i++) {
    uint8_t datum = read(address);
    sum += datum;
    address++;
    sprintf(s, "%02x", datum);
    Serial.print(s);
  }
  sprintf(s, " %04x", sum);
  Serial.println(s);
}

