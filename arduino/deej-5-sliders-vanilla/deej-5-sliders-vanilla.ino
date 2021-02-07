#include <ClickEncoder.h>
#include <TimerOne.h>

const int NUM_ENCODERS = 5;

// a lower multiplier means more granular control
const int MASTER_STEP_MULTIPLIER = 3;
const int STEP_MULTIPLIER = 5;

static const uint8_t D2 = 2;
static const uint8_t D3 = 3;
static const uint8_t D4 = 4;
static const uint8_t D5 = 5;

ClickEncoder *encoder1;
ClickEncoder *encoder2;
ClickEncoder *encoder3;
ClickEncoder *encoder4;
ClickEncoder *encoder5;

int encoderValues[NUM_ENCODERS];

void timerIsr()
{
  encoder1->service();
  encoder2->service();
  encoder3->service();
  encoder4->service();
  encoder5->service();
}

void updateEncoderValues()
{
  encoderValues[0] += encoder1->getValue() * MASTER_STEP_MULTIPLIER;
  encoderValues[1] += encoder2->getValue() * STEP_MULTIPLIER;
  encoderValues[2] += encoder3->getValue() * STEP_MULTIPLIER;
  encoderValues[3] += encoder4->getValue() * STEP_MULTIPLIER;
  encoderValues[4] += encoder5->getValue() * STEP_MULTIPLIER;

  for (int i = 0; i < NUM_ENCODERS; i++)
  {
    if (encoderValues[i] < 0)
    {
      encoderValues[i] = 0;
    }

    if (encoderValues[i] > 100)
    {
      encoderValues[i] = 100;
    }
  }
}

void setup()
{
  Serial.begin(9600);
  // ClickEncoder(DT, CLK, SW (button)); // swap DT and CLK here if encoders are backwards
  encoder1 = new ClickEncoder(A1, A0, A7); // master
  encoder2 = new ClickEncoder(A3, A2, A7);
  encoder3 = new ClickEncoder(A5, A4, A7);

  encoder4 = new ClickEncoder(D3, D2, A7);
  encoder5 = new ClickEncoder(D5, D4, A7);

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);

  encoder1->setAccelerationEnabled(true);
  encoder2->setAccelerationEnabled(true);
  encoder3->setAccelerationEnabled(true);
  encoder4->setAccelerationEnabled(true);
  encoder5->setAccelerationEnabled(true);

  // the sliders actually control their volume as a proportion of the master
  // volume, i.e spotify.exe at 100% will be 25% on the Windows volume
  // mixer if the master volume is set to 25%

  // so, with the assumption that master is always in position 0:
  // set all other sliders to be "full volume" i.e. 100% of master volume
  // then set master to 0
  encoderValues[0] = 0;

  for (int i = 1; i < NUM_ENCODERS; i++)
  {
    READ THE FOLLOWING THEN REMOVE THIS LINE TO COMPILE
    // uncomment this line to set all other sliders to 100% of master
    // NOTE: if slider 0 is not set as master this could potentially blow your speakers
    // and/or eardrums
    // encoderValues[i] = 100;
  }

  // sanity check
  if (STEP_MULTIPLIER < 1 || STEP_MULTIPLIER > 10 || MASTER_STEP_MULTIPLIER < 1 || MASTER_STEP_MULTIPLIER > 10)
  {
    STEP_MULTIPLIER = 1;
    MASTER_STEP_MULTIPLIER = 1;
  }
}

void loop()
{
  updateEncoderValues();

  String builtString = String("");

  for (int i = 0; i < NUM_ENCODERS; i++)
  {
    builtString += String(map(encoderValues[i], 0, 100, 0, 1023)); // convert to analog equivalent

    if (i < NUM_ENCODERS - 1)
    {
      builtString += String("|");
    }
  }

  Serial.println(builtString);
  delay(10);
}
