/*
 * Laser range finder code for the accelerometer test rig
 * By Paul L (mightyboat) & Mila A (scrappymacgyver)
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

int byteRead = 0;
int CMD_LASER_ON[3] = {2, 0x4F, 0x31};      //"O1" turn laser on
int CMD_LASER_OFF[3] = {2, 0x4F, 0x30};     //"O0" turn laser off
// int CMD_CONFIG[2] = {1, 0x1B};              //"<esc>" config mode
// int CMD_SINGLE_READING[2] = {1, 0x63};      //"c" get single reading 
int CMD_CONTINUOUS_READING[2] = {1, 0x43};  //"C" get reading continuously
int CMD_MEASUREMENT_MODE_CONTINUOUS_ASCII[3] = {2, 0x4D, 0x31};  //"M1" continuous measurement in ascii mode 
int CMD_GET_ERROR[2] = {1, 0x64};           //"d" get errors

void setup() {                
  Serial.begin(9600);
  Serial3.begin(9600);
  delay(10);

  send_command(CMD_LASER_OFF);
  send_command(CMD_LASER_ON);
  send_command(CMD_CONTINUOUS_READING);
  send_command(CMD_MEASUREMENT_MODE_CONTINUOUS_ASCII);
}

void loop() {
  
  while (Serial3.available()) {
    byteRead = Serial3.read();
    if(byteRead == 10 || byteRead2 == 13) {
      Serial.println('\n');
    } else {
      Serial.print(char(byteRead));  
    }    
  } else {
    send_command(CMD_GET_ERROR);
  }
  delay(10);
}

// Command wrapper <esc>...<cr> where <esc> is (0x1B) and <cr> is (0xD)
void send_command(int command[]){
  Serial3.write(0x1B);
  for (int b=1; b <= command[0]; b++) {
    Serial3.write(command[b]);
  }
  Serial3.write(0xD); 
}



