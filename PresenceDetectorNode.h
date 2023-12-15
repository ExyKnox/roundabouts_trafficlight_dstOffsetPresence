#include "arduino.h"
#include <Wire.h>

#ifndef DEBUG
    #define DEBUG true
#endif

#define DETECT_DISTANCE 1

class PresenceDetectorNode {
    // 이 객체는 :
    // ToF 최대 거리값을 가지고 있고 (initialize time 필요)
    // 범위 내에 물체(차량) 감지되었는지 여부를 넘겨주는 메소드가 있음
    
    private:
        int current_distance;
        int offset_distance;
        int node_addr;

        void CheckRegisterAddress();
        int ReadTOF250Distance();

    public:
        PresenceDetectorNode();
        PresenceDetectorNode(int address);
        bool IsVehicleDetected();
        int GetDistance();
};