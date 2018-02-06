
/**
 Collectin Light - Intermedium
 
 This program (v1.0) takes values from IR sensor and 
 changes the frequency of sinusoidal lighting pattern acording to such a sensor value.
 
 A change in the phase of the lighting for each LED is also implemented.
 For that, the derivate of sensor value is used.
 
 */

//#define IR  5 // PIN of the IR sensor. Analog input.
#define IR  1 // PIN of the IR sensor. Analog input.

#define LED_NUM  3  // Number of LEDs

//#define DEBUG    // Uncomment if you want to send values by serial port for debuging.
                    // Notice that serial writings delay the loop function, as
                    // each writing takes about 20 ms.

int led_pins[LED_NUM] = {3, 5, 6};  // PINs for the LEDs. Analog output.

void setup() {
  
  for( unsigned int i = 0; i < LED_NUM; i++ ) {
    pinMode(led_pins[i], OUTPUT);
  }
#ifdef DEBUG  
  Serial.begin(9600);
#endif
}

int tau = 1; // time in ms of the delay between iterations.
int t;      // time counter for the sinusoidal pattern

long Tmax = 1000 * tau;  // Maximum period of the pattern  
long Tmin = Tmax/10;    // Minimum period of the pattern
unsigned long T = Tmax;  // Instant Period. It is initilized to Tmax.
double desfase = 2 * PI / LED_NUM;  // Instant phase of the sinusoidal pattern.

int dmax = 300;    // Theses are the max and min of the sensor reading.
int dmin = 20;      // max is close, min is far
int d = 0, d_old = 0, dev_d;  // Instant distance taken from sensor reading.  
                              // d_old and dev_d is derivate of such distance.

void loop() {

  d = analogRead( IR );  // read sensor value
  dev_d = d - d_old;     // calculate derivate of such value
  
  // sets amplitude for each LED. Notice the role of desfase.
  int amplitude[LED_NUM];    
  for( unsigned int i = 0; i < LED_NUM; i++ ) {
    amplitude[i] = 255 * ( 1 + sin( 2 * PI * t / T + desfase * i ))/2;
  }

  t += tau; // count time passing
  if( t/T >= 1 ) {   // handle if time pass one period and if so initialize time
    T = abs( map( d, dmax, dmin, Tmin, Tmax ) ); // updates period value

    // update phase value
    if( abs(dev_d) > 10 ) desfase += 2 * PI / LED_NUM ;
    else desfase -= PI / 32;
    if(desfase < 0) desfase = 0;
 
 /*  // this updating for the phase also works quite well 
    if( d < dmax/4 ) desfase = 0;
    else if( d > dmax/4 && d < dmax/2 ) desfase = PI / 2;
    else if( d > dmax/2 && d < 3*dmax/4 ) desfase = PI;
    else if( d > 3*dmax/4 ) desfase = 3 * PI / 2;
   */   
    t = 0;
  }
  
  // change the amplitude of each vector
  blink_all( amplitude ); 

  d_old = d; // for calculating distance derivate

// Debuging stuff
#ifdef DEBUG  
  Serial.print( "d[0] = " ); Serial.print( d ); 
//  Serial.print( "  d[1] = " ); Serial.print( d_vector[1] ); 
//  Serial.print( "  d[2] = " ); Serial.print( d_vector[2] ); 
  Serial.print( "  dev_d = " ); Serial.print( dev_d ); 
  Serial.print( "  desfase = " ); Serial.print( desfase ); 
//  Serial.print( "  dmin = " ); Serial.print( dmin ); 
//  Serial.print( "  dmax = " ); Serial.print( dmax );  
  Serial.print( "  A = " ); Serial.print( amplitude[0] );
  Serial.print( "  T = " ); Serial.println( T );
#endif

  delay(tau);
}

void blink_all( int amp[] ) {
  for( int i = 0; i < LED_NUM; i++ ) {
      analogWrite(led_pins[i], amp[i]);    
  }    
}


