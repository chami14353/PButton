/*
 * Simple arduino pushbutton handling library
 * PButton.cpp
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

#include "PButton.h"

#define  BTN_EVT_PRESS 0
#define  BTN_EVT_RELEASE 1
#define  BTN_EVT_RELEASE_BEFORE_HOLD 2
#define  BTN_EVT_HOLD 3
#define  BTN_EVT_REPEAT 4

PButton::PButton(uint8_t pin, uint8_t pinSetup, short holdTime, short repeatTime) { 
  init(pin, pinSetup, holdTime, repeatTime);
}

PButton::PButton(uint8_t pin, uint8_t pinSetup) { 
  init(pin, pinSetup, DEF_HOLD_TIME, DEF_REPEAT_TIME);	
}

PButton::PButton(uint8_t pin) { 
  init(pin, INPUT_PULLUP, DEF_HOLD_TIME, DEF_REPEAT_TIME);
}

void PButton::init(uint8_t pin, uint8_t pinSetup, short holdTime, short repeatTime) { 
  _debouncer = Bounce();
  pinMode(pin, pinSetup);
  _debouncer.attach(pin);
  _debouncer.interval(5);
  _holdTime = holdTime;
  _repeatTime = repeatTime;
}

void PButton::update(){
  static callback currentCallbacks[NUM_EVENTS];
  
  if ( _debouncer.update() ) {
	// Get the update value
	if ( _debouncer.read() == HIGH) {
	   released(currentCallbacks);
	   _isPressed = false;
	   _isHeld = false;
	 } else {
		 for (byte i=0; i<NUM_EVENTS; i++) {
			currentCallbacks[i] = btnCallbacks[i]; 
		 }
	   pressed(currentCallbacks);
		 
	   _isPressed = true;
	   _buttonPressTimeStamp = millis(); 
	 }
  } 
  if  ( _isPressed ) {
	if (!_isHeld && ( millis() - _buttonPressTimeStamp >= _holdTime )) {
	  _isHeld = true;
	  _repeatTimeAccel = _repeatTime;
	  held(currentCallbacks);
	  _buttonPressTimeStamp = millis();
	} else if (_isHeld && millis() - _buttonPressTimeStamp >= _repeatTimeAccel ) {
	  repeated(currentCallbacks);
	  _buttonPressTimeStamp = millis();
	  if (_repeatTimeAccel > (_repeatTime >> 3)) _repeatTimeAccel-= (_repeatTime >> 4);
	}
  }
}

void PButton::resetEvents() {
  for (byte i = 0; i < NUM_EVENTS; i++) {
	btnCallbacks[i] = NULL;
  }
}

void PButton::pressed(callback btnCallbacks[]) {
	//only the pressed event
	if (btnCallbacks[BTN_EVT_PRESS] != NULL) 
		(*btnCallbacks[BTN_EVT_PRESS]) ();  
}

void PButton::held(callback btnCallbacks[]){
	if (btnCallbacks[BTN_EVT_HOLD] != NULL) 
		(*btnCallbacks[BTN_EVT_HOLD]) ();  
}

void PButton::released(callback btnCallbacks[]){
	if ( btnCallbacks[BTN_EVT_RELEASE_BEFORE_HOLD] != NULL 
		&&
		((!_isHeld)
		|| (_isHeld && btnCallbacks[BTN_EVT_HOLD] == NULL))) 
		(*btnCallbacks[BTN_EVT_RELEASE_BEFORE_HOLD]) ();  
	else if (btnCallbacks[BTN_EVT_RELEASE] != NULL) 
		(*btnCallbacks[BTN_EVT_RELEASE]) ();  
}

void PButton::repeated(callback btnCallbacks[]){
	if (btnCallbacks[BTN_EVT_REPEAT] != NULL) 
		(*btnCallbacks[BTN_EVT_REPEAT]) ();  
}


void PButton::addPushEvent(callback action, callback endAction) {
	btnCallbacks[BTN_EVT_PRESS] = action;
	btnCallbacks[BTN_EVT_RELEASE] = endAction;  
}

void PButton::addPushEvent(callback action) {
	addPushEvent(action, NULL);
}

void PButton::addShortEvent(callback action){
	btnCallbacks[BTN_EVT_RELEASE_BEFORE_HOLD] = action;
//    btnCallbacks[BTN_EVT_PRESS] = NULL;
}

void PButton::addLongEvent(callback action){
	btnCallbacks[BTN_EVT_HOLD] = action;
}

void PButton::addRepeatEvent(callback action) {
	addRepeatEvent(action, NULL);
}

void PButton::addRepeatEvent(callback action, callback endAction){
	btnCallbacks[BTN_EVT_REPEAT] = action;
	
	if (btnCallbacks[BTN_EVT_PRESS] == NULL)
		btnCallbacks[BTN_EVT_PRESS] = action;
		
	if (btnCallbacks[BTN_EVT_HOLD] == NULL)
		btnCallbacks[BTN_EVT_HOLD] = action;
		
	btnCallbacks[BTN_EVT_RELEASE] = endAction;
}

