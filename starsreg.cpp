
#include <stdio.h>

#include "starsreg.h"
#include "consts.h"
#include "logging/logging.h"
#include "RegistrySettings.h"


int TStarsReg::InstanceCount = 0;
TStarsReg *TStarsReg::StarsList[32]; // maximum of 31 displays (0 reserved for all)

TStarsReg::TStarsReg()
{
	TraceMethod trace(100, "TStarsReg::TStarsReg()");

	fRadius = 8000;
	fSpeed = 5 * DELAY;
	fAngle = 0;
	fStarCount = 10000;

	fCurInstance = ++InstanceCount;
	if (fCurInstance < 32)
		StarsList[fCurInstance] = this;
	SendLogMessage(90, "Instance #%i", fCurInstance);
}

TStarsReg::~TStarsReg()
{
	TraceMethod trace(100, "TStarsReg::~TStarsReg()");
	if (fCurInstance < 32)
		StarsList[fCurInstance] = NULL;
}

void TStarsReg::SetSpeed(float NewSpeed)
{
	fSpeed = NewSpeed;
}

void TStarsReg::SetDelay(int NewDelay)
{
	SetSpeed(fSpeed * NewDelay / DELAY);
}

void TStarsReg::SetAllDelay(int NewDelay)
{
	if (NewDelay < 5)
		NewDelay = 5;
	if (NewDelay > 50)
		NewDelay = 50;
	for (int i = 1; i <= InstanceCount; i++)
		if (StarsList[i])
			StarsList[i]->SetDelay(NewDelay);
	DELAY = NewDelay;
}

