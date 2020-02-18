#include "wakeupRoutine.h"
#include <avr\io.h>
#include <stdlib.h>
#include <string.h>


wakeupRoutine::wakeupRoutine()
{
}

void wakeupRoutine::parseData(const char *data)
{
	const char *string = data;
	char wakeupData[] = "14:14";

	char tempActivation[6];
	char tempAdress[6];
	char tempDeactivation[6];

	

	int occurences = 0;
	int i = 0;
	while (string[i] != '\0')
	{
		if (string[i] == '\'') occurences++;

		i++;
	}

	i = 1;
	while (i < (occurences))
	{
		string = strchr(string, '\'');
		string++;
		if ((i % 2) == 0);
		else
		{
			memcpy(wakeupData, string, ((strchr(string, '\'') - string)));


			if (((i / 2) % 3) == 2)
			{
				strcpy(tempDeactivation, wakeupData);
				addWakeupUnit(tempActivation, tempAdress, tempDeactivation);
			}
			else if (((i / 2) % 3) == 1)
			{
				strcpy(tempAdress, wakeupData);
			}
			else if (((i / 2) % 3) == 0)
			{
				strcpy(tempActivation, wakeupData);
			}


		}
		
		i++;
	}
	
	//Get time//
	string = strchr(string, '\(');
	string++;
	
	memcpy(wakeupData, string, ((strchr(string, '\)') - string)));
	for (int i = 0; i < 6; i++)
	{
		timeAtBoot[i] = wakeupData[i];
	}
}

void wakeupRoutine::addWakeupUnit(char* activationTime, char* adress, char* deactivationTime)
{
	unitArr[index_].setActivationTime(activationTime);
	unitArr[index_].setAdress(adress);
	unitArr[index_].setDeactivationTime(deactivationTime);

	index_++;
}

void wakeupRoutine::initClock()
{
	OCR5A = 62500; 
	
	TIMSK5 |= 0b00000010; //Output Compare A interrupt
	TCCR5A |= 0b11000000; //Output Compare A
	TCCR5B |= 0b00001101; //1024 prescaler
}

char wakeupRoutine::compare()
{
	char currentTime[4];
	char buffer[2];
	
	if ((minutesSinceBoot ) >= (24 * 60)) // when timeAtBoot reached
	{
		minutesSinceBoot = 0;
	}
	
	int MinutesSinceMidnight = ((timeAtBoot[4] - '0') * 1) + ((timeAtBoot[3] - '0') * 10) + ((timeAtBoot[1] - '0') * 60) + ((timeAtBoot[0] - '0') * 60 * 10) + minutesSinceBoot;
	
	
	itoa(MinutesSinceMidnight / 60 / 10, buffer,10); //Hours tens
	currentTime[0] = buffer[0];
	
	itoa(MinutesSinceMidnight / 60 % 10, buffer,10); //Hours ones
	currentTime[1] = buffer[0];
	
	itoa(MinutesSinceMidnight % 60 / 10, buffer,10); //Minutes tens
	currentTime[3] = buffer[0];
	
	itoa((MinutesSinceMidnight % 60) % 10, buffer,10); //Minutes ones
	currentTime[4] = buffer[0];
	
	if ( (currentTime[0] - '0') >= 3  || ((currentTime[0] - '0') >= 2 && (currentTime[1] - '0') >= 4))
	{
		//char tempString[2];
		//tempString[0] = currentTime[0];
		//tempString[1] = currentTime[1];
		itoa(((((currentTime[0] - '0') * 10) + ((currentTime[1] - '0') * 1)) - 24) / 10, buffer,10);
		currentTime[0] = buffer[0];
		itoa(((((currentTime[0] - '0') * 10) + ((currentTime[1] - '0') * 1)) - 24) % 10, buffer,10);
		currentTime[1] = buffer[0];
	}
	
	
	while (compareActIndex_ < 16)
	{
		if (( unitArr[compareActIndex_].activationTime_[0] == currentTime[0] ) && ( unitArr[compareActIndex_].activationTime_[1] == currentTime[1] ) && ( unitArr[compareActIndex_].activationTime_[3] == currentTime[3] ) && ( unitArr[compareActIndex_].activationTime_[4] == currentTime[4] ))
		{
			return compareActIndex_ + 1; // Command: Turn on
		} 
		compareActIndex_++;
	}
	compareActIndex_ = 0;
	
	while (compareDeactIndex_ < 16)
	{
		if (unitArr[compareDeactIndex_].deactivationTime_[0] == currentTime[0] && unitArr[compareDeactIndex_].deactivationTime_[1] == currentTime[1] && unitArr[compareDeactIndex_].deactivationTime_[3] == currentTime[3] && unitArr[compareDeactIndex_].deactivationTime_[4] == currentTime[4])
		{
			return compareDeactIndex_ + 1 + 16; // Command: Turn off
		}
		compareDeactIndex_++;
	}
	compareDeactIndex_ = 0;
	
	return 0;
	
}