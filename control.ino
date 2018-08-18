#include <PID_v1.h>

class Controller {

  public:
    Controller(MAX6675* thermocouple, double* input, double* output, double* setpoint, double* alarm, int Kp, int Ki, int Kd)
      : PID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT) {
      myAlarm = alarm;
    };

    void MeasureTemp() {
      myInput = thermocouple.readCelsius();
    };

    void ComputePID() {
      if (myInput < myAlarm) {
        Compute();
      } else {
        SetMode(MANUAL);
        myOutput = 0;
      }
    };
    
    void ExecutePID(int controlPin, unsigned long* windowStart, unsigned long windowSize) {
    
      if (millis() - windowStart > windowSize) {
        //time to shift the Relay Window
        windowStart += windowSize;
      }
    
      if (myOutput > millis() - windowStart) {
        digitalWrite(controlPin, HIGH);
      } else {
        digitalWrite(controlPin, LOW);
      }
    };

    void TogglePID() {
      mode = PID::GetMode();
      if (mode == MANUAL) {
        PID::SetMode(AUTOMATIC);
      } else if (mode == AUTOMATIC) {
        PID::SetMode(MANUAL);
        myOutput = 0;
      }
    };
};
