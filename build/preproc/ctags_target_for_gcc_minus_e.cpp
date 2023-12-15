# 1 "C:\\Users\\하승갑\\Nextcloud\\2023-2SEM\\캡스톤프로젝트(졸업작품)\\roundabouts_trafficlight_dstOffsetPresence\\roundabouts_trafficlight_dstOffsetPresence.ino"
# 2 "C:\\Users\\하승갑\\Nextcloud\\2023-2SEM\\캡스톤프로젝트(졸업작품)\\roundabouts_trafficlight_dstOffsetPresence\\roundabouts_trafficlight_dstOffsetPresence.ino" 2
# 3 "C:\\Users\\하승갑\\Nextcloud\\2023-2SEM\\캡스톤프로젝트(졸업작품)\\roundabouts_trafficlight_dstOffsetPresence\\roundabouts_trafficlight_dstOffsetPresence.ino" 2
# 4 "C:\\Users\\하승갑\\Nextcloud\\2023-2SEM\\캡스톤프로젝트(졸업작품)\\roundabouts_trafficlight_dstOffsetPresence\\roundabouts_trafficlight_dstOffsetPresence.ino" 2

#define DEBUG true
#define TOF250_COUNT 4
#define DETECTION_TIMEOUT 5000 /* 5 seconds*/

// defines for controlling neopixel
#define NUMPIXELS 12
#define NEOPIN 3

CRGB pixels[12];
PresenceDetectorNode tof250_nodes[4];

int tof250Addresses[4] = {0x54, 0x55, 0x56, 0x57};
static int zoneLedCount = 12 / 4;
unsigned long lastDetectionTime = 0; // 마지막으로 차량을 감지한 시간
unsigned long prevMillis = 0;

void controlTrafficLightZone(byte zoneNumber, char color) {
  // this function controls neopixels by zone
  // "zoneNumber" shall starts from 0
  // "color" shall be 'R', 'Y', 'G'
  for (byte i = zoneNumber * zoneLedCount; i < (zoneNumber * zoneLedCount) + zoneLedCount; i++) {
    // prevent out of range
    if (i > 12) break;

    if (color == 'R') {
      pixels[i] = CRGB::Red;
    }
    else if (color == 'G') {
      pixels[i] = CRGB::Green;
    }
    else if (color == 'Y') {
      pixels[i] = CRGB::Yellow;
    }
    FastLED.show();
  }
}

void setup() {
  SerialUSB.begin(9600);
  Wire.begin();
  SerialUSB.println("START");

  pinMode(3, OUTPUT);
  FastLED.addLeds<NEOPIXEL, 3>(pixels, 12);

  for (int i = 0; i < 4; ++i) {
    // create instances - each PresenceDetecterNode instance does calibration at this time
    tof250_nodes[i]= PresenceDetectorNode(tof250Addresses[i]);
  }
}

void loop() {

  if (millis() - prevMillis > 100) {
    for (int i = 0; i < 4; ++i) {
      // 차량 감지 여부 업데이트
      if (tof250_nodes[i].IsVehicleDetected()) {
        lastDetectionTime = millis(); // 차량을 감지한 시간 업데이트

        if(true) {
          SerialUSB.print("node ");
          SerialUSB.print(tof250Addresses[i], 16);
          SerialUSB.println(" VehicleDetected, traffic light on");
        }

        // 현재 인식된 노드 영역 초록색, 이외 빨간색 신호등 제어
        for (int ii = 0; ii < 4; ii++) {
          if (i == ii) controlTrafficLightZone(i, 'G');
          else controlTrafficLightZone(ii, 'R');
        }

        // TODO: 차량이 진입하면 - 해당 노드 구역 초록불, 이외 빨간불
        // else: 빨간불
        // 해당 제어를 1. 현재 진입로 차량 전부 빠져나가고 2. DETECTION_TIMEOUT 지날 때까지 유지
        while(millis() - lastDetectionTime < 5000 /* 5 seconds*/) {
          if (millis() - prevMillis > 100) {
            if (tof250_nodes[i].IsVehicleDetected()) {
              // 현재 초록불인 진입로에 차량이 아직 있는 경우
              lastDetectionTime = millis();
            } else {
              // 현재 초록불인 진입로에 차량이 없고, 회전교차로 내에 차량이 있는 경우
              // 문제는, 다른 진입로/회전교차로 내에 차량이 있어도 이 조건 충족함
              // -> 일단, 시연 환경에선 문제가 없음/테스트 필요!!
              for (int ii = 0; ii < 4; ii++) {
                if (i == ii) continue;
                if (tof250_nodes[ii].IsVehicleDetected()) {
                  lastDetectionTime = millis();
                }
              }
            }

            prevMillis = millis();
          }
        }
      }

      if (true) {
        SerialUSB.print("Sensor ");
        SerialUSB.print(tof250Addresses[i], 16);
        SerialUSB.print(" Distance: ");
        SerialUSB.print(tof250_nodes[i].GetDistance());
        SerialUSB.println(" mm");
      } else {
        SerialUSB.println("Failed to read distance.");
      }
    }

    // 차량이 인식되지 않은 경우 전부 초록색
    for (int i = 0; i < 4; i++) {
      controlTrafficLightZone(i, 'G');
    }

    prevMillis = millis();
  }
}
# 1 "C:\\Users\\하승갑\\Nextcloud\\2023-2SEM\\캡스톤프로젝트(졸업작품)\\roundabouts_trafficlight_dstOffsetPresence\\tof250_commands.ino"
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
