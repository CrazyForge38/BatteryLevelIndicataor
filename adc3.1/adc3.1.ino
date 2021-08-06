#include <Adafruit_ADS1X15.h>
#include "Queue.h"
#define AVERAGE_WINDOW_SIZE (10)

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;/* Use this for the 12-bit version */

//float volts0, volts1, volts2, volts3; //might be used later

Queue<float> Mov_AvgQ = Queue<float>(AVERAGE_WINDOW_SIZE);
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
  delay(4000);

  //batteryScaleFactor();

  //MovingAverage();
  for (int i = 0; i < 21; i++)
  {
   delay(1000);
   testMA(mySamples[i], myMA[i]);
  }
  delay(1000);
}

void testMA(float sample, float ma) //tests the moving average
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
}


float grabVoltValue()
{
  int16_t adc0, adc1, adc2, adc3;
  float volts0, volts1, volts2, volts3;
  static int i = -1;
  i++;
  //adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);
  //Serial.println("testing grab");
  //volts0 = ads.computeVolts(adc0);
  volts1 = ads.computeVolts(adc1);
  volts2 = ads.computeVolts(adc2);
  volts3 = ads.computeVolts(adc3);

  volts0 = mySamples[i];
  //return adc0;
  return volts0;
}

void fillQueue(float sample)// queue
{
  static int stateFill = 0;
  static bool stateFilled = false; 
  
  Serial.println("tesing the state");
  if (stateFill < AVERAGE_WINDOW_SIZE)
  {
    Serial.print("tesing the sample");
    Serial.println(sample);
    Mov_AvgQ.push(sample);
    stateFill++;
  }

  if (stateFill == AVERAGE_WINDOW_SIZE && stateFilled == true)
  {
    Serial.println("tesing the ==");
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
  static int fill_Index = 0;
  static float sumMA = 0;
  static float Ma_Grab_Volt_Value = 0;
  static float movingAverage = 0; 
  float temp = 0;
  Queue<float> tempMA = Queue<float>(AVERAGE_WINDOW_SIZE);
 
  Ma_Grab_Volt_Value = grabVoltValue();

  Serial.print("Volt: ");
  Serial.println(Ma_Grab_Volt_Value);

  fillQueue(Ma_Grab_Volt_Value);

  while (Mov_AvgQ.count() != 0) //.count != 0
  {
    if(Mov_AvgQ.count() != 0)
    {
      Serial.print("[");
      Serial.print(Mov_AvgQ.count()-1);
      Serial.print("]: ");
      Serial.println(Mov_AvgQ.peek());
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

  if (fill_Index < AVERAGE_WINDOW_SIZE)
  {
    fill_Index++;
  }

  movingAverage = sumMA / fill_Index;

  Serial.print("sumMA: ");
  Serial.println(sumMA);

  Serial.print("ma: ");
  Serial.print(movingAverage);
  Serial.println(" V");
  Serial.println();

  sumMA = 0;
  return movingAverage;
}
