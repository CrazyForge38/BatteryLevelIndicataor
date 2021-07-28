#include <Adafruit_ADS1X15.h>
#define elementCount(x) (sizeof(x) / sizeof(x[0])
#define AVERAGE_WINDOW_SIZE (10)

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;/* Use this for the 12-bit version */


//float volts0, volts1, volts2, volts3;

const int AVG_RATE_TEST_SIZE = 1000;// do we want this to be adjustable and keep as a gloabal










float maCollection [AVERAGE_WINDOW_SIZE] = {};

//////////////////////////////// testing

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
  batteryScaleFactor();
  delay(1000);
}

/*void testMA(float sample, float ma) {
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
  static int AvgRateState = 0;
  static float starttime = 0;

  if (AvgRateState = 1)
  {
    starttime = StartTime();


    Serial.print("Start: ");
    Serial.print(starttime);
    Serial.println();
  }

  AvgRateState++;

  if (AvgRateState = AVG_RATE_TEST_SIZE)
  {
    static float rateResult = 0;
    float finishtime = FinishTime();
    Serial.print("end:   ");
    Serial.print(finishtime);
    Serial.println();

    rateResult = (finishtime - starttime) / AVG_RATE_TEST_SIZE;

    Serial.print("average for 1k tests: ");
    Serial.print(rateResult, 4); //", 4" prints the amount of decimals I want
    Serial.print(" micro seconds");
    Serial.println();
    Serial.println();
  }

}

float grabAdcValue()
{
  int16_t adc0, adc1, adc2, adc3;
  adc0 = ads.readADC_SingleEnded(0); //used for testing wanted values
  return adc0;
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
    maCollection[i] = maCollection[i + 1];
  }

  if (currentIndexQA == AVERAGE_WINDOW_SIZE )
  {
    currentIndexQA = AVERAGE_WINDOW_SIZE;
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
    static float Ma_Grab_Adc_Value = 0;
    static float movingAverage = 0;

    Ma_Grab_Adc_Value = grabAdcValue();

    //Serial.print("adc: ");
    //Serial.println(Ma_Grab_Adc_Value);

    fillMovingArray(Ma_Grab_Adc_Value); //I could just use the grabadcvalue in here 

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

float batteryScaleFactor()
{
  float scaleFactor = 0;
  
  Serial.println("Enter a float for the battery power level.");
  
  while(Serial.available() == 0);
  
  scaleFactor = Serial.parseFloat() / MovingAverage();

  Serial.println(scaleFactor);
}
