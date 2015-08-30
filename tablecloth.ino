#include <math.h>

#define MODE_SOLID 0
#define MODE_BLINK 1
#define MODE_STROBE 2
#define MODE_SCROLL 3
#define MODE_WAVE 4
#define MODE_GLOW 5
byte mode = MODE_SOLID;

#define F_MIN 0.5   // min freq in Hz
#define F_MAX 20.0  // max freq in Hz
#define WAVE_T 20   // sampling period for wave pattern in ms

int LEDI = 3;
int LEDE1 = 5;
int LEDE2 = 6;
int LEDE3 = 9;
int brightPot = 10;
int speedPot = 11;
int readbrightPot = 0;
int readspeedPot = 0;
int writebrightPot = 0;
int writespeedPot = 0;

unsigned int blink_period;
unsigned int strobe_period;
unsigned int scroll_period;
unsigned int wave_freq;
unsigned int n = 0;


void setup() {
  Serial.begin(9600);
  pinMode(brightPot, INPUT);
  pinMode(speedPot, INPUT);
  pinMode(LEDI, OUTPUT);
  pinMode(LEDE1, OUTPUT);
  pinMode(LEDE2, OUTPUT);
  pinMode(LEDE3, OUTPUT);
  
}
void loop() {
  switch(mode) {
   case MODE_SOLID:
     solid();
     break;
   case MODE_BLINK:
     blinky();
     break;
   case MODE_STROBE:
     strobe();
     break;
   case MODE_SCROLL:
     scroll();
     break;
   case MODE_WAVE:
     wave();
     break;
   case MODE_GLOW:
     glow();
     break; 
   default:
     solid(); 
  }
}

void update(){
  readbrightPot = analogRead(brightPot);
  readspeedPot = analogRead(speedPot);
  writebrightPot = (255. / 1023.) * readbrightPot;
  writespeedPot = (255. / 1023.) * readspeedPot;

}

void solid(){
  update();
  analogWrite(LEDI, writebrightPot);
  analogWrite(LEDE1, writebrightPot);
  analogWrite(LEDE2, writebrightPot);
  analogWrite(LEDE3, writebrightPot);
  delay(10);
}

void blinky(){
  update();
  blink_period = (1000. / (F_MIN + (F_MAX - F_MIN)*((float)readspeedPot/1023.)));
  analogWrite(LEDI, writebrightPot);
  analogWrite(LEDE1, writebrightPot);
  analogWrite(LEDE2, writebrightPot);
  analogWrite(LEDE3, writebrightPot);
  update();
  delay(blink_period/2);
  update();
  analogWrite(LEDI, 0);
  analogWrite(LEDE1, 0);
  analogWrite(LEDE2, 0);
  analogWrite(LEDE3, 0);
  delay(blink_period/2);
}

void strobe(){
  update();
  strobe_period = (1000. / (F_MIN + (F_MAX - F_MIN)*((float)readspeedPot/1023.)));
  analogWrite(LEDI, writebrightPot);
  analogWrite(LEDE1, writebrightPot);
  analogWrite(LEDE2, writebrightPot);
  analogWrite(LEDE3, writebrightPot);
  update();
  delay(10);
  update();
  analogWrite(LEDI, 0);
  analogWrite(LEDE1, 0);
  analogWrite(LEDE2, 0);
  analogWrite(LEDE3, 0);
  delay(strobe_period - 10);
}

void scroll(){
  update();
  scroll_period = (1000. / (F_MIN + (F_MAX - F_MIN)*((float)readspeedPot/1023.)));
  analogWrite(LEDI, writebrightPot);
  analogWrite(LEDE1, 0);
  analogWrite(LEDE2, 0);
  analogWrite(LEDE3, 0);
  update();
  delay(scroll_period);
  update();
  analogWrite(LEDI, 0);
  analogWrite(LEDE1, writebrightPot);
  analogWrite(LEDE2, 0);
  analogWrite(LEDE3, 0);
  update();
  delay(scroll_period);
  update();
  analogWrite(LEDI, 0);
  analogWrite(LEDE1, 0);
  analogWrite(LEDE2, writebrightPot);
  analogWrite(LEDE3, 0);
  update();
  delay(scroll_period);
  update();
  analogWrite(LEDI, 0);
  analogWrite(LEDE1, 0);
  analogWrite(LEDE2, 0);
  analogWrite(LEDE3, writebrightPot);
  update();
  delay(scroll_period);
}

void wave(){
  update();
  wave_freq = (F_MIN + (F_MAX - F_MIN)*((float)readspeedPot/1023.));
  float w = 2.0 * PI * wave_freq * (float)n * ((float)WAVE_T/1000.);
  analogWrite(LEDI, (127. * (1. + cos(w))));
  analogWrite(LEDE1, (127. * (1. + cos(w - (PI/20)))));
  analogWrite(LEDE2, (127. * (1. + cos(w - PI))));
  analogWrite(LEDE3, (127. * (1. + cos(w - (3.*PI/2.)))));
  update();
  delay(WAVE_T);
  n++;
  if(n >= 1000./(wave_freq * WAVE_T))
  {
    n=0; //prevent overflow
  }
}

void glow(){
  update();
  wave_freq = (F_MIN + (F_MAX - F_MIN)*((float)readspeedPot/1023.));
  float w = 2.0 * PI * wave_freq * (float)n * ((float)WAVE_T/1000.);
  analogWrite(LEDI, (127. * (1. + cos(w))));
  analogWrite(LEDE1, (127. * (1. + cos(w))));
  analogWrite(LEDE2, (127. * (1. + cos(w))));
  analogWrite(LEDE2, (127. * (1. + cos(w))));
  update();
  delay(WAVE_T);
  if(n > (1000./(wave_freq*WAVE_T)))
  {
    n=0; //prevent overflow
  }
}


