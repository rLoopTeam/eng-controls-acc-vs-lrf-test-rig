/*
 * Laser range finder code for the accelerometer test rig
 * By Paul L (mightyboat) & Mila A (scrappymacgyver)
 * -------------------------------------------------------
 * 
 * Command format (see API docs for more info)
 * -------------------------------------------
 * <esc><command><value (optional) ><carriage return>
 * e.g. Turn on laser: 0x1B 0x4D 0x31 0xD (ignore spaces)
 *
 * LRF useful values
 * -----------------
 * 0x1B - esc - go in configuration mode
 * 0x1C - separator
 * 0x43 - C continuous mode
 * 0x63 - c single mode
 * 0x64 - d error
 * 0xD  - cariage return
 * 0x4F // laser
 *  - 1    ON
 *  - 0    OFF
 */

// setup variables to be used
int byteRead = 0;
int i = 0;
float prev_dist = 0.0;
float current_dist = 0.0;
float delta = 0.0;
float dt = 50.0/1000.0;
float velocity = 0.0;
bool started = false;
String str;

void setup() {  
  /*
   * initialise the USB and hardware serial ports
   */
  Serial.begin(9600); // USB
  Serial3.begin(9600);// Hardware
  delay(100);
  
  /*
   * Send "O1" command (turn on laser) through hardware serial port 3
   */
  Serial3.write(0x1B);
  Serial3.write(0x4F);
  Serial3.write(0x31);
  Serial3.write(0xD);

}

void loop() {
  if(!started) {
  /*
   * Send "O1" command(turn on laser) again and only on startup, through hardware serial port 3
   */
  Serial3.write(0x1B);
  Serial3.write(0x4F);
  Serial3.write(0x31);
  Serial3.write(0xD);
  delay(10); // delay needed for code to turn consistently (we think?)

  }
  /*
   * Send "M1" command(set measurement mode to continuous ASCII) through hardware serial port 3
   */ 
  Serial3.write(0x1B);
  Serial3.write(0x4D);
  Serial3.write(0x31);
  Serial3.write(0xD);
  delay(10); // delay needed for code to turn consistently (we think?)

  /*
   * Send "c" command(get single distance measurement) through hardware serial port 3
   */
  Serial3.write(0x1B);
  Serial3.write(0x63);
  Serial3.write(0xD);
  delay(10); // delay needed for code to turn consistently (we think?)
  
  // set bool to true so that the init code doesn't run again. Note: May be redundant
  started = true;

  /*
   * While there is data available on the serial port (connected to laser), run the code for every character
   */
  while (Serial3.available() > 0) {

    //reset the result string
    str = "";
    
    //read the first character available and clear the buffer
    byteRead = Serial3.read();
    
    // if character is D, we know it's a measurement (see API doc for more information)
    if (char(byteRead) == 'D') {
      // we know a measurement consists of five numbers, so run a loop with 5 iterations
      for (i=0; i<5; i++) {
        // block the loop until buffer contains the next character
        while (Serial3.available() <= 0);
        // read character and clear buffer again
        byteRead = Serial3.read();
        // add character to result string
        str += char(byteRead);
      }
      // convert result string to a number and divide by 1000 to get the value in meters
      current_dist = str.toFloat() / 1000;
      // get the difference between current distance and previous distance
      delta = prev_dist - current_dist;
      // calculate velocity
      velocity = delta*dt;
      // set lagging variable for previous distance
      prev_dist = current_dist;
      
      /*
       * Print result to USB serial port so the data can be analysed
       */
      Serial.print(velocity, 12);
      Serial.print(",");
      Serial.println(current_dist, 12);
    }
  }
  delay(50);
}
