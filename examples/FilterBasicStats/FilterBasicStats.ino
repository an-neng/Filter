/** 
 * Filter example code
 * 
 * Simple example demonstrating the "Filter" Arduino library
 * 
 * This code reads an analog sensor and outputs the current value, as well 
 * as the mean, median, mode, and standard deviation values (based on the 
 * last 100 values).  This data is output as text to the Arduino hardware 
 * serial port.  
 *
 * Author: Karl Ward
 * License: this example code is released into the public domain
 * (note that the Filter library itself is not public domain)
 */

#include "Filter.h"

// we're going to store 100 values for analysis
Filter filteredData(100); 

// and we're going to get those values from pin A0
int analogPin = A0; 

void setup() { 
  // open serial port to see debugging output
  Serial.begin(9600);
} 

void loop() { 
  // read the sensor
  int analogValue = analogRead(analogPin);

  // put that sensor value into the Filter object
  filteredData.prepend(analogValue); 

  // calculate mean, median, mode, and standard deviation
  int analogMean = filteredData.mean(); 
  int analogMedian = filteredData.median(); 
  int analogMode = filteredData.mode(); 
  int analogStdev = filteredData.stdev(); 

  // output the results to the serial port so you can see them
  Serial.print("analog sensor value is "); 
  Serial.println(analogValue); 
  Serial.print("  mean is "); 
  Serial.println(analogMean); 
  Serial.print("  median is "); 
  Serial.println(analogMedian);
  Serial.print("  mode is "); 
  Serial.println(analogMode); 
  Serial.print("  standard deviation is "); 
  Serial.println(analogStdev); 

  delay(250); // short delay so we don't flood the serial monitor
}
