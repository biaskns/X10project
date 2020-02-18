#include "wakeupUnit.h"

#ifndef _WAKEUPROUTINE_H_
#define  _WAKEUPROUTINE_H_

class wakeupRoutine
{
	public:
	wakeupRoutine();
	void parseData(const char*);
	void addWakeupUnit(char*, char*, char*);
	wakeupRoutine getWakeupUnit(int i);
	wakeupUnit unitArr[16];
	
	void initClock(); //Timer 5
	char compare(); //return index+1 of unitArray
	int minutesSinceBoot = 0;
	char secondsCounter = 0;
	char timeAtBoot[6] = "00:00";
	
	
	private:
	int index_ = 0;
	int compareActIndex_ = 0;
	int compareDeactIndex_ = 0;
	
};

#endif
