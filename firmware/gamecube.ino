// Use with the Sparkfun Pro Micro 16MHz. Use Pin 3 for the GC data.
const uint8_t gamecube_pin = PD0;
const uint8_t gamecube_prefix = 25;
const uint8_t gamecube_bitcount = 64;
unsigned char data[gamecube_bitcount + gamecube_prefix];

#define PIN_READ(pin)  (PIND & (1 << pin))
#define TWOMICROSECOND_NOPS "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
#define WAIT_FALLING_EDGE(pin) while(!PIN_READ(pin)); while(PIN_READ(pin));

void setup()
{
  // Activate gamecube pin as an input
  DDRD &= ~(1 << gamecube_pin);
    
  Serial.begin(115200);
}

void loop()
{
  uint8_t bits_left = gamecube_prefix + gamecube_bitcount;
  unsigned char* ptr = data;

  noInterrupts();

  // Collect data
  while (true) {

    // Wait for the data line to go low
    WAIT_FALLING_EDGE(gamecube_pin);

    // Wait 2us between line reads
    asm volatile(TWOMICROSECOND_NOPS);

    // Read a bit and store in the header
    *ptr = PIN_READ(gamecube_pin);
    ++ptr;

    if (--bits_left == 0) {
      break;
    }
  }

  interrupts();

  Serial.write(data + gamecube_prefix, gamecube_bitcount);
  Serial.write('\n');
}
