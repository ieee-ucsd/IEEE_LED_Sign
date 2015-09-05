/*
 * Based off the original IEEE LED Sign created by Alex Forencich.
 * The modes were created by Alex, and the IEEE LED Sign used 2 potentiometers
 * and a button. One potentiometer for adjusted the speed, and the other
 * adjusted for brightness. The button changed through the modes.
 * The original code was written with a focus on low-level register manipulation.
 * 
 * This new version still uses 2 potentiometers and a button for the
 * same purposes. But it is configured so it can be controlled using
 * an Android app called "ArduDroid". Due to the nature of switching between
 * physical and bluetooth by the user, a timer interrupt has been employed to 
 * allow everything to run smoothly.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define MODE_SOLID 2
#define MODE_BLINK 3
#define MODE_STROBE 4
#define MODE_SCROLL 5
#define MODE_WAVE 6
#define MODE_GLOW 7
volatile byte mode = MODE_SOLID;
volatile byte t_mode;

#define F_MIN 0.5   // min freq in Hz
#define F_MAX 20.0  // max freq in Hz
#define WAVE_T 20   // sampling period for wave pattern in ms

int button = 2;
int LEDI = 3;
int LEDE1 = 5;
int LEDE2 = 6;
int LEDE3 = 9;
int brightPot = 10;
int speedPot = 11;
float readbrightPot = 0;
float readspeedPot = 0;
float calcbrightPot = 0;
float calcspeedPot = 0;

unsigned int blink_period;
unsigned int strobe_period;
unsigned int scroll_period;
unsigned int wave_freq;
unsigned int n = 0;

volatile int state = HIGH;

#define START_CMD_CHAR '*'
#define END_CMD_CHAR '#'
#define DIV_CMD_CHAR '|'
#define CMD_DIGITALWRITE 10
#define CMD_ANALOGWRITE 11
#define CMD_TEXT 12
#define CMD_READ_ARDUDROID 13
#define MAX_COMMAND 20  // max command number code. used for error checking.
#define MIN_COMMAND 10  // minimum command number code. used for error checking. 
#define IN_STRING_LENGHT 40
#define MAX_ANALOGWRITE 255
#define PIN_HIGH 3
#define PIN_LOW 2

String inText;

void setup() {
  Serial.begin(9600);
  Serial.flush();
  pinMode(button, OUTPUT);
  attachInterrupt(0, ISR_button, RISING);

  cli();
  TCCR1A=0;
  TCCR1B=0;
  OCR1A = 199999; //compare match register to desired timer count
  TCCR1B |= (1 << WGM12); //turn on CTC
  TCCR1B |= (1 << CS10); //set CS12 bit for no prescaler
  TIMSK1 |= (1 <<  OCIE1A); //set interrupt on compare match register
  sei();

}

void loop() {
  Serial.flush();
  int ard_command = 0;
  int pin_num = 0;
  int pin_value = 0;

  char get_char = ' ';  //read serial

  // wait for incoming data
  if (Serial.available() < 1) return; // if serial empty, return to loop().

  // parse incoming command start flag
  get_char = Serial.read();
  if (get_char != START_CMD_CHAR) return; // if no command start flag, return to loop().

  // parse incoming command type
  ard_command = Serial.parseInt(); // read the command

  // parse incoming pin# and value
  pin_num = Serial.parseInt(); // read the pin
  pin_value = Serial.parseInt();  // read the value

  // 2) GET digitalWrite DATA FROM ARDUDROID
  if (ard_command == CMD_DIGITALWRITE) {
    mode = pin_num;
    return;  // return from start of loop()
  }

  // 3) GET analogWrite DATA FROM ARDUDROID
  if (ard_command == CMD_ANALOGWRITE) {
    if (pin_num == 10) {
      //Serial.println("pin_num, pin_value");
      calcbrightPot = pin_value;
    }
    else if (pin_num == 11) {
      //Serial.println("pin_num, pin_value");
      calcspeedPot = pin_value;
    }
    return;  // Done. return to loop();
  }

  // 4) SEND DATA TO ARDUDROID
  if (ard_command == CMD_READ_ARDUDROID) { 
    Serial.print(" Mode: "); 
    Serial.print(mode);
    Serial.print(" , Analog 1 = ");
    Serial.println(analogRead(A1));
    return;  // Done. return to loop();
  }
}

void update() {
  readbrightPot = analogRead(brightPot);
  readspeedPot = analogRead(speedPot);
  calcbrightPot = (255. / 1023.) * readbrightPot;
  calcspeedPot = (255. / 1023.) * readspeedPot;
}

void solid() {
  Serial.println("Mode SOLID");
  update();
  analogWrite(LEDI, calcbrightPot);
  analogWrite(LEDE1, calcbrightPot);
  analogWrite(LEDE2, calcbrightPot);
  analogWrite(LEDE3, calcbrightPot);
  delay(10);
}

void blinky() {
  Serial.println("Mode Blinky");
  update();
  blink_period = (1000. / (F_MIN + (F_MAX - F_MIN) * (calcspeedPot * 255.)));
  analogWrite(LEDI, calcbrightPot);
  analogWrite(LEDE1, calcbrightPot);
  analogWrite(LEDE2, calcbrightPot);
  analogWrite(LEDE3, calcbrightPot);
  update();
  delay(blink_period / 2);
  analogWrite(LEDI, 0);
  analogWrite(LEDE1, 0);
  analogWrite(LEDE2, 0);
  analogWrite(LEDE3, 0);
  update();
  delay(blink_period / 2);
}

void strobe() {
  Serial.println("Mode Strobe");
  update();
  strobe_period = (1000. / (F_MIN + (F_MAX - F_MIN) * (calcspeedPot * 255.)));
  analogWrite(LEDI, calcbrightPot);
  analogWrite(LEDE1, calcbrightPot);
  analogWrite(LEDE2, calcbrightPot);
  analogWrite(LEDE3, calcbrightPot);
  update();
  delay(10);
  update();
  analogWrite(LEDI, 0);
  analogWrite(LEDE1, 0);
  analogWrite(LEDE2, 0);
  analogWrite(LEDE3, 0);
  delay(strobe_period - 10);
}

void scroll() {
  Serial.println("Mode Scroll");
  update();
  scroll_period = (1000. / (F_MIN + (F_MAX - F_MIN) * (calcspeedPot * 255.)));
  analogWrite(LEDI, calcbrightPot);
  analogWrite(LEDE1, 0);
  analogWrite(LEDE2, 0);
  analogWrite(LEDE3, 0);
  update();
  delay(scroll_period);
  update();
  analogWrite(LEDI, 0);
  analogWrite(LEDE1, calcbrightPot);
  analogWrite(LEDE2, 0);
  analogWrite(LEDE3, 0);
  update();
  delay(scroll_period);
  update();
  analogWrite(LEDI, 0);
  analogWrite(LEDE1, 0);
  analogWrite(LEDE2, calcbrightPot);
  analogWrite(LEDE3, 0);
  update();
  delay(scroll_period);
  update();
  analogWrite(LEDI, 0);
  analogWrite(LEDE1, 0);
  analogWrite(LEDE2, 0);
  analogWrite(LEDE3, calcbrightPot);
  update();
  delay(scroll_period);
}

void wave() {
  Serial.println("Wave");
  update();
  wave_freq = (F_MIN + (F_MAX - F_MIN) * (calcspeedPot * 255.));
  float w = 2.0 * PI * wave_freq * (float)n * ((float)WAVE_T / 1000.);
  analogWrite(LEDI, (127. * (1. + cos(w))));
  analogWrite(LEDE1, (127. * (1. + cos(w - (PI / 20)))));
  analogWrite(LEDE2, (127. * (1. + cos(w - PI))));
  analogWrite(LEDE3, (127. * (1. + cos(w - (3.*PI / 2.)))));
  update();
  delay(WAVE_T);
  n++;
  if (n >= 1000. / (wave_freq * WAVE_T)) {
    n = 0; //prevent overflow
  }
}

void glow() {
  Serial.println("Glow");
  update();
  wave_freq = (F_MIN + (F_MAX - F_MIN) * (calcspeedPot * 255.));
  float w = 2.0 * PI * wave_freq * (float)n * ((float)WAVE_T / 1000.);
  analogWrite(LEDI, (127. * (1. + cos(w))));
  analogWrite(LEDE1, (127. * (1. + cos(w))));
  analogWrite(LEDE2, (127. * (1. + cos(w))));
  analogWrite(LEDE3, (127. * (1. + cos(w))));
  update();
  delay(WAVE_T);
  if (n > (1000. / (wave_freq * WAVE_T))) {
    n = 0; //prevent overflow
  }
}

volatile int __x;
volatile int __i;
void ISR_button() {
int switchState = digitalRead(4);
  //if (0) {
  if (switchState == HIGH) {
    mode = ((mode - 1) % 6) + 2;
  }
  else {
    
  }
  for (__i = 0; __i < 10000; __i++) {
    __x = (__x * __i) % __i;
  }
}

ISR(TIMER1_COMPA_vect) {
  switch (mode) {
    case MODE_GLOW:
      glow();
      break;
    case MODE_WAVE:
      wave();
      break;
    case MODE_SCROLL:
      scroll();
      break;
    case MODE_STROBE:
      strobe();
      break;
    case MODE_BLINK:
      blinky();
      break;
    case MODE_SOLID:
      solid();
      break;
    default:
      solid();
      break;
  }
}
