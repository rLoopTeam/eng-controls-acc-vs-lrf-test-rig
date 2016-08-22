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
int i = 0;
int data = 0;
bool started = false;
String str;
String control;

void setup() {                
  Serial.begin(9600);
  Serial3.begin(9600);
  delay(100);


  Serial3.write(0x1B);
  Serial3.write(0x4F);
  Serial3.write(0x31);
  Serial3.write(0xD);
}

void loop() {
  if(!started) {
    Serial3.write(0x1B);
    Serial3.write(0x4F);
    Serial3.write(0x31);
    Serial3.write(0xD);
    delay(10);
  }

  Serial3.write(0x1B);
  Serial2.write(0x4D);
  Serial3.write(0x31);
  Serial3.write(0xD);
  delay(10);

  Serial3.write(0x1B);
  Serial3.write(0x63);
  Serial3.write(0xD);
  delay(10);

  started = true;

  while (Serial3.available() > 0) {

    str = "";
    byteRead = Serial3.read();
    if (char(byteRead) == 'D') {
      for (i=0; i<5; i++) {
        while (Serial3.available() <= 0);
        byteRead = Serial3.read();
        str += char(byteRead);
      }
      data = str.toInt();
      Serial.println(data);
    }
  }
    delay(500);
}
