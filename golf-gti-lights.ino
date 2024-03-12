// Inspired by KendinYap's work
// https://www.rcpano.net/2021/02/08/how-to-make-rc-car-light-system-diy-remote-control-rc-light-circuit/
// Possible thanks to Dave Madison libraries and his tutorial
// https://www.partsnotincluded.com/how-to-use-an-rc-controller-with-an-arduino/

#include <ServoInput.h>

// define pinout for lights
#define MAIN_LIGHTS 2
#define RIGHT_TURN_SIGNAL_LIGHTS 3
#define LEFT_TURN_SIGNAL_LIGHTS 4
#define BREAK_LIGHTS 5
#define REVERSING_LIGHTS 6
#define REAR_FOG_LIGHTS 7
#define HIGH_BEAM_LIGHTS 8

// define pinout for controller signals
#define THROTTLE_SIGNAL 14
#define STEERING_SIGNAL 15
#define LIGHTS_CONTROLLER_SIGNAL 16

#define STEERING_CENTER 77     // 0%
#define STEERING_LEFT_ZONE 40  // cca 40%
#define STEERING_RIGHT_ZONE 28 // cca 40%

unsigned long TIME1 = 0;
unsigned long TIME2;

bool SIGNALS_STATUS = false;

unsigned long THROTTLE;
unsigned long BRAKE;

ServoInputPin<STEERING_SIGNAL> steeringServo;

void setup()
{
  pinMode(MAIN_LIGHTS, OUTPUT);
  pinMode(RIGHT_TURN_SIGNAL_LIGHTS, OUTPUT);
  pinMode(LEFT_TURN_SIGNAL_LIGHTS, OUTPUT);
  pinMode(BREAK_LIGHTS, OUTPUT);
  pinMode(REVERSING_LIGHTS, OUTPUT);
  pinMode(REAR_FOG_LIGHTS, OUTPUT);
  pinMode(HIGH_BEAM_LIGHTS, OUTPUT);

  // set lights initial state
  digitalWrite(MAIN_LIGHTS, LOW);
  digitalWrite(RIGHT_TURN_SIGNAL_LIGHTS, LOW);
  digitalWrite(LEFT_TURN_SIGNAL_LIGHTS, LOW);
  digitalWrite(BREAK_LIGHTS, LOW);
  digitalWrite(REVERSING_LIGHTS, LOW);
  digitalWrite(REAR_FOG_LIGHTS, LOW);
  digitalWrite(HIGH_BEAM_LIGHTS, LOW);

  Serial.begin(115200);
  steeringServo.attach();

  while (steeringServo.available() == false)
  {
    Serial.println("Waiting for steering servo signal...");
    delay(500);
  }
}

void loop()
{
  float steeringAngle = steeringServo.getAngle();
  Serial.println(steeringAngle);

  // Turning Left
  if (steeringAngle > STEERING_CENTER + STEERING_LEFT_ZONE)
  {
    digitalWrite(LEFT_TURN_SIGNAL_LIGHTS, LOW);

    unsigned long TIME2 = millis();
    if (TIME2 - TIME1 >= 350)
    {
      SIGNALS_STATUS = !SIGNALS_STATUS;
      digitalWrite(RIGHT_TURN_SIGNAL_LIGHTS, SIGNALS_STATUS);
      TIME1 = millis();
    }
  }
  // Turning Right
  else if (steeringAngle < STEERING_CENTER - STEERING_RIGHT_ZONE)
  {
    digitalWrite(RIGHT_TURN_SIGNAL_LIGHTS, LOW);

    unsigned long TIME2 = millis();
    if (TIME2 - TIME1 >= 350)
    {
      SIGNALS_STATUS = !SIGNALS_STATUS;
      digitalWrite(LEFT_TURN_SIGNAL_LIGHTS, SIGNALS_STATUS);
      TIME1 = millis();
    }
  }
  // Going Straight
  else
  {
    digitalWrite(RIGHT_TURN_SIGNAL_LIGHTS, LOW);
    digitalWrite(LEFT_TURN_SIGNAL_LIGHTS, LOW);
  }
}
