#include <Adafruit_ADS1X15.h>
#include "Queue.h"
#define AVERAGE_WINDOW_SIZE (10)

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;/* Use this for the 12-bit version */

Queue<float> Mov_AvgQ = Queue<float>(AVERAGE_WINDOW_SIZE);
static int stateFill = 0; 
bool stateFilled = false;

static int ma_Fill_Index = 0;


void setup(void)
{
  Serial.begin(9600);
  //while(!Serial);// this allows the bellow code to run

  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)//set
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
}

void loop(void)//this is the main???
{
  static int init_Wait = 0;

  if (init_Wait == 0)
  {
    delay(4000);
    Serial.println("Enter 1 for finding the scale factor ect...");
    init_Wait++;
  }

  switch (Serial.parseInt())
  {
    case 1: batteryScaleFactor(); init_Wait = 0; break;

    default: break;
  }

  MovingAverage();

}

float StartTime()
{
  float starttime = micros();
  return starttime;
}

float FinishTime()
{
  float finishtime = micros();
  return finishtime;
}

float AverageRate(void)
{
  const int AVG_RATE_TEST_SIZE = 1000;
  int AvgRateState = 0;
  float starttime = 0;
  float rateResult = 0;
  static float periodResult = 0;
  float finishTime = 0;

  if (AvgRateState = 1)
  {
    starttime = StartTime();

    Serial.print("Start: ");
    Serial.print(starttime);
    Serial.print(" us");
    Serial.println();
  }

  AvgRateState++;

  if (AvgRateState = AVG_RATE_TEST_SIZE)
  {
    finishTime = FinishTime();

    Serial.print("end:   ");
    Serial.print(finishTime);
    Serial.print(" us"); // in micro so lets fix that
    Serial.println();

    periodResult = (finishTime - starttime) / AVG_RATE_TEST_SIZE;

    Serial.print("Period: ");
    Serial.print(periodResult);
    Serial.print(" us");
    Serial.println();

    rateResult = 1 / (periodResult * 1e-6);

    Serial.print("Rate: ");
    Serial.print(rateResult);
    Serial.print(" Hz");
    Serial.println();
    Serial.println();

    return rateResult;
  }
}

float grabAdsValue()
{
  int16_t adsValue0_1, adsValue2_3;
  float volts0, volts1, volts2, volts3;

  adsValue0_1 = ads.readADC_Differential_0_1();
  adsValue2_3 = ads.readADC_Differential_2_3();

  volts0 = ads.computeVolts(adsValue0_1);

  return volts0;
}

void fillQueue(float sample)// queue
{

  if (stateFill < AVERAGE_WINDOW_SIZE)
  {
    Mov_AvgQ.push(sample);
    stateFill++;
  }

  if (stateFill == AVERAGE_WINDOW_SIZE && stateFilled == true)
  {
    Mov_AvgQ.pop();
    Mov_AvgQ.push(sample);
  }

  if (stateFill == AVERAGE_WINDOW_SIZE)
  {
    stateFilled = true;
  }

  return;
}

float MovingAverage()
{
  static float sumMA = 0;
  static float Ma_Grab_Volt_Value = 0;
  static float movingAverage = 0;
  float temp = 0;
  Queue<float> tempMA = Queue<float>(AVERAGE_WINDOW_SIZE);

  Ma_Grab_Volt_Value = grabAdsValue();

  Serial.print("sample: ");
  Serial.print(Ma_Grab_Volt_Value);
  Serial.print("  ");

  fillQueue(Ma_Grab_Volt_Value);

  while (Mov_AvgQ.count() != 0) //.count != 0
  {
    if(Mov_AvgQ.count() != 0)
      {
      Serial.print("[");
      Serial.print(Mov_AvgQ.count()-1);
      Serial.print("]: ");
      Serial.print(Mov_AvgQ.peek());
      Serial.print("  ");
      }
    temp = Mov_AvgQ.peek();
    tempMA.push(temp);
    Mov_AvgQ.pop();
    sumMA += temp;
  }

  while (tempMA.count() != 0)
  {
    Mov_AvgQ.push(tempMA.peek());
    tempMA.pop();
  }

  if (ma_Fill_Index < AVERAGE_WINDOW_SIZE)
  {
    ma_Fill_Index++;
  }

  movingAverage = sumMA / ma_Fill_Index;

  Serial.print("ma: ");
  Serial.print(movingAverage);
  Serial.print(" V  ");
  Serial.println();
  Serial.println();


  sumMA = 0;
  return movingAverage;
}

float grabFloatValue()
{
  while (Serial.available() == 0);

  float grabFloat = Serial.parseFloat();

  return grabFloat;
}

int grabIntValue()
{
  while (Serial.available() == 0);

  int grabInt = Serial.parseInt();

  return grabInt;
}

/*float findSlope(float x1, float y1, float x2, float y2)
{
  

  return slope;
}*/

float batteryScaleFactor() //need to set Serial Monitor to "no line ending"
{
  float scaled_Output = 0;
  float battery_Volt_Val = 0;
  int state_of_Scale_Factor = 0;
  float tempX1 = 0;
  float tempY1 = 0;
  float scaleFactor = 0;

  if (state_of_Scale_Factor == 0) //therefore i can re warite this to find a slope y = mx + b // regression method type thing maybe.
  {
    Serial.println("Enter a float for the first battery power level.");

    battery_Volt_Val = grabFloatValue();

    Serial.print("first Voltage: ");
    Serial.print(battery_Volt_Val);
    Serial.print("  ");

    state_of_Scale_Factor ++;

    scaled_Output = MovingAverage();

    tempX1 = battery_Volt_Val;
    tempY1 = scaled_Output;
  }

  if (state_of_Scale_Factor == 1)
  {
    Serial.println("Enter a float for the second battery power level.");

    battery_Volt_Val = grabFloatValue();

    Serial.print("second Voltage: ");
    Serial.print(battery_Volt_Val);
    Serial.print("  ");

    //memset(Mov_AvgQ, 0, 40);

    clearQueue();

    for (int i = 0; i < AVERAGE_WINDOW_SIZE; i++)
    {
      MovingAverage();
    }

    scaled_Output = MovingAverage();

    state_of_Scale_Factor ++;

    //scaleFactor = findSlope(tempX1, tempY1, battery_Volt_Val, scaled_Output);
  }

  return scaleFactor;
}

void clearQueue()
{
  stateFill = 0;
  stateFilled = false;
  ma_Fill_Index = 0;
  
  while (Mov_AvgQ.count() != 0)
  {
    Mov_AvgQ.pop();
  }
}
