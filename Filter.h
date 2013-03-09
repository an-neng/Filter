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

#ifndef Filter_h
#define Filter_h 

#include "Arduino.h"

class Filter {
  public:
    // constructor, create a Filter that contains up to specified number of values
    Filter(long sampleSize); 

    // put a new value into the Filter object, discarding oldest value if necessary
    void put(long value); 

    // return a string describing the state of the object
    String describe(); 

    // return the absolute maximum, largest value currently in Filter object 
    long maximum(); // absolute maximum only for now

    // return the mean, average of values currently in Filter object
    long mean(); 

    // return the median, most commonly appearing value in Filter object
    long median(); 

    // return the absolute minimum, smallest value currently in Filter object 
    long minimum(); // absolute minimum only for now

    // signal percentage, calculated from ratio of mean to standard deviation
    long signalPercentage(); 

    // return the standard deviation of values currently in Filter object
    long stdev(); 

  private:
    // data
    long _maximum; 
    long _mean;
    long _median;
    long *_medianValues;
    long _medianValuesCount;
    long _minimum; 
    long _sampleSize;
    float _stdev; 
    long *_values;
    long _valuesCount;
    long _valuesFirst;
    long _valuesLast;

    // methods
    long _longRound(long input, long multiplier); 
    void _moveOver(long start, long end); 
    void _orderedInsertRecursive(long value, long pos);
    void _orderedInsert(long value);
};

#endif

