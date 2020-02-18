#include "wakeupUnit.h"

wakeupUnit::wakeupUnit()
{
}

void wakeupUnit::setActivationTime(char* activation)
{
	for (int i = 0; i < 6; i++)
	{
		activationTime_[i] = activation[i];
	}
}

void wakeupUnit::setAdress(char* adress)
{
	for (int i = 0; i < 6; i++)
	{
		adress_[i] = adress[i];
	}
}

void wakeupUnit::setDeactivationTime(char* deactivation)
{
	for (int i = 0; i < 6; i++)
	{
		deactivationTime_[i] = deactivation[i];
	}
}

char wakeupUnit::getActivationTime(int i) const
{
	return activationTime_[i];
}

char wakeupUnit::getAdress(int i) const
{
	return adress_[i];
}

char wakeupUnit::getDeactivationTime(int i) const
{
	return deactivationTime_[i];
}
