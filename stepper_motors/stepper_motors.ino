/*
 * Code for the stepper motors on the acelerometer test rig
 * Authors: Mila (@scrappymcguyver), Paul L (@mightyboat), Joakim (@vookungdoofu)
 * 
 * The teensy and stepper motor power supplies should be plugged into the same "on/off" switch.
 * You dont want to keep the teensy running while the power for the motors is off.
 * Then it's going to start at a random position and you wont get consistent results.
 * 
 * The "sled" should be placed in the middle of the test track at the start of a test-run.
 */

#include <AccelStepper.h>

/* Define a stepper and the pins it will use */
AccelStepper stepper(1, 3, 4);

/* Distance that will be traveled */
int pos = 2300;

void setup()
{
  Serial.begin(9600);  
  /* Sets the max speed and acceleration of the motors.
  * Faster speed and accel might mean you need to increase voltage supplied to the motors. 
  * If they start "stuttering" they need more voltage.
  */
  stepper.setMaxSpeed(4000);
  stepper.setAcceleration(4000);
  
  stepper.setCurrentPosition(0);
  
  /* Small delay to give the power supply time to power up before the loop starts. */
  delay(5000);
}

void loop()
{
  if (stepper.distanceToGo() == 0)
  {
    /* This delay makes the sled "pause" at each end of the track.
    *  That gives a nice flat area on the graph later so you can easily see when it changed direction.
    */
    delay(500);
    pos = -pos;
    stepper.moveTo(pos);
  }
  Serial.println(stepper.currentPosition());
  stepper.run();
}
