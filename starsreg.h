#ifndef _STARSREG_H_
#define _STARSREG_H_

#include <stdio.h>
#include "StarsSettings.h"

class TStarsReg
{
protected:
	static int InstanceCount;
	static TStarsReg *StarsList[32]; // maximum of 32 displays
	int fCurInstance;

	int fStarCount; 
	float fRadius, fSpeed, fAngle;

	
	virtual void SetSpeed(float NewSpeed);
	virtual void SetDelay(int NewDelay);
public:
	TStarsReg();
	virtual ~TStarsReg();


	virtual bool DrawStars() = 0;
	static void SetAllDelay(int NewDelay);
};

#endif // _STARSREG_H_

