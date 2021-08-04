#include <Adafruit_ADS1X15.h>
#define AVERAGE_WINDOW_SIZE (10)

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;/* Use this for the 12-bit version */

//float volts0, volts1, volts2, volts3; //might be used later

float maCollection [AVERAGE_WINDOW_SIZE] = {};

const float mySamples[21] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
const float myMA[21] = {1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6.5, 7.5, 8.5, 9.5, 10.5, 11.5, 12.5, 13.5, 14.5, 15.5, 16.5};

void setup(void)
{
  Serial.begin(9600);
  //while(!Serial);// this allows the bellow code to run
  Serial.println("Hello, I am Ready!");

  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
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
  delay(1000);

  //batteryScaleFactor();

    findSlope(4,2,6,8);
  
  //MovingAverage();
  //for (int i = 0; i < 21; i++)
  //{
   // delay(1000);
   // testMA(mySamples[i], myMA[i]);
  //}
  delay(1000);
}

/*void testMA(float sample, float ma) //tests the moving average
{
  static float sampledMA = 0;
  sampledMA = MovingAverage();

  Serial.print("Testing sample[");
  Serial.print(sample);
  Serial.print("]: ");

  Serial.print("sampled MA[");
  Serial.print(sampledMA);
  Serial.print("]: ");

  Serial.print("Testing MA[");
  Serial.print(ma);
  Serial.print("]: ");

  if (abs(sampledMA - ma) <= .01)
  {
    Serial.println("OK");
    Serial.println();
  }
  else
  {
    Serial.println("ERROR");
    Serial.println();
  }

  Serial.println("---------------------------------------------------------");
}*/

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
  static int AvgRateState = 0;
  static float starttime = 0;

  if (AvgRateState = 1)
  {
    starttime = StartTime();

    //Serial.print("Start: ");
    //Serial.print(starttime);
   // Serial.print(" us");
    //Serial.println();
  }

  AvgRateState++;

  if (AvgRateState = AVG_RATE_TEST_SIZE)
  {
    static float rateResult = 0;
    float finishtime = FinishTime();
    //Serial.print("end:   ");
   // Serial.print(finishtime);
   // Serial.print(" us");
    //Serial.println();

    rateResult = (finishtime - starttime) / AVG_RATE_TEST_SIZE;

    //Serial.print("average for 1k tests: ");
    //Serial.print(rateResult, 4); //", 4" prints the amount of decimals I want
    //Serial.print(" micro seconds");
    //Serial.println();
    //Serial.println();
  }
}

float grabVoltValue()
{
  int16_t adc0, adc1, adc2, adc3;
  float volts0, volts1, volts2, volts3;
  static int i = -1;
  i++;
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);

  volts0 = ads.computeVolts(adc0);
  volts1 = ads.computeVolts(adc1);
  volts2 = ads.computeVolts(adc2);
  volts3 = ads.computeVolts(adc3);

  //volts0 = mySamples[i];

  return volts0;
}

void fillMovingArray(float sample)//array imp of q
{
  static int stateFill = 0;
  static int currentIndexQA = 0;

  if (stateFill < AVERAGE_WINDOW_SIZE)
  {
    //Serial.print("sample: ");
    //Serial.println(sample);

    //Serial.print("index: ");
    //Serial.println(currentIndexQA);

    maCollection[currentIndexQA] = sample;

    currentIndexQA++;

    stateFill++;

    return;
  }

  //Serial.print("state: ");
  //Serial.println(stateFill);

  //Serial.print("index: ");
  //Serial.println(currentIndexQA);

  for (int i = 0; i < AVERAGE_WINDOW_SIZE; i++)
  {
      maCollection[i] = maCollection[i + 1]; // array is not out of bounds??
  }   
                                        
  //Serial.print("sample: ");
  //Serial.println(sample);

  //Serial.print("index: ");
  //Serial.println(currentIndexQA);

  maCollection[currentIndexQA] = sample;

  return;
}

float MovingAverage()
{
  static int First_Fill_Index = 1;
  //static int callcount = 0;
  static float sumMA = 0;
  static float Ma_Grab_Volt_Value = 0;
  static float movingAverage = 0;

  Ma_Grab_Volt_Value = grabVoltValue();

  //Serial.print("Volt: ");
  //Serial.println(Ma_Grab_Volt_Value);

  fillMovingArray(Ma_Grab_Volt_Value);

  for (int i = 0; i < AVERAGE_WINDOW_SIZE; i++)
  {
    //Serial.print("[");
    //Serial.print(i);
    //Serial.print("]: ");
    //Serial.println(maCollection[i]);
  }

  //Serial.println();

  if (First_Fill_Index < AVERAGE_WINDOW_SIZE)
  {
    for (int i = 0; i < First_Fill_Index; i++)
    {
      sumMA += maCollection[i];
    }

    //Serial.print("sumMA: ");
    //Serial.println(sumMA);

    movingAverage = sumMA / float(First_Fill_Index);

    sumMA = 0;

    First_Fill_Index++;

    //Serial.print("ma: ");
    //Serial.println(movingAverage);
    //Serial.println();

    return movingAverage;
  }

  for (int i = 0; i < First_Fill_Index; i++)
  {
    sumMA += maCollection[i];
  }

  //Serial.print("sumMA: ");
  //Serial.println(sumMA);

  movingAverage = sumMA / AVERAGE_WINDOW_SIZE;
  sumMA = 0;

  //Serial.print("ma: ");
  //Serial.print(movingAverage);
  //Serial.println(" V");
  //Serial.println();

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

float findSlope(float x1, float y1, float x2, float y2)
{
  float chngInX = 0;
  float chngInY = 0;
  float slope = 0;
  
  chngInY = y2-y1;
  chngInX = x2-x1;

  slope = abs(chngInY/chngInX);

  Serial.println(slope);
 
  return slope;
}

float batteryScaleFactor() //need to set Serial Monitor to "no line ending"
{
  float scaled_Output = 0;
  float battery_Volt_Val = 0;
  int state_of_Scale_Factor = 0;
  float tempX1 = 0;
  float tempY1 = 0; 

  if (state_of_Scale_Factor == 0)
  {
    Serial.println("Enter a float for the first battery power level.");

    battery_Volt_Val = grabFloatValue();

    state_of_Scale_Factor ++;

    scaled_Output = MovingAverage();
    
    tempX1 = battery_Volt_Val;
    tempY1 = scaled_Output;
  }

  if (state_of_Scale_Factor == 1)
  {
    Serial.println("Enter a float for the second battery power level.");

    battery_Volt_Val = grabFloatValue();

    scaled_Output = MovingAverage();

    state_of_Scale_Factor ++;
    
    findSlope(tempX1, tempY1, battery_Volt_Val, scaled_Output);
  }
}
