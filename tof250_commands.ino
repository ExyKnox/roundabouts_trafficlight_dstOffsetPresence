// #include <SoftwareSerial.h>

// SoftwareSerial tof250(2, 3); //rx, tx

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(9600);
//   tof250.begin(9600);
// }

// void loop() {
//   // s5-EE# : set IIC address (decimal - think hex value! 12 != 0x12)
//   // s7-0# : turn off measurement (stops distance value to serial monitor) | s7-1# : turn on measurement 
//   if(tof250.available()){
//     Serial.print((char)tof250.read());
//   }

//   if(Serial.available()) {
//     tof250.write(Serial.read());
//   }
// }
