/*
 * Laser range finder code for the accelerometer test rig
 * By Paul L (mightyboat)
 * -------------------------------------------------------
 * LRF useful parameters
 * 0x1B - esc - go in configruation mode
 * 0x1C - separator
 * 0x43 - C continuous mode
 * 0x63 - c single mode
 * 0x64 - d error
 * 0xD  - cariage return
 * 0x4F // laser
 *  - 1    ON
 *  - 0    OFF
 */

bool started = false;
int byteRead = 0;
int CMD_LASER_ON[3] = {2, 0x4F, 0x1};       //turn laser off
int CMD_LASER_OFF[3] = {2, 0x4F, 0x0};      //turn laser on
int CMD_CONFIG[2] = {1, 0X1b};              //config mode
int CMD_SINGLE_READING[2] = {1, 0x63};      //get single readin
int CMD_CONTINUOUS_READING[2] = {1, 0x43};  //get reading continuously
int CMD_GET_ERROR[2] = {1, 0x64};           //get errors

void setup() {                
  Serial.begin(9600);
  Serial3.begin(9600);
  delay(10);
}

void loop() {
  if (!started){
    send_command(CMD_CONFIG);
    send_command(CMD_LASER_ON);
    send_command(CMD_SINGLE_READING);
  }
  
  if (Serial3.available()) {
    byteRead = Serial3.read();
    Serial.print("Distance: "); Serial.println(byteRead);
  } else {
    //send_command(CMD_GET_ERROR);
  }
  started = true;
}

/*
 * Send command to laser range finder functions
 */
void send_command(int command[]){
  Serial3.write(0x1B);
  Serial3.write(0x1B);
  for (int b=1; b <= command[0]; b++) {
    Serial3.write(command[b]);
  }
  Serial3.write(0xD); //end command
}
