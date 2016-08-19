
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

Adafruit_MMA8451 mma = Adafruit_MMA8451();

float x_acc = 0;
float y_acc = 0;

float x_cal = 0; 
float y_cal = 0;

float x_vel = 0;
float y_vel = 0;

float x_dist = 0;
float y_dist = 0;

float kalman_vel_1 = 0;
float kalman_vel_2 = 0;

float kalman_dist_1 = 0;
float kalman_dist_2 = 0;

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
  
  mma.setRange(MMA8451_RANGE_2_G);

  /* do gravity calibration on startup*/
  Serial.println("Calibrating..");
  calibrate();
  k_state_1 = kalman_init(0.125, 10, 300, 0);
  k_state_2 = kalman_init(0.125, 1, 300, 0);
}

/* calibrate for gravity by taking the average of the first 100 samples */
void calibrate() {
  int i;
  float x_tot = 0;
  float y_tot = 0;
  
  sensors_event_t event;
  
  for (i=0; i<100; i++){
    mma.getEvent(&event);
    /* calibrate for X and Y separately, we dont care about Z (down) right now */
    x_tot = x_tot+event.acceleration.x;
    y_tot = y_tot+event.acceleration.y;
    delay(5);
  }

  x_cal = x_tot/100;
  y_cal = y_tot/100;
  
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
  sensors_event_t event; 
  mma.getEvent(&event);

  /* remove gravity calibration value from sensor value */
  x_acc = event.acceleration.x - x_cal;
  y_acc = event.acceleration.y - y_cal;

  if (abs(x_acc) < 0.05 ) {
    return;
  }

  /*
  if (x_acc < 0){
    x_acc = -x_acc;
  }

  if (y_acc < 0){
    y_acc = -y_acc;
  }
  */

  x_vel += x_acc;
  x_dist += x_vel;

  kalman_update(&k_state_1, x_acc);
  kalman_update(&k_state_2, x_acc);

  kalman_vel_1 += k_state_1.x;
  kalman_vel_2 += k_state_2.x;

  kalman_dist_1 += kalman_vel_1;
  kalman_dist_2 += kalman_vel_2;
  
  Serial.print(x_acc); Serial.print(",");
  Serial.print(y_acc); Serial.print(",");  
  Serial.print(x_vel/200); Serial.print(",");
  Serial.print(y_vel/200); Serial.print(",");
  Serial.print(x_dist/200); Serial.print(",");
  Serial.print(y_dist/200); Serial.print(","); 
  Serial.print(k_state_1.x); Serial.print(",");
  Serial.print(k_state_2.x); Serial.print(",");
  Serial.print(kalman_vel_1/200); Serial.print(",");
  Serial.print(kalman_vel_2/200); Serial.print(",");
  Serial.print(kalman_dist_1/200); Serial.print(",");
  Serial.print(kalman_dist_2/200);
  Serial.println();
  
  delay(5);
  
}
