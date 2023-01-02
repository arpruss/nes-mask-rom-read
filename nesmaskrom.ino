#define LED PC13

/*
 * data pins should be 5V tolerant in case
 * we want to run on 5V 
 * 

pinout for 128kb 28-pin using an stm32f103c8t6 blue pill

micro  rom    ---_---   rom   micro
PA7    A15 - |01   28| - +5V
PA4    A12 - |02   27| - A14  PA6
PC15   A7  - |03   26| - A13  PA5
PC14   A6  - |04   25| - A8   PA0
PB12   A5  - |05   24| - A9   PA1
PB13   A4  - |06   23| - A11  PA3
PB14   A3  - |07   22| - A16  PB0
PB15   A2  - |08   21| - A10  PA2
PA8    A1  - |09   20| - /CE  GND
PA9    A0  - |10   19| - D7   PA10
PB9    D0  - |11   18| - D6   PA15
PB8    D1  - |12   17| - D5   PB3
PB7    D2  - |13   16| - D4   PB4
GND    GND - |14   15| - D3   PB6

#define NES_A08 PA0
#define NES_A09 PA1
#define NES_A10 PA2
#define NES_A11 PA3
#define NES_A12 PA4
#define NES_A13 PA5
#define NES_A14 PA6
#define NES_A15 PA7
#define NES_A16 PB0
#define NES_A17 PB10
#define NES_A18 PB11


 more pinouts here: https://www.nesdev.org/NES%20ROM%20Pinouts.txt
 * 
 */

// Make sure the data pins are 5v-tolerant. For blue pill, see:
// http://land-boards.com/blwiki/index.php?title=File:STM32F103C8T6-Blue-Pill-Pin-Layout.gif
#define NES_D0 PB9
#define NES_D1 PB8
#define NES_D2 PB7
#define NES_D3 PB6
#define NES_D4 PB4
#define NES_D5 PB3
#define NES_D6 PA15
#define NES_D7 PA10

#define NES_A00 PA9
#define NES_A01 PA8
#define NES_A02 PB15
#define NES_A03 PB14
#define NES_A04 PB13
#define NES_A05 PB12
#define NES_A06 PC14
#define NES_A07 PC15
#define NES_A08 PA0
#define NES_A09 PA1
#define NES_A10 PA2
#define NES_A11 PA3
#define NES_A12 PA4
#define NES_A13 PA5
#define NES_A14 PA6
#define NES_A15 PA7
#define NES_A16 PB0
#define NES_A17 PB10
#define NES_A18 PB11

#define MAX_ADDRESS_BITS 19

const uint32_t nesAddress[MAX_ADDRESS_BITS] = {
  NES_A00, NES_A01, NES_A02, NES_A03, NES_A04, NES_A05, NES_A06, NES_A07, 
  NES_A08, NES_A09, NES_A10, NES_A11, NES_A12, NES_A13, NES_A14, NES_A15, NES_A16, NES_A17, NES_A18
};

const uint32_t nesData[8] = {
  NES_D0, NES_D1, NES_D2, NES_D3, NES_D4, NES_D5, NES_D6, NES_D7
};

uint32_t address = 0;
unsigned addressBits = 17;
uint32_t romSize = 128*1024;
uint32_t crc = 0;

uint8_t read(uint32_t address) {
  for (unsigned i = 0 ; i < addressBits ; i++, address >>= 1) {
    digitalWrite(nesAddress[i], address & 1);
  }
  delayMicroseconds(4);
  uint8_t datum = 0;
  for (int i = 7 ; i >= 0 ; i--) {
    datum = (datum << 1) | digitalRead(nesData[i]);
  }

  return datum;
}

void getLine(char* p, unsigned length) {
  unsigned pos = 0;

  while (pos < length - 1) {
    int c;
    while (-1 == (c = Serial.read())) ;
    if (c == '\n') {
      p[pos] = 0;
      return;
    }
    else if (c == 8) {
      if (pos > 0) 
        pos--;
    }
    else if (c != '\r') {
      p[pos++] = c;
    }
  }
  p[pos] = 0;
}

// crc32 adapted from https://github.com/aeldidi/crc32/blob/master/src/crc32.c
uint32_t
crc32_for_byte(uint32_t byte)
{
  const uint32_t polynomial = 0xEDB88320L;
  uint32_t       result     = byte;
  size_t         i          = 0;

  for (; i < 8; i++) {
    /* IMPLEMENTATION: the code below always shifts result right by
     * 1, but only XORs it by the polynomial if we're on the lowest
     * bit.
     *
     * This is because 1 in binary is 00000001, so ANDing the
     * result by 1 will always give 0 unless the lowest bit is set.
     * And since XOR by zero does nothing, the other half only
     * occurs when we're on the lowest bit.
     *
     * I didn't leave the above implementation in, despite being
     * faster on my machine since it is a more complex operation
     * which may be slower on less sophisticated processors. It can
     * be added in in place of the loop code below.
     */

    result = (result >> 1) ^ (result & 1) * polynomial;

    /* Here is the code I replaced with the branch I tried to
     * remove:
    if (result & 1) {
      result = (result >> 1) ^ polynomial;
      continue;
    }
    result >>= 1;
     */
  }
  return result;
}

uint32_t crc32(const void *input, size_t size, uint32_t start)
{
  const uint8_t *current = (const uint8_t*)input;
  uint32_t       result  = ~start;
  size_t         i       = 0;

  for (; i < size; i++) {
    result ^= current[i];
    result = crc32_for_byte(result);
  }

  return ~result;
}

void getCommand() {
  char input[128];
  Serial.println("Mask ROM dumper ready.");
  do {
    Serial.println("How large is your ROM (kb, max 512)?");
    getLine(input, 128);
    Serial.println(input);
    romSize = (uint32_t)atol(input);
    if (romSize == 0 || romSize > 512) {
      Serial.println("Invalid ROM size.");
      continue;
    }
    romSize *= 1024;
    for (unsigned i = 0 ; i < MAX_ADDRESS_BITS ; i++)
      if (romSize <= (1ul << i)) {
        address = 0;
        addressBits = i;
        crc = 0;
        Serial.print("Addresses will be ");
        Serial.print(addressBits);
        Serial.println(" bits long.");
        return;
      }    
  } while(1);
    
}

void setup() {
  for (unsigned i = 0 ; i < addressBits ; i++) {
    pinMode(nesAddress[i], OUTPUT);
    digitalWrite(nesAddress[i], 0);
  }
  for (unsigned i = 0 ; i < 8 ; i++) {
    pinMode(nesData[i], INPUT);
  }

  while(!Serial);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 1);
  delay(4000);
  digitalWrite(LED, 0);
  getCommand();
}

void loop() {
  char s[9];
  uint8_t data16[16];
  if (address >= romSize) {
    Serial.print("Dumped: ");
    Serial.print(romSize);
    Serial.print(" bytes\nCRC-32: ");
    sprintf(s,"%08lx", crc);
    Serial.println(s);
    getCommand();
  }
  uint16_t sum = 0;
  sprintf(s, ">%05lx ", address);
  Serial.print(s);
  for (unsigned i = 0 ; i < 16 ; i++) {
    uint8_t datum = read(address);
    data16[i] = datum;
    sum += datum;
    address++;
    sprintf(s, "%02x", datum);
    Serial.print(s);
  }
  crc = crc32(data16, 16, crc);
  sprintf(s, " %04x", sum);
  Serial.println(s);
}

