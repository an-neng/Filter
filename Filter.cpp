/**
 * Filter - Arduino data filtering library
 * 
 * The Filter library provides Arduino programmers with data filtering 
 * operations on a configurable number of recent values.
 * 
 * Copyright 2012-2013 Karl Ward
 * See the file CREDITS for contributors and external code referenced/incorporated
 * See the file COPYING for details on software licensing
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Version 0.5.0 */ 

#include "Arduino.h"
#include "Filter.h"

// Constructor
Filter::Filter(long sampleSize) {
  // sample size indicates how many values to store for mean, median, etc. 
  _sampleSize = sampleSize; 
  // object will store values in array (well, pointer) of specified size, plus 1
  _values = (long *) malloc(sizeof(long) * (_sampleSize+1)); 
  _valuesFirst = _sampleSize; // index to oldest value, initialize to last index
  _valuesLast = _sampleSize; // index to newest value, initialize to last index
  _valuesCount = 0; // no values stored yet
  _medianValues = NULL; // _medianValues is undefined until median() is called 
}

void Filter::put(long value) {
  if ((_valuesFirst == _sampleSize) || (_valuesLast == _sampleSize)) { // no values yet 
    _values[0] = value; 
    _valuesFirst = 0; 
    _valuesLast = 0; 
    _valuesCount++; 
  } 
  else if (_valuesCount < _sampleSize) { // not full of values yet
    _valuesLast = (_valuesLast + 1) % _sampleSize; 
    _values[_valuesLast] = value; 
    _valuesCount++; 
  } 
  else { // overwriting old values now
    _values[_valuesFirst] = value; 
    _valuesFirst = (_valuesFirst + 1) % _sampleSize; 
    _valuesLast = (_valuesLast + 1) % _sampleSize; 
  }
}

long Filter::mean() { 
  long sum = 0;
  // sum all values
  // NOTE: we're doing floating point math in long rather than using floats
  for (long i=0; i < _valuesCount; i++) { 
    sum = sum + (_values[i] * 10); // multiply by 10 to do FP math
  }
  _mean = sum / _valuesCount;
  _mean = _longRound(_mean, 10); 
  return(_mean); 
}

long Filter::median() { 
  // erase any previous values used to determine median
  if (_medianValues != NULL) { 
    free(_medianValues); 
    _medianValues = NULL; 
    _medianValuesCount = 0; 
  } 
  // allocate memory to store ordered set of values
  _medianValues = (long *) malloc(sizeof(long) * (_sampleSize));
  // create an ordered array of the latest values 
  for (long i=0; i < _valuesCount; i++) { 
    //_orderedInsert(_values[i], 0); // insert into _medianValues array
    _orderedInsert2(_values[i]); 
  } 
  
  // median is the element in the middle of the ordered list of values
  long midpoint = 0; 
  if (_valuesCount > 1) { 
    midpoint = (_valuesCount - 1) / 2; 
  }
  if (_valuesCount % 2 == 1) { // we have an odd number of values
    _median = _medianValues[midpoint]; 
  } 
  else { // we have an even number of values, so get mean of midpoint pair
    // NOTE: we're doing floating point math in long rather than using floats
    _median = ((_medianValues[midpoint] + _medianValues[midpoint+1]) * 10) / 2;
    _median = _longRound(_median, 10); 
  }
  return(_median); 
}

void Filter::_moveOver(long start, long end) { 
  for (long i = end; i > start; i--) { 
    _medianValues[i] = _medianValues[i-1]; 
  }
} 

// non-recursive version, CPU intensive
void Filter::_orderedInsert2(long value) { 
  for (long j = 0; j <= _medianValuesCount; j++) { 
    if (j == _medianValuesCount) { 
      _medianValues[j] = value; 
      _medianValuesCount++; 
      //Serial.print("inserting value "); 
      //Serial.print(_medianValues[j]); 
      //Serial.print(" at pos "); 
      //Serial.println(j); 
      return; // break out of the loop 
    }
    else if (value < _medianValues[j]) {
      //_orderedInsert(_medianValues[pos], pos+1); 
      _moveOver(j, _medianValuesCount); 
      _medianValues[j] = value; 
      _medianValuesCount++; 
      //Serial.print("inserting value "); 
      //Serial.print(_medianValues[j]); 
      //Serial.print(" at pos "); 
      //Serial.println(j); 
      return; // break out of the loop  
    } 
  }
} 

// NOTE: recursive, memory intensive
void Filter::_orderedInsert(long value, long pos) { 
  if (_medianValuesCount < _valuesCount) { 
    if (pos == _medianValuesCount) { 
      _medianValues[pos] = value; 
      _medianValuesCount++; 
    }
    else if (value < _medianValues[pos]) {
      _orderedInsert(_medianValues[pos], pos+1); 
      _medianValues[pos] = value;   
    } 
    else if (value >= _medianValues[pos]) { 
      _orderedInsert(value, pos+1); 
    }
  }
} 

// signal to noise ratio, defined as mean divided by standard deviation
// returns percentage (long, rather than float)
// TODO: implement different SNR algorithms
long Filter::signalToNoise() { // FIXME: rename to signalPercentage ? 
  //long snr; // signal to noise ratio
  long sp; // signal as percentage
  long sd = stdev(); // NOTE: signalToNoise() relies on stdev() calling mean() 
                     //   to update _mean, this avoids calling mean() twice
  if (sd == 0) { // we don't want to divide by zero
    sp = 100; // all signal, no noise 
  } 
  else { 
    //snr = (_mean * 1000) / sd; 
    //snr = _longRound(snr, 10); // FIXME: this should not be 1000, should prob. be 10
    // TODO: test math here 
    sp = sd * 1000 / _mean; // using long rather than float 
    sp = _longRound(sp, 1000) * 100; // multiply by 100 to get percentage
  } 
  return(sp); 
}

long Filter::stdev() { 
  // make sure we have the most recent mean calculated
  mean();

  // standard deviation calculation  
  long sum = 0; 
  for (long i=0; i < _valuesCount; i++) { 
    sum += sq(_values[i] - _mean) * 100; // i.e. a multiplier of 10 (100 is 10 squared)
  } 
  _stdev = sqrt(sum / _valuesCount);
  _stdev = _longRound(_stdev, 10); // round and undo that multiplier of 10

  return(_stdev); 
} 

long Filter::_longRound(long input, long multiplier) { 
  if (input % multiplier < (multiplier/2)) { 
    input = input / multiplier; // round down 
  }
  else { 
    input = (input / multiplier) + 1; // round up
  } 
  return(input); 
} 

long Filter::maximum() { 
  for (long i=0; i < _valuesCount; i++) { 
    if ((i == 0) || (_values[i] > _maximum)) { 
      _maximum = _values[i]; 
    } 
  } 
  return(_maximum); 
} 

long Filter::minimum() { 
  for (long i=0; i < _valuesCount; i++) { 
    if ((i == 0) || (_values[i] < _minimum)) { 
      _minimum = _values[i]; 
    } 
  } 
  return(_minimum); 
} 

String Filter::describe() { 
  String description = String("stored values count: "); 
  description.concat(_valuesCount); 
  description.concat(" of "); 
  description.concat(_sampleSize); 
  description.concat("\n"); 

  description.concat("values: "); 
  for (long i=0; i < _valuesCount; i++) { 
    description.concat(_values[i]); 
    description.concat(' '); 
  } 
  description.concat("\n"); 
  return(description);  
}
