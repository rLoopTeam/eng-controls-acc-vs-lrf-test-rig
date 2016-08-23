
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

Adafruit_MMA8451 mma = Adafruit_MMA8451();

float x_acc = 0.00;
float y_acc = 0.00;

float x_cal = 0.00; 
float y_cal = 0.00;

float x_vel = 0.00;
float y_vel = 0.00;

float x_dist = 0.00;
float y_dist = 0.00;

float kalman_vel_1 = 0.00;
float kalman_vel_2 = 0.00;

float kalman_dist_1 = 0.00;
float kalman_dist_2 = 0.00;

float last_millis = 0.00;
float now_millis = 0.00;

float diff = 0.00;
float delta = 0.00;

float dt = 51 / 1000.00;

sensors_event_t event; 

typedef struct {
  double q; //process noise covariance
  double r; //measurement noise covariance
  double x; //value
  double p; //estimation error covariance
  double k; //kalman gain
} kalman_state;

kalman_state k_state_1;
kalman_state k_state_2;

/* initialize a kalman state */
kalman_state kalman_init(double q, double r, double p, double intial_value)
{
  kalman_state result;
  result.q = q;
  result.r = r;
  result.p = p;
  result.x = intial_value;

  return result;
}

void setup(void) {
  Serial.begin(9600);

  if (! mma.begin()) {
    Serial.println("Couldnt start");
    while (1);
  }
  
  mma.setRange(MMA8451_RANGE_8_G);

  /* do gravity calibration on startup*/
  Serial.println("Calibrating..");
  calibrate();
  k_state_1 = kalman_init(0.125, 10, 300, 0);
  k_state_2 = kalman_init(0.125, 1, 300, 0);
}

/* calibrate for gravity by taking the average of the first 100 samples */
void calibrate() {
  int i;
  float x_tot = 0.00;
  float y_tot = 0.00;
  
  sensors_event_t event;
  
  for (i=0; i<100; i++){
    mma.getEvent(&event);
    /* calibrate for X and Y separately, we dont care about Z (down) right now */
    x_tot = x_tot+event.acceleration.x;
    y_tot = y_tot+event.acceleration.y;
    delay(5);
  }

  x_cal = x_tot/100.00;
  y_cal = y_tot/100.00;
}

void kalman_update(kalman_state* state, double measurement)
{
  //prediction update
  //omit x = x
  state->p = state->p + state->q;

  //measurement update
  state->k = state->p / (state->p + state->r);
  state->x = state->x + state->k * (measurement - state->x);
  state->p = (1 - state->k) * state->p;
}

void loop() {
  /* Get a new sensor event */ 
  now_millis = millis();
  mma.getEvent(&event);
  
  /* remove gravity calibration value from sensor value */
  x_acc = event.acceleration.x - x_cal;
  y_acc = event.acceleration.y - y_cal;

  x_vel = x_vel + (x_acc * dt); 
  
  x_dist = x_dist + (x_vel * dt); 

  kalman_update(&k_state_1, x_acc);
  kalman_update(&k_state_2, x_acc);

  kalman_vel_1 = kalman_vel_1 + (k_state_1.x * dt);
  kalman_vel_2 = kalman_vel_2 + (k_state_2.x * dt);

  kalman_dist_1 = kalman_dist_1 + (kalman_vel_1 * dt);
  kalman_dist_2 = kalman_dist_2 + (kalman_vel_2 * dt);
  
  Serial.print(x_acc); Serial.print(",");
  Serial.print(y_acc); Serial.print(",");  
  Serial.print(x_vel); Serial.print(",");
  Serial.print(y_vel); Serial.print(",");
  Serial.print(x_dist); Serial.print(",");
  Serial.print(y_dist); Serial.print(","); 
  Serial.print(k_state_1.x); Serial.print(",");
  Serial.print(k_state_2.x); Serial.print(",");
  Serial.print(kalman_vel_1); Serial.print(",");
  Serial.print(kalman_vel_2); Serial.print(",");
  Serial.print(kalman_dist_1); Serial.print(",");
  Serial.print(kalman_dist_2); Serial.print(",");
  Serial.print(dt);
  Serial.println();

  last_millis = now_millis;
  delay(50);
}
