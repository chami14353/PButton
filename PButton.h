/*
 * Simple arduino pushbutton handling library
 * PButton.h
 * Copyright (C) 2016  Nacho Lucia
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef PBUTTON_H
#define PBUTTON_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif
#include "Bounce2.h"

#define NUM_EVENTS 5

#define DEF_HOLD_TIME 500
#define DEF_REPEAT_TIME 200

class PButton {
  typedef void (*callback) (void);

  private:
    unsigned short _holdTime;
    unsigned short _repeatTime;
    byte _repeatTimeAccel;
    bool _isPressed;
    bool _isHeld;
    unsigned long _buttonPressTimeStamp;
    Bounce _debouncer;
    
    callback btnCallbacks[NUM_EVENTS];
    
    
	void init(uint8_t pin, uint8_t pinSetup, short holdTime, short repeatTime);
    void pressed(callback*);
    void held(callback*);
    void released(callback*);
    void repeated(callback*);
      
  public:
    PButton(uint8_t pin);
    PButton(uint8_t pin, uint8_t pinSetup);
    PButton(uint8_t pin, uint8_t pinSetup, short holdTime, short repeatTime);
    
    void update();
    void addPushEvent(callback);
    void addPushEvent(callback, callback);
    void addShortEvent(callback);
    void addLongEvent(callback);
    void addRepeatEvent(callback);
    void addRepeatEvent(callback, callback);
    void resetEvents();

};

#endif
