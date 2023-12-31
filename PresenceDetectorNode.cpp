#include "PresenceDetectorNode.h"

PresenceDetectorNode::PresenceDetectorNode() {
    // empty constructor for global variable.
}

PresenceDetectorNode::PresenceDetectorNode(int address) {
    // constructor, initializes baseline distance value.
    node_addr = address;
    CheckRegisterAddress();

    // calibrate node's baseline distance value. get average of 5 times
    // ToF250 sampling rate is 10hz(at default)
    // so use 100ms of delay to get proper value at proper timing.
    unsigned int distanceSum = 0;
    for (byte i = 0; i < 5; i++) {
        distanceSum += ReadTOF250Distance();
        delay(100);
    }
    baseline_distance = distanceSum / 5;

    if (DEBUG) {
        Serial.print("Node addr: ");
        Serial.print(node_addr, HEX);
        Serial.println(" Register Address is OK"); 
    }
}

void PresenceDetectorNode::CheckRegisterAddress() {
    Wire.beginTransmission(node_addr);
    Wire.write(0x00);
    int error = Wire.endTransmission();

    if (error == 0 && DEBUG) {
        Serial.print("Node addr: ");
        Serial.print(node_addr, HEX);
        Serial.println(" Register Address is OK");
    } else {
        Serial.print("Node addr: ");
        Serial.print(node_addr, HEX);
        Serial.println(" Register Address Check Failed");
    }
}

int PresenceDetectorNode::ReadTOF250Distance() {
    Wire.beginTransmission(node_addr);
    Wire.write(0x00);
    int error = Wire.endTransmission();

    if (error != 0 && DEBUG) {
        Serial.print("I2C Error: ");
        Serial.print(error);
        Serial.print(" Node addr: ");
        Serial.print(node_addr, HEX);
        Serial.println(", Failed to read distance.");
        return -1;
    }

    Wire.requestFrom(node_addr, 2);

    if (Wire.available() >= 2) {
        current_distance = (Wire.read() << 8) | Wire.read();
        return current_distance;
    }
}

bool PresenceDetectorNode::IsVehicleDetected() {
    // calculate distance diff from baseline_distance
    int offset_diff_distance = baseline_distance - ReadTOF250Distance();

    if (offset_diff_distance > DETECT_DISTANCE) return true;
    else return false;
}

int PresenceDetectorNode::GetDistance() {
    return current_distance;
}