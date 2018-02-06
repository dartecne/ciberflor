
/**
 Collectin Light - Intermedium
 
 This program (v1.0) takes values from IR sensor and 
 changes the frequency of sinusoidal lighting pattern acording to such a sensor value.
 
 A change in the phase of the lighting for each LED is also implemented.
 For that, the derivate of sensor value is used.
 
 */
#include <Servo.h>
#include "pitches.h"

//#define IR  5 // PIN of the IR sensor. Analog input.
#define IR  1 // PIN of the IR sensor. Analog input.

#define LED_NUM  3  // Number of LEDs

//#define DEBUG    // Uncomment if you want to send values by serial port for debuging.
                    // Notice that serial writings delay the loop function, as
                    // each writing takes about 20 ms.

int led_pins[LED_NUM] = {3, 5, 6};  // PINs for the LEDs. Analog output.
Servo myservo;
int pos = 0;

#define STATE_IDLE 0
#define STATE_CLOSE 1
#define STATE_CALM 2
#define STATE_FAR 3

int state = STATE_IDLE;

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void setup() {
  myservo.attach( 9 );
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }

  for( unsigned int i = 0; i < LED_NUM; i++ ) {
    pinMode(led_pins[i], OUTPUT);
  }
#ifdef DEBUG  
  Serial.begin(9600);
#endif
  setup_tone();
}

int tau = 1; // time in ms of the delay between iterations.
int t;      // time counter for the sinusoidal pattern

long Tmax = 1000 * tau;  // Maximum period of the pattern  
long Tmin = Tmax/10;    // Minimum period of the pattern
unsigned long T = Tmax;  // Instant Period. It is initilized to Tmax.
unsigned long T_servo = Tmax;  // Instant Period. It is initilized to Tmax.
double desfase = 2 * PI / LED_NUM;  // Instant phase of the sinusoidal pattern.

/*  90 - mas de 2m
 *  170 - 30cm  
 *  600 - maximo cercano

*/
int dmax = 600;    // Theses are the max and min of the sensor reading.
int dmin = 90;      // max is close, min is far
int d = 0, d_old = 0, dev_d;  // Instant distance taken from sensor reading.  
                              // d_old and dev_d is derivate of such distance.
int amplitude[LED_NUM];   
int servo_pos = 0;
float amp_modulation = 1.0; 
float freq_modulation = 1.0;

int counter = 0;

void loop() {
  d = analogRead( IR );  // read sensor value  
  dev_d = d - d_old;     // calculate derivate of such value
  amp_modulation = 3 * d / dmax;
    
  // sets amplitude for each LED. Notice the role of desfase.
  for( unsigned int i = 0; i < LED_NUM; i++ ) {
    amplitude[i] = 255 * ( 1 + sin( 2 * PI * t / T + desfase * i ))/2;
  }
//  servo_pos = amp_modulation * 180 * ( 1 + sin( 2 * PI * t * freq_modulation / T  ))/2;
  servo_pos = 180 * ( 1 + sin( 2 * PI * t * freq_modulation / T_servo  ))/2;
  T_servo = T * amp_modulation + 2 * Tmax;    
  t += tau; // count time passing
  
  if( t/T >= 1 ) {   // handle if time pass one period and if so initialize time
    T = abs( map( d, dmax, dmin, Tmin, Tmax ) ); // updates period value

    // update phase value
    if( abs(dev_d) > 10 ) desfase += 2 * PI / LED_NUM ;
    else desfase -= PI / 32;
    if(desfase < 0) desfase = 0;
 
   // this updating for the phase also works quite well 
 /*   if( d < dmax/4 ) desfase = 0;
    else if( d > dmax/4 && d < dmax/2 ) desfase = PI / 2;
    else if( d > dmax/2 && d < 3*dmax/4 ) desfase = PI;
    else if( d > 3*dmax/4 ) desfase = 3 * PI / 2;
   */   
    t = 0;
  }
  
  // change the amplitude of each vector
  blink_all( amplitude );
  myservo.write( servo_pos  );
  if( d > dmax/2 ) tone( 10, map( d, dmax/2, dmax, 60, 2000 ) );
  
  d_old = d; // for calculating distance derivate

#ifdef DEBUG  
  debug();
#endif

  delay(tau);
}

void blink_all( int amp[] ) {
  for( int i = 0; i < LED_NUM; i++ ) {
      analogWrite(led_pins[i], amp[i]);    
  }    
}

void setup_tone() {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(10, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(10);
  }
}
void debug() {
  Serial.print( "d[0] = " ); Serial.print( d ); 
//  Serial.print( "  d[1] = " ); Serial.print( d_vector[1] ); 
//  Serial.print( "  d[2] = " ); Serial.print( d_vector[2] ); 
  Serial.print( "  dev_d = " ); Serial.print( dev_d ); 
  Serial.print( "  desfase = " ); Serial.print( desfase ); 
//  Serial.print( "  dmin = " ); Serial.print( dmin ); 
//  Serial.print( "  dmax = " ); Serial.print( dmax );  
  Serial.print( "  A = " ); Serial.print( amplitude[0] );
  Serial.print( "  T = " ); Serial.println( T );
}


