#ifndef _STARSSETTINGS_H_
#define _STARSSETTINGS_H_

#include "consts.h"

class TStarsSettings
{
public:
	int StarCount;
	float Radius, Speed, Angle;
	int Delay;
	bool SubpixelPositioning;

	virtual bool LoadSettings(int Instance) = 0;
	virtual void SaveSettings(int Instance) = 0;
	void ResetDefaults()
	{
		Radius = 15000;
		Speed = 5 * DELAY;
		Angle = 0;
		StarCount = 15000;
		SubpixelPositioning = true;
	}
};


#endif // _STARSSETTINGS_H_
