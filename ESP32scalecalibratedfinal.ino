#include <Wire.h>
#include "SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h"

NAU7802 myScale;

// Offset value for zero calibration
long zeroOffset = 0;

// Calibration factor (force per unit reading)
float calibrationFactor = 0.00034;  // Adjust this value based on your calibration

void setup()
{
  Serial.begin(9600);
  Serial.println("Qwiic Scale Example");

  Wire.begin();

  if (myScale.begin() == false)
  {
    Serial.println("Scale not detected. Please check wiring. Freezing...");
    while (1);
  }
  Serial.println("Scale detected!");

  // Perform zero calibration
  calibrateZero();
}

void loop()
{
  if (myScale.available() == true)
  {
    long currentReading = myScale.getReading() - zeroOffset;
    float force = currentReading * calibrationFactor;

    //Serial.print("Current Reading: ");
    //Serial.print(currentReading);
    Serial.print("");
    Serial.println(force, 3);  // Print force value with 4 decimal places
  }
}

void calibrateZero()
{
  // This function should be called when no load is applied to the load cell.
  // It sets the current reading as the zero point.

  Serial.println("Calibrating zero...");

  long sum = 0;
  int samples = 100;  // Number of readings to average

  // Take multiple readings and calculate the average
  for (int i = 0; i < samples; ++i)
  {
    sum += myScale.getReading();
    delay(10); // Delay between readings
  }

  zeroOffset = sum / samples;

  Serial.print("Zero calibration offset: ");
  Serial.println(zeroOffset);

  Serial.println("Zero calibration completed.");
}
