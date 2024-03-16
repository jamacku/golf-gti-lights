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

/* -------------------------------- */
/* --- RC 1st chanel - steering --- */
/* -------------------------------- */

// define steering zones
#define STEERING_CENTER 77          // 0%
#define STEERING_LEFT_DEAD_ZONE 40  // cca 40%
#define STEERING_RIGHT_DEAD_ZONE 28 // cca 40%

unsigned long TURN_SIGNAL_TIME1 = 0;
unsigned long TURN_SIGNAL_TIME2;

bool SIGNALS_STATUS = false;

/* -------------------------------- */
/* --- RC 2nd chanel - throttle --- */
/* -------------------------------- */

// define throttle zones
#define THROTTLE_DEAD_ZONE 25 // cca 15%

// define throttle zones
#define HIGH_BEAM_THROTTLE 25 // cca 25%

enum ThrottleStatus
{
  ACCELERATING,
  IDLE,
  BREAKING,
  REVERSING,
};
#define SIZE_OF_THROTTLE_STATUS 10
ThrottleStatus throttleStatus[SIZE_OF_THROTTLE_STATUS] = {IDLE, IDLE, IDLE, IDLE, IDLE, IDLE, IDLE, IDLE, IDLE, IDLE};

/* ------------------------------ */
/* --- RC 3rd chanel - switch --- */
/* ------------------------------ */

enum SwitchPosition
{
  POSITION_A = 1,
  POSITION_NEUTRAL,
  POSITION_B,
  NUMBER_OF_SWITCH_POSITIONS
};

bool SWITCH_STATUS_A = false;
bool SWITCH_STATUS_B = false;

// initialize RC controller signals
ServoInputPin<THROTTLE_SIGNAL> throttleSignal;
ServoInputPin<STEERING_SIGNAL> steeringServo;
ServoInputPin<SWITCH_SIGNAL> switchSignal;

void pushThrottleStatus(ThrottleStatus value)
{
  // shift all elements to the left
  for (int i = 0; i < SIZE_OF_THROTTLE_STATUS - 1; i++)
  {
    throttleStatus[i] = throttleStatus[i + 1];
  }

  // add new value to the end
  throttleStatus[SIZE_OF_THROTTLE_STATUS - 1] = value;
}

ThrottleStatus getPreviousThrottleStatus()
{
  ThrottleStatus maxOccurrence;
  int i, j, count;
  int maxCount = 0;

  for (i = 0; i < SIZE_OF_THROTTLE_STATUS; i++)
  {
    count = 1;
    for (j = i + 1; j < SIZE_OF_THROTTLE_STATUS; j++)
    {
      if (throttleStatus[j] == throttleStatus[i])
      {
        count++;
        if (count > maxCount)
        {
          maxOccurrence = throttleStatus[j];
        }
      }
    }
  }

  return maxOccurrence;
}

/* -------------------------- */
/* --- SETUP & INITIALIZE --- */
/* -------------------------- */

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

/* ----------------- */
/* --- MAIN LOOP --- */
/* ----------------- */

void loop()
{
  digitalWrite(MAIN_LIGHTS, HIGH);

  float steeringAngle = steeringServo.getAngle();

  // Turning Left
  if (steeringAngle > STEERING_CENTER + STEERING_LEFT_DEAD_ZONE)
  {
    digitalWrite(LEFT_TURN_SIGNAL_LIGHTS, LOW);

    unsigned long TURN_SIGNAL_TIME2 = millis();
    if (TURN_SIGNAL_TIME2 - TURN_SIGNAL_TIME1 >= 350)
    {
      SIGNALS_STATUS = !SIGNALS_STATUS;
      digitalWrite(RIGHT_TURN_SIGNAL_LIGHTS, SIGNALS_STATUS);
      TURN_SIGNAL_TIME1 = millis();
    }
  }
  // Turning Right
  else if (steeringAngle < STEERING_CENTER - STEERING_RIGHT_DEAD_ZONE)
  {
    digitalWrite(RIGHT_TURN_SIGNAL_LIGHTS, LOW);

    unsigned long TURN_SIGNAL_TIME2 = millis();
    if (TURN_SIGNAL_TIME2 - TURN_SIGNAL_TIME1 >= 350)
    {
      SIGNALS_STATUS = !SIGNALS_STATUS;
      digitalWrite(LEFT_TURN_SIGNAL_LIGHTS, SIGNALS_STATUS);
      TURN_SIGNAL_TIME1 = millis();
    }
  }
  // Going Straight
  else
  {
    digitalWrite(RIGHT_TURN_SIGNAL_LIGHTS, LOW);
    digitalWrite(LEFT_TURN_SIGNAL_LIGHTS, LOW);
  }

  int position = switchSignal.map(1, NUMBER_OF_SWITCH_POSITIONS);
  switch (position)
  {
  // Fog lights control
  case POSITION_A:
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

  case POSITION_B:
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

  case POSITION_NEUTRAL:
  default:
    break;
  }

  int speed = throttleSignal.mapDeadzone(-100, 100, (THROTTLE_DEAD_ZONE * 0.01));
  if (speed == 0)
  {
    pushThrottleStatus(IDLE);
    digitalWrite(HIGH_BEAM_LIGHTS, LOW);
    digitalWrite(BREAK_LIGHTS, LOW);
    digitalWrite(REVERSING_LIGHTS, LOW);
  }
  else if (speed > 0)
  {
    pushThrottleStatus(ACCELERATING);
    if (speed >= HIGH_BEAM_THROTTLE)
    {
      digitalWrite(HIGH_BEAM_LIGHTS, HIGH);
    }
    else
    {
      if (!SWITCH_STATUS_B)
      {
        digitalWrite(HIGH_BEAM_LIGHTS, LOW);
      }
    }
  }
  // check for speed less then -1 to eliminate ocasional noise
  else if (speed < -1)
  {
    const int previousThrottleStatus = getPreviousThrottleStatus();
    if (previousThrottleStatus == ACCELERATING || previousThrottleStatus == BREAKING)
    {
      pushThrottleStatus(BREAKING);
      digitalWrite(BREAK_LIGHTS, HIGH);
    }
    if (previousThrottleStatus == IDLE || previousThrottleStatus == REVERSING)
    {
      pushThrottleStatus(REVERSING);
      digitalWrite(REVERSING_LIGHTS, HIGH);
    }
  }
  else
  {
    digitalWrite(HIGH_BEAM_LIGHTS, LOW);
    digitalWrite(BREAK_LIGHTS, LOW);
    digitalWrite(REVERSING_LIGHTS, LOW);
  }

  delay(200);
}
