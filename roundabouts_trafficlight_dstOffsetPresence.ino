#include <Wire.h>
#include <FastLED.h>
#include "PresenceDetectorNode.h"

#define DEBUG true
#define TOF250_COUNT 4
#define DETECTION_TIMEOUT 5000  // 5 seconds

// defines for controlling neopixel
#define NUMPIXELS 12
#define NEOPIN 3

CRGB pixels[NUMPIXELS];
PresenceDetectorNode tof250_nodes[TOF250_COUNT];

int tof250Addresses[TOF250_COUNT] = {0x54, 0x55, 0x56, 0x57};
static int zoneLedCount = NUMPIXELS / 4;
unsigned long lastDetectionTime = 0;  // 마지막으로 차량을 감지한 시간
unsigned long prevMillis = 0;

void controlTrafficLightZone(byte zoneNumber, char color) {
  // this function controls neopixels by zone
  // "zoneNumber" shall starts from 0
  // "color" shall be 'R', 'Y', 'G'
  for (byte i = zoneNumber * zoneLedCount; i < (zoneNumber * zoneLedCount) + zoneLedCount; i++) {
    // prevent out of range
    if (i > NUMPIXELS) break;

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
  Serial.begin(9600);
  Wire.begin();
  Serial.println("START");

  pinMode(NEOPIN, OUTPUT);
  FastLED.addLeds<NEOPIXEL, NEOPIN>(pixels, NUMPIXELS);

  for (int i = 0; i < TOF250_COUNT; ++i) {
    // create instances - each PresenceDetecterNode instance does calibration at this time
    tof250_nodes[i]= PresenceDetectorNode(tof250Addresses[i]);
  }
}

void loop() {

  if (millis() - prevMillis > 100) {
    for (int i = 0; i < TOF250_COUNT; ++i) {
      // 차량 감지 여부 업데이트
      if (tof250_nodes[i].IsVehicleDetected()) {
        lastDetectionTime = millis();  // 차량을 감지한 시간 업데이트

        if(DEBUG) {
          Serial.print("node ");
          Serial.print(tof250Addresses[i], HEX);
          Serial.println(" VehicleDetected, traffic light on");
        }

        // 현재 인식된 노드 영역 초록색, 이외 빨간색 신호등 제어
        for (int ii = 0; ii < 4; ii++) {
          if (i == ii) controlTrafficLightZone(i, 'G');
          else controlTrafficLightZone(ii, 'R');
        }

        // TODO: 차량이 진입하면 - 해당 노드 구역 초록불, 이외 빨간불
        // else: 빨간불
        // 해당 제어를 1. 현재 진입로 차량 전부 빠져나가고 2. DETECTION_TIMEOUT 지날 때까지 유지
        while(millis() - lastDetectionTime < DETECTION_TIMEOUT) {
          if (millis() - prevMillis > 100) {
            if (tof250_nodes[i].IsVehicleDetected()) {
              // 현재 초록불인 진입로에 차량이 아직 있는 경우
              lastDetectionTime = millis();
            } else {
              // 현재 초록불인 진입로에 차량이 없고, 회전교차로 내에 차량이 있는 경우
              // 문제는, 다른 진입로/회전교차로 내에 차량이 있어도 이 조건 충족함
              // -> 일단, 시연 환경에선 문제가 없음/테스트 필요!!
              for (int ii = 0; ii < TOF250_COUNT; ii++) {
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

      if (DEBUG) {
        Serial.print("Sensor ");
        Serial.print(tof250Addresses[i], HEX);
        Serial.print(" Distance: ");
        Serial.print(tof250_nodes[i].GetDistance());
        Serial.println(" mm");
      } else {
        Serial.println("Failed to read distance.");
      }
    }

    // 차량이 인식되지 않은 경우 전부 초록색
    for (int i = 0; i < 4; i++) {
      controlTrafficLightZone(i, 'G');
    }

    prevMillis = millis();
  }
}
