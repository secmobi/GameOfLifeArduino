#include <Adafruit_GFX.h>
#include <RGBmatrixPanel.h>


// If you don't use a LM35 sensor for ramdom seed, comment this line
#define USE_LM35_FOR_SEED

// If you don't use a button to reset, comment this line
#define USE_BUTTON_TO_RESET

// Set the default density of lit LEDs at initial status. Default is 0.3
#define DENSITY 0.3

// Set the max iterate steps number. Default is 600
#define MAX_STEPS 600

// Set the microseconds waited during iterations. Default is 500
#define WAIT_MICROSECOND 500


#define CLK 8
#define LAT A3
#define OE  9
#define A   A0
#define B   A1
#define C   A2

RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);


#ifdef USE_LM35_FOR_SEED
  #define PIN_LM35 5
#endif

#ifdef USE_BUTTON_TO_RESET
  #define PIN_BUTTON 12
  int buttonState = HIGH;
#endif

// use a 32x16 LED matrix
#define NI 32
#define NJ 16

// use 1 byte for 4 LEDs to save SRAM space
#define MI ((NI + 2) / 2)
#define MJ ((NJ + 2) / 2)

byte old[MI * MJ];
byte current[MI * MJ];

void setOld(int i, int j, byte value)
{
  byte pos = i/2 * MJ + j/2;
  byte off = ((i % 2) * 2 + (j % 2)) * 2;
  old[pos] = (old[pos] & (~(0x01 << off))) | ((value & 0x01) << off); 
}

byte getOld(int i, int j)
{
  byte pos = i/2 * MJ + j/2;
  byte off = ((i % 2) * 2 + (j % 2)) * 2;
  return (old[pos] & (0x01 << off)) >> off;
}

void setCurrent(int i, int j, byte value)
{
  byte pos = i/2 * MJ + j/2;
  byte off = ((i % 2) * 2 + (j % 2)) * 2;
  current[pos] = (current[pos] & (~(0x01 << off))) | ((value & 0x01) << off); 
}

byte getCurrent(int i, int j)
{
  byte pos = i/2 * MJ + j/2;
  byte off = ((i % 2) * 2 + (j % 2)) * 2;
  return (current[pos] & (0x01 << off)) >> off;
}

void initRandom(float rate = DENSITY)
{
  int i, j, n;
  
  for(i = 1; i <= NI; i++)
    for(j = 1; j <= NJ; j++) {
      n = random(100);
      if (n < rate * 100)
        setCurrent(i, j, 1);
      else
        setCurrent(i, j, 0);
    }
}

void iterate()
{
  int i, j;
  int im, ip, jm, jp;
  int nsum;
  
  for(i = 1; i <= NI; i++) {
    for(j = 1; j <= NJ; j++) {
      setOld(i, j, getCurrent(i, j));
    }
  }
  
  setOld(0, 0, getOld(NI, NJ));
  setOld(0, NJ+1, getOld(NI, 1));
  setOld(NI+1, NJ+1, getOld(1, 1));
  setOld(NI+1, 0, getOld(1, NJ));
  
  for(i = 1; i <= NI; i++) {
    setOld(i, 0, getOld(i, NJ));
    setOld(i, NJ+1, getOld(i, 1));
  }

  for(j = 1; j <= NJ; j++) {
    setOld(0, j, getOld(NI, j));
    setOld(NI+1, j, getOld(1, j));
  }
  
  for(i = 1; i <= NI; i++) {
    for(j = 1; j <= NJ; j++) {
      im = i-1;
      ip = i+1;
      jm = j-1;
      jp = j+1;
      
      nsum = getOld(im, jp) + getOld(i, jp) + getOld(ip, jp)
           + getOld(im, j)                  + getOld(ip, j)
           + getOld(im, jm) + getOld(i, jm) + getOld(ip, jm);

      switch(nsum) {
        case 3:
          setCurrent(i, j, 1);
          break;
        case 2:
          setCurrent(i, j, getOld(i, j));
          break;
        default:
          setCurrent(i, j, 0);
      }
    }
  }
}

void setup()
{
#ifdef USE_BUTTON_TO_RESET
  pinMode(PIN_BUTTON, INPUT);
#endif
  
#ifdef USE_LM35_FOR_SEED
  long seed = analogRead(PIN_LM35);
  // use a simple hash to make the seed more discrete
  seed = seed * seed % 2039;
  randomSeed(seed);
#endif
  
  initRandom();
  
  matrix.begin();
}

void loop()
{  
  int i, j;
  int n;
  byte old, current;
  int diff = NI*NJ;
  
  int button;
  
  for(n = 0; n < MAX_STEPS; n++) {
#ifdef USE_BUTTON_TO_RESET
    button = digitalRead(PIN_BUTTON);
    if (buttonState == HIGH && button == LOW)
      initRandom();
    buttonState = button;
#endif

    iterate();
    
    diff = 0;
    
    for(i = 0; i < NI; i++) {
      for(j = 0; j < NJ; j++) {
        old = getOld(i+1, j+1);
        current = getCurrent(i+1, j+1);
        
        if(old != current) diff++;
        
        if(old == 0 && current == 1) {  // new live
          matrix.drawPixel(i, j, matrix.Color333(0, 0, 3));
        } else if(old == 1 && current == 1) {  // still live
          matrix.drawPixel(i, j, matrix.Color333(0, 3, 0));
        } else { // dead
          matrix.drawPixel(i, j, matrix.Color333(0, 0, 0));
        }
      }
    }
    delay(WAIT_MICROSECOND);
    
    if(diff == 0) { // achieve a stable status
      n = 0;
      initRandom();
    }
    
    // TODO: reset if achieve a 2 steps loop
  }
}
