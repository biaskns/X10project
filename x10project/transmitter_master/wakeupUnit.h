
#ifndef __WAKEUPUNIT_H__
#define  __WAKEUPUNIT_H__

class wakeupUnit
{
	public:
	wakeupUnit();
	void setActivationTime(char* activation);
	void setAdress(char* adress);
	void setDeactivationTime(char* deactivation);
	char getActivationTime(int i) const;
	char getAdress(int i) const;
	char getDeactivationTime(int i) const;
	
	char activationTime_[6];
	char adress_[3];
	char deactivationTime_[6];
	
	private:

};

#endif
