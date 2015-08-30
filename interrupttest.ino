//In Arduino. Testing if interrupt with button worked.

#define MODE_SOLID 0
#define MODE_BLINK 1
#define MODE_STROBE 2
#define MODE_SCROLL 3
#define MODE_WAVE 4
#define MODE_GLOW 5
volatile byte mode = MODE_SOLID;

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

int button = 2;
volatile int state = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(brightPot, INPUT);
  pinMode(speedPot, INPUT);
  pinMode(LEDI, OUTPUT);
  pinMode(LEDE1, OUTPUT);
  pinMode(LEDE2, OUTPUT);
  pinMode(LEDE3, OUTPUT);
  pinMode(button, OUTPUT);
  attachInterrupt(0, ISR_button, RISING);

}

void loop() {
switch(mode) {
   case MODE_SOLID:
     solid();
     break;
   case MODE_BLINK:
     blinky();
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
  Serial.println("Mode SOLID");
  update();
  analogWrite(LEDI, writebrightPot);
  analogWrite(LEDE1, writebrightPot);
  analogWrite(LEDE2, writebrightPot);
  analogWrite(LEDE3, writebrightPot);
  delay(10);
}

void blinky(){
  Serial.println("Mode Blinky");
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

volatile int __x;
volatile int __i;

void ISR_button() {
  mode = (mode + 1) % 2;
  for(__i = 0; __i < 10000; __i++) {
    __x = (__x * __i) % __i; 
  }
}
