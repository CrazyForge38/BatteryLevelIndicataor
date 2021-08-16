#include <Queue.h>

//#define DEBUG
#define AVERAGE_WINDOW_SIZE (10)
#define NUM_SCALE_SAMPLES (4)
#include <Adafruit_ADS1X15.h>
#include "Queue.h"

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//5 decimals for current
Queue<float> Mov_Avg_VoltQ = Queue<float>(AVERAGE_WINDOW_SIZE);
//Queue<float> Mov_Avg_VoltQ = Queue<float>(AVERAGE_WINDOW_SIZE);

static int stateFill = 0; //used for reseting "pushQueue"
bool stateFilled = false;

void setup(void)
{
  Serial.begin(9600);

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

  if (!ads.begin())
  {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
}

void loop(void)
{
  static unsigned int state = 0;
  static unsigned int mode = 0;
  static unsigned int subState1 = 0; 
  static unsigned int subState2 = 0;
  static unsigned int avgCount = 0;
  static int counted_Samples = 0;
  float Ma_Grab_Volt_Value = 0;
  float scaled_Output = 0;
  float battery_Volt_Val = 0;
  int state_of_Scale_Factor = 0;
  float avg_Scale_Factor = 0;
  float sum_Scale_Fator = 0;
  static unsigned int countedSamples = 0;
 // if(mode == 0)
 // {
    switch (state)// inital start and second phase
    {
      case 0:  
        delay(2000);
        Serial.println("Enter 1 for Voltage, 2 for current, and 3 to restart");
        int userSelection = grabIntValue();
        switch (userSelection)
        { 
          case 1:
            mode = 1;
            //battery_Avg_Scale_Factor(1);
            //init_Wait = 0;
            break;
      
          case 2: 
            mode = 2;
            //battery_Avg_Scale_Factor(2);
            //init_Wait = 0;
            break;
      
          case 3: 
            state = 0;
            subState1 = 0;
            subState2 = 0;
            //mode = 0;
            break;
            
          default:
            break;
        }
        state = 1;
        subState1 = 0;
        counted_Samples = 0;
        break;
  
      case 1: 
        switch (subState1)
        {
          case 0:
            switch (subState2)
            {          
              case 0: 
                Serial.println("Enter a inital battery power level.");
                 
                battery_Volt_Val = grabFloatValue();
                  
                Serial.print("applied Voltage: ");
                Serial.print(battery_Volt_Val);
                Serial.print("  ");
                Serial.println();
                  
                clearQueue();
                
                //subState1 = 1;
                subState2 = 1;
                avgCount = 0;
                break;

              case 1:
                if (avgCount < AVERAGE_WINDOW_SIZE)
                {
                 Ma_Grab_Volt_Value = grabAdsValue(mode);
                 
                 MovingAverage(Ma_Grab_Volt_Value);
                 avgCount++;
                }
                else
                {
                  //subState1 = 0;
                  subState2 = 2;
                  
                  scaled_Output = MovingAverage(Ma_Grab_Volt_Value);
   
                  sum_Scale_Fator += slopeIntercept(battery_Volt_Val, scaled_Output);

                  countedSamples++;
                } 
                break; 

              case 2:
                if(countedSamples == 4)
                {
                  state = 2;
                  
                }
                subState2 = 0;
                countedSamples = 0;
                break;
              
              default: 
                break;
            }
           break;
        }
        
        case 2:
          avg_Scale_Factor = sum_Scale_Fator / NUM_SCALE_SAMPLES;
  
          Serial.print("The average slope of the 4 is : ");
          Serial.print(avg_Scale_Factor);
          Serial.println();

          state = 0;
          subState1 = 0;
          subState2 = 0;
          sum_Scale_Fator = 0;
          //mode = 0;
          break;
            
        default: 
          break;
    } 
    
 // }
 // else //do we need the else
 // {
    Ma_Grab_Volt_Value = grabAdsValue(mode);
    
    MovingAverage(Ma_Grab_Volt_Value);  
 // }

//have this print out the volt/current too

//  switch ()
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

  if (AvgRateState = AVG_RATE_TEST_SIZE)//rate test size was definded
  {
    finishTime = FinishTime();

    Serial.print("end:   ");
    Serial.print(finishTime);
    Serial.print(" us"); 
    Serial.println();

    periodResult = (finishTime - starttime) / AVG_RATE_TEST_SIZE;

    Serial.print("Period: ");
    Serial.print(periodResult);
    Serial.print(" us");
    Serial.println();

    rateResult = 1 / (periodResult * 1e-6);//account for the unit, micro seconds

    Serial.print("Rate: ");
    Serial.print(rateResult);
    Serial.print(" Hz");
    Serial.println();
    Serial.println();

    return rateResult;
  }
}

float grabAdsValue(unsigned int input)
{
  int16_t adsValue;
  float volts0, volts1, volts2, volts3;

  switch (input)
  {
      case(1): 
        adsValue = ads.readADC_Differential_0_1();
        break;
        
      case(2): 
        adsValue = ads.readADC_Differential_2_3();
        break;

      default: 
        adsValue = 0;
  }

  volts0 = ads.computeVolts(adsValue); //converts ads to voltage
  
  return volts0;
}

void pushQueue(float sample)
{

  if (stateFill < AVERAGE_WINDOW_SIZE)//fills the queue one at a time
  {
    Mov_Avg_VoltQ.push(sample);
    stateFill++;
  }

  if (stateFill == AVERAGE_WINDOW_SIZE && stateFilled == true)//stateFilled lets the queue know that it is filled and can be popped 
  {
    Mov_Avg_VoltQ.pop();
    Mov_Avg_VoltQ.push(sample);
  }

  if (stateFill == AVERAGE_WINDOW_SIZE)
  {
    stateFilled = true;
  }

  return;
}

float MovingAverage(float Ma_Grab_Volt_Value)
{
  static float sumMA = 0;
  //static float Ma_Grab_Volt_Value = 0;
  static float movingAverage = 0;
  float temp = 0;
  Queue<float> tempMA = Queue<float>(AVERAGE_WINDOW_SIZE);//temporarily holds the value of the queue to find the sum
  
  #ifdef DEBUG
  Serial.print("sample: ");
  Serial.print(Ma_Grab_Volt_Value);
  Serial.print("  ");
  Serial.println();
  #endif

  pushQueue(Ma_Grab_Volt_Value);

  while (Mov_Avg_VoltQ.count() != 0) //.count tells how many floats are in the queue
  {
    if (Mov_Avg_VoltQ.count() != 0)
    {
      #ifdef DEBUG
      Serial.print("[");
      Serial.print(Mov_Avg_VoltQ.count() - 1);
      Serial.print("]: ");
      Serial.print(Mov_Avg_VoltQ.peek());
      Serial.print("  ");
      #endif
    }
    
    temp = Mov_Avg_VoltQ.peek();//this is the temp queue
    tempMA.push(temp);
    Mov_Avg_VoltQ.pop();
    sumMA += temp;
  }

  while (tempMA.count() != 0)//rebuilds the queue
  {
    Mov_Avg_VoltQ.push(tempMA.peek());
    tempMA.pop();
  }

  movingAverage = sumMA / Mov_Avg_VoltQ.count();

  #ifdef DEBUG
  Serial.print("ma: ");
  Serial.print(movingAverage);
  Serial.print(" V  ");
  Serial.println();
  #endif

  sumMA = 0;
  return movingAverage;
}

float grabFloatValue()
{
  while (Serial.available() == 0);//waits for user input

  float grabFloat = Serial.parseFloat();

  return grabFloat;
}

int grabIntValue()
{
  while (Serial.available() == 0);//waits for user input

  int grabInt = Serial.parseInt();

  return grabInt;
}

float slopeIntercept(float y, float x)//y = mx + c with c == 0
{
  float slope = 0;

  slope = y / x;

  Serial.print("input voltage: ");
  Serial.print(y);
  Serial.println();
  Serial.print("output voltage: ");
  Serial.print(x);
  Serial.println();
  Serial.print("the slope is: ");
  Serial.println(slope);
  Serial.println();

  return slope;
}

float battery_Avg_Scale_Factor(unsigned int input)
{
  float scaled_Output = 0;
  float battery_Volt_Val = 0;
  int state_of_Scale_Factor = 0;
  float avg_Scale_Factor = 0;
  float sum_Scale_Fator = 0;
  float Ma_Grab_Volt_Value = 0;

  while (state_of_Scale_Factor < NUM_SCALE_SAMPLES)
  {
    Serial.println("Enter a inital battery power level.");

    battery_Volt_Val = grabFloatValue();

    Serial.print("applied Voltage: ");
    Serial.print(battery_Volt_Val);
    Serial.print("  ");
    Serial.println();

    clearQueue();

    for (int i = 0; i < AVERAGE_WINDOW_SIZE; i++)//repopulate the queue with the new Voltage reading
    {
      MovingAverage(input);
    }

    state_of_Scale_Factor ++;
    
    scaled_Output = MovingAverage(input);

    sum_Scale_Fator += slopeIntercept(battery_Volt_Val, scaled_Output); //(y,x) where y is the battery pack and x is the adc output voltage 

  }

  avg_Scale_Factor = sum_Scale_Fator / NUM_SCALE_SAMPLES;

  Serial.print("The average slope of the 4 is : ");
  Serial.print(avg_Scale_Factor);
  Serial.println();

  //findInitVolt(avg_Scale_Factor);

  return avg_Scale_Factor;
}

//void findInitVolt(float scaleFactor)
//{
//  float MA = 0;
//
//  Serial.print("the scale factor is: ");
//  Serial.println(scaleFactor);
//  Serial.println("press '2' to end the program ");
//  Serial.println();
//
//  while (Serial.parseInt() != 2)
//  {
//    for (int i = 0; i < AVERAGE_WINDOW_SIZE; i++)//repopulate the queue with the new Voltage reading
//    {
//      MA = MovingAverage();
//    }
//
//    Serial.print("adc Voltage: ");
//    Serial.println(MA);
//    Serial.print("calculated input Voltage: ");
//    Serial.print(MA * scaleFactor);
//    Serial.println();
//    Serial.println();
//
//    delay(1000);
//  }
//}

void clearQueue()
{
  stateFill = 0;//these 2 lines will reset "pushQueue"
  stateFilled = false;

  while (Mov_Avg_VoltQ.count() != 0)
  {
    Mov_Avg_VoltQ.pop();
  }
}
