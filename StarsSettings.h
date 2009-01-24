#ifndef _STARSSETTINGS_H_
#define _STARSSETTINGS_H_

#include "consts.h"

class TStarsSettings
{
public:
	int StarCount;
	float Radius, Speed, Angle;
	int Delay;

	virtual bool LoadSettings(int Instance) = 0;
	virtual void SaveSettings(int Instance) = 0;
	void ResetDefaults()
	{
		Radius = 8000;
		Speed = 5 * DELAY;
		Angle = 0;
		StarCount = 10000;
	}
};


#endif // _STARSSETTINGS_H_
