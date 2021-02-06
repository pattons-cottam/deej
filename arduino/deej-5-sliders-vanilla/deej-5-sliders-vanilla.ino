#include <ClickEncoder.h>
#include <TimerOne.h>

const int NUM_ENCODERS = 2;

ClickEncoder *encoder1;
ClickEncoder *encoder2;

int encoderValues[NUM_ENCODERS];

void timerIsr()
{
  encoder1->service();
  encoder2->service();
}

void updateEncoderValues()
{
  encoderValues[0] += encoder1->getValue();
  encoderValues[1] += encoder2->getValue();

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
  encoder1 = new ClickEncoder(A1, A0, A2); // CLK, DT, SW (button)
  encoder2 = new ClickEncoder(A4, A3, A2); // CLK, DT, SW (button)

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);

  encoder1->setAccelerationEnabled(true);
  encoder2->setAccelerationEnabled(true);

  delay(10);
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
}
