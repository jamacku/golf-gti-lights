// Inspired by KendinYap's work
// https://www.rcpano.net/2021/02/08/how-to-make-rc-car-light-system-diy-remote-control-rc-light-circuit/
// Possible thanks to Dave Madison libraries and his tutorial
// https://www.partsnotincluded.com/how-to-use-an-rc-controller-with-an-arduino/

#include <ServoInput.h>

// define pin-out for lights
#define MAIN_LIGHTS 2
#define RIGHT_TURN_SIGNAL_LIGHTS 3
#define LEFT_TURN_SIGNAL_LIGHTS 4
#define BREAK_LIGHTS 5
#define REVERSING_LIGHTS 6
#define REAR_FOG_LIGHTS 7
#define HIGH_BEAM_LIGHTS 8

// define pin-out for controller signals
#define THROTTLE_SIGNAL 14
#define STEERING_SIGNAL 15
#define SWITCH_SIGNAL 16

// define throttle zones
#define THROTTLE_DEAD_ZONE 15 // cca 15%

// define steering zones
#define STEERING_CENTER 77          // 0%
#define STEERING_LEFT_DEAD_ZONE 40  // cca 40%
#define STEERING_RIGHT_DEAD_ZONE 28 // cca 40%

// define switch positions
#define NUMBER_OF_SWITCH_POSITIONS 4
#define SWITCH_POSITION_A 1
#define SWITCH_POSITION_NEUTRAL 2
#define SWITCH_POSITION_B 3

bool SWITCH_STATUS_A = false;
bool SWITCH_STATUS_B = false;

unsigned long TIME1 = 0;
unsigned long TIME2;

bool SIGNALS_STATUS = false;

// initialize RC controller signals
ServoInputPin<THROTTLE_SIGNAL> throttleSignal;
ServoInputPin<STEERING_SIGNAL> steeringServo;
ServoInputPin<SWITCH_SIGNAL> switchSignal;

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

  // !FIXME: remove Serial.begin(115200) when all is working
  Serial.begin(115200);
  throttleSignal.attach();
  steeringServo.attach();
  switchSignal.attach();

  while (!ServoInput.allAvailable())
  {
    Serial.println("Waiting for RC controller signals...");
    delay(500);
  }
}

void loop()
{
  float steeringAngle = steeringServo.getAngle();

  // Turning Left
  if (steeringAngle > STEERING_CENTER + STEERING_LEFT_DEAD_ZONE)
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
  else if (steeringAngle < STEERING_CENTER - STEERING_RIGHT_DEAD_ZONE)
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

  int speed = throttleSignal.mapDeadzone(-100, 100, (THROTTLE_DEAD_ZONE * 0.01));
  if (speed == 0)
  {
    // Serial.println("In throttle deadzone!");
  }
  if (speed > 0)
  {
    // Serial.print("Throttle: ");
    // Serial.print(speed);
    // Serial.println("%");
  }
  else
  {
    // Serial.print("Brake: ");
    // Serial.print(abs(speed));
    // Serial.println("%");
  }

  int position = switchSignal.map(1, NUMBER_OF_SWITCH_POSITIONS);
  Serial.println(position);
  switch (position)
  {
  // Fog lights control
  case SWITCH_POSITION_A:
    if (!SWITCH_STATUS_A)
    {
      SWITCH_STATUS_A = true;
      digitalWrite(REAR_FOG_LIGHTS, HIGH);
    }
    else
    {
      SWITCH_STATUS_A = false;
      digitalWrite(REAR_FOG_LIGHTS, LOW);
    }
    break;

  case SWITCH_POSITION_B:
    if (!SWITCH_STATUS_B)
    {
      SWITCH_STATUS_B = true;
      digitalWrite(HIGH_BEAM_LIGHTS, HIGH);
    }
    else
    {
      SWITCH_STATUS_B = false;
      digitalWrite(HIGH_BEAM_LIGHTS, LOW);
    }
    break;

  case SWITCH_POSITION_NEUTRAL:
  default:
    break;
  }

  delay(500);
}
