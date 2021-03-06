#include <stdlib.h>
#include <math.h>

#include "stars.h"
#include "logging/logging.h"



struct KBStateStruct KBState;

int TStars::InstanceCount = 0;
TStars* TStars::StarsList[32]; // maximum of 31 displays (0 reserved for all)
TStarsSettings* TStars::settings = NULL;
int TStars::ActiveScreen = 0;
int TStars::ActiveScreenShowTime = 0;


void TStars::ClearKBState()
{
	TraceMethod trace(90, "TStars::ClearKBState");
	KBState.rightarrow = 
		KBState.leftarrow = 
		KBState.uparrow = 
		KBState.downarrow = 
		KBState.pageup = 
		KBState.pagedown = 
		KBState.plus =
		KBState.minus =
		KBState.tab =
		KBState.del =
		KBState.home =
		KBState.end = false;
}

TStars::TStars(TStarsRenderer *renderer)
{
	TraceMethod trace(100, "TStars::TStars");
	fCurInstance = RegisterInstance(this);
	
	fRenderer = renderer;

	LoadSettings();

	fHalfHeight = fHalfWidth = 0;
	fHeight = fWidth = 0;
	SetScreenSize(fRenderer->ScreenWidth, fRenderer->ScreenHeight);
	x = y = z = NULL;
	fTimeRemaining = 0;
	fBiggestStarCount = 0;
	InitStars();
}

TStars::~TStars()
{
	TraceMethod trace(100, "TStars::~TStars");
	SaveSettings();
	delete fRenderer;
	CleanupStars();
	UnregisterInstance(fCurInstance);
}

void TStars::SetScreenSize(int width, int height)
{
	TraceMethod trace(90, "TStars::SetScreenSize");
	SendLogMessage(90, "%i, %i", width, height);
	fWidth = width;
	fHeight = height;
	fFloatWidth = width;
	fFloatHeight = height;
	fHalfWidth = width / 2;
	fHalfHeight = height / 2;
}

void TStars::CleanupStars()
{
	TraceMethod trace(90, "TStars::CleanupStars");
	if (x)
		free(x);
	if (y)
		free(y);
	if (z)
		free(z);
}

inline int GetRandomX()
{
//	TraceMethod trace(10, "TStars::GetRandomX");
	return (rand() - 16384) * 5000;
}

inline int GetRandomY()
{
//	TraceMethod trace(10, "TStars::GetRandomY");
	return (rand() - 16384) * 1000;
}

void TStars::InitStars()
{
	TraceMethod trace(90, "TStars::InitStars");
	SendLogMessage(50, "Starcount = %i", fStarCount);
	x = (float *)malloc(sizeof(float) * fStarCount); 
	y = (float *)malloc(sizeof(float) * fStarCount); 
	z = (float *)malloc(sizeof(float) * fStarCount); 

	// sin() and cos() are used here, but they're outside the loop. The only
	// time they're used is when the movement angle is changed	
	SetSpeed(fSpeed);
//	xspeed = sin(fAngle) * fSpeed * XZCONVERSION;
//	zspeed = cos(fAngle) * fSpeed;

	// This is where the stars are initally created.  The ranges used for
	// x, y, and z coordinates was determined by trial and error
	// The (rand() % 20000) * 5000 instead of (rand() % 100000000) is because
	// rand() doesn't produce numbers in that range
	int i;
	for (i = 0; i < fStarCount; i++)
	{
		z[i] = rand() % ZMAX - (ZMAX / 2);
		x[i] = GetRandomX();
		y[i] = GetRandomY();
	}
	fHalfWidth = fWidth / 2;
	fHalfHeight = fHeight / 2;
}

void TStars::ChangeStarCount(int newcount)
{
	TraceMethod trace(90, "TStars::ChangeStarCount");
	float *temp;
	if (fBiggestStarCount == 0)
		fBiggestStarCount = fStarCount;
	if (newcount > fBiggestStarCount)
	{
		temp = (float *)realloc(x, sizeof(float) * newcount);
		if (!temp)
			return;
		x = temp;
		temp = (float *)realloc(y, sizeof(float) * newcount);
		if (!temp)
			return;
		y = temp;
		temp = (float *)realloc(z, sizeof(float) * newcount);
		if (!temp)
			return;
		z = temp;
		fBiggestStarCount = newcount;
	}
	for (int i = fStarCount; i < newcount; i++)
	{
		z[i] = rand() % ZMAX - (ZMAX / 2);
		x[i] = GetRandomX();
		y[i] = GetRandomY();
	}
	fStarCount = newcount;
}


void TStars::ResetDefaults()
{
	TraceMethod trace(90, "TStars::ResetDefaults");
	DELAY = 10;
	CleanupStars();

	settings->ResetDefaults();
	fStarCount = settings->StarCount;
	fRadius = settings->Radius;
	fSpeed = settings->Speed;
	fAngle = settings->Angle;
	fSubpixelPositioning = settings->SubpixelPositioning;

	InitStars();
}

bool TStars::DrawStars()
{
	TraceMethod trace(40, "TStars::DrawStars");
	if (!fRenderer->BeforeDraw())
		return false;

	if (ActiveScreenShowTime > 0)
	{
		if ((ActiveScreen == fCurInstance) ||
				((fCurInstance == InstanceCount) && (ActiveScreen == 0)))
			ActiveScreenShowTime--;
		if ((ActiveScreen == fCurInstance) || (ActiveScreen == 0))
			fRenderer->ShowActiveScreen(ActiveScreenShowTime);
	}

// This is the main section where the current star is drawn
// Notice the lack of sines, cosines, arctangents, and square roots
// If you want to reduce it to the barest algorithm without error
// checking or special cases, this is all there is:
// {
//	calcrad = radius / z[i];
//	calcx = (int) (x[i] / z[i]) + HALFWIDTH;
//	calcy = (int) (y[i] / z[i]) + HALFHEIGHT;
//	drawcircle(calcx, calcy, (int) calcrad, 0xFFFFFF);
// }
// The rest is just bounds checking and optimizations.
//
	float calcrad;
	float calcx, calcy;
	int i;
	int logcount = 0;
	for (i = 0; i < fStarCount; i++)
		if (z[i] > 1)
		{
			calcrad = fRadius / z[i];
			if (calcrad > .125)
			{
				calcx = (x[i] / z[i]) + fHalfWidth;
				calcy = (y[i] / z[i]) + fHalfHeight;
				if ((calcx > -calcrad && calcx - calcrad < fFloatWidth) 
						&& (calcy > -calcrad && calcy - calcrad < fFloatHeight))
				{
					if (fSubpixelPositioning)
						fRenderer->DrawCircleExact(calcx, calcy, calcrad);
					else
						fRenderer->DrawCircle(calcx, calcy, calcrad);
				}
			}
		}
	
	if (!fRenderer->AfterDraw())
		return false;
}

void TStars::WrapStars()
{
// If it reaches the defined edge of the X axis, wrap it around
	for (int i = 0; i < fStarCount; i++)
	{
		if (x[i] < -XMAX / 2)
			x[i] += XMAX;
		else if (x[i] > XMAX / 2)
			x[i] -= XMAX;
	}
// If it reaches the edge of visibility, wrap it around and reset X and Y axes
	for (int i = 0; i < fStarCount; i++)
	{
		if (z[i] < 0)
		{
			z[i] += ZMAX;
			x[i] = GetRandomX();
			y[i] = GetRandomY();
		} 
		else if (z[i] > ZMAX)
		{
			z[i] -= ZMAX;
			x[i] = GetRandomX();
			y[i] = GetRandomY();
		}
	}
}

void TStars::MoveStars(float MillisecondsElapsed)
{
	TraceMethod trace(35, "TStars::MoveStars");
	xspeed = sin(fAngle) * fSpeed * XZCONVERSION * MillisecondsElapsed * 0.1f;
	zspeed = cos(fAngle) * fSpeed * MillisecondsElapsed * 0.1f;
	
	for (int i = 0; i < fStarCount; i++)
	{
		x[i] -= xspeed;
		z[i] -= zspeed;
	}
	
	fTimeRemaining -= MillisecondsElapsed;
	if (fTimeRemaining <= 0.0f)
	{
		fTimeRemaining += 100.0f;
		WrapStars();
		UpdateSettings();
	}
}


void TStars::UpdateSettings()
{
	TraceMethod trace(49, "TStars::UpdateSettings");
	{
		static bool last = false;
		if (!last && KBState.tab)
		{
			SendLogMessage(80, "ActiveScreen was %i", ActiveScreen);
			do
			{
				ActiveScreen++;
				ActiveScreen %= (InstanceCount < 32) ? InstanceCount + 1 : 32;
			} while ((ActiveScreen != 0) && (StarsList[ActiveScreen] == NULL));
			ActiveScreenShowTime = 100;
			SendLogMessage(80, "Tab, changing screen to %i", ActiveScreen);
		}
		last = KBState.tab;
	}
	if ((ActiveScreen != fCurInstance) && (ActiveScreen != 0))
		return;
	if (KBState.s && !LastS)
	{
		if (fSubpixelPositioning) 
			fSubpixelPositioning = false;
		else
			fSubpixelPositioning = true;
	}
	LastS = KBState.s;
	if (KBState.rightarrow)
	{
		SendLogMessage(50, "Right arrow, rotate clockwise");
		fAngle += .1;
		SetSpeed(fSpeed);
	}
	if (KBState.leftarrow)
	{
		SendLogMessage(50, "Left arrow, rotate counterclockwise");
		fAngle -= .1;
		SetSpeed(fSpeed);
	}
	if (KBState.uparrow)
	{
		SendLogMessage(50, "Up arrow, speed up");
		if (fSpeed < .01)
			fSpeed = .01;
		SetSpeed(fSpeed * 1.05);
	}
	if (KBState.downarrow)
	{
		SendLogMessage(50, "Down arrow, slow down");
		SetSpeed(fSpeed / 1.05);
	}
	if (KBState.pageup)
	{
		int NewStarCount = (fStarCount >= 10) ? (int)(fStarCount * 1.05) : 10;
		SendLogMessage(50, "Page up, more stars, %i", NewStarCount);
		ChangeStarCount(NewStarCount);
	}
	if (KBState.pagedown)
	{
		int NewStarCount = (int)(fStarCount / 1.05);
		SendLogMessage(50, "Page down, less stars, %i", NewStarCount);
		ChangeStarCount(NewStarCount);
	}
	if (KBState.plus)
	{
		SendLogMessage(50, "Keypad plus, radius bigger");
		fRadius *= 1.01;
	}
	if (KBState.minus)
	{
		SendLogMessage(50, "Keypad minus, radius smaller");
		fRadius /= 1.01;
		if (fRadius < 1)
			fRadius = 1;
	}
	if (!LastDel && KBState.del)
	{
		ResetDefaults();
		SendLogMessage(50, "Keypad delete, resetting to defaults");
	}
	LastDel = KBState.del;
	if (KBState.home)
	{
		if (KBState.home == 1)
		{
			SetDelayAll(DELAY + 1);
			SendLogMessage(50, "Keypad home, increasing delay");
		}
		KBState.home %= 5;
		KBState.home++;
	}
	if (KBState.end)
	{
		if (KBState.end == 1)
		{
			SetDelayAll(DELAY - 1);
			SendLogMessage(50, "Keypad end, decreasing delay");
		}
		KBState.end %= 5;
		KBState.end++;
	}
};


void TStars::OnKeyDel()
{
	TraceMethod trace(90, "TStars::OnKeyDel");
	ResetDefaults();
}

void TStars::SetSpeed(float NewSpeed)
{
	fSpeed = NewSpeed;
}

void TStars::SetDelay(int NewDelay)
{
//	SetSpeed(fSpeed * NewDelay / DELAY);
}



bool TStars::LoadSettings()
{
	TraceMethod trace(90, "TStarsReg::LoadSettings()");
	settings->LoadSettings(fCurInstance);
	fStarCount = settings->StarCount;
	fRadius = settings->Radius;
	fSpeed = settings->Speed;
	fAngle = settings->Angle;
	fSubpixelPositioning = settings->SubpixelPositioning;
}
void TStars::SaveSettings()
{
	TraceMethod trace(90, "TStarsReg::SaveSettings()");
	settings->StarCount = fStarCount;
	settings->Radius = fRadius;
	settings->Speed = fSpeed;
	settings->Angle = fAngle;
	settings->SubpixelPositioning = fSubpixelPositioning;
	settings->SaveSettings(fCurInstance);
}

void TStars::HandleKeyEventAll()
{
	TraceMethod trace(50, "TStars::HandleKeyEventAll");
	for (int i = 1; i <= InstanceCount; i++)
		if (StarsList[i])
			StarsList[i]->UpdateSettings();
}

void TStars::DrawStarsAll()
{
	TraceMethod trace(40, "TStars::DrawAllStars");
	for (int i = 1; i <= InstanceCount; i++)
		if (StarsList[i])
			StarsList[i]->DrawStars();
}

void TStars::MoveStarsAll(float MillesecondsElapsed)
{
	TraceMethod trace(40, "TStars::DrawAllStars");
	for (int i = 1; i <= InstanceCount; i++)
		if (StarsList[i])
			StarsList[i]->MoveStars(MillesecondsElapsed);
}

int TStars::RegisterInstance(TStars* instance)
{
	int ID = ++InstanceCount;
	if (ID == 1)
		ClearKBState();
	if (ID < 32)
		StarsList[ID] = instance;
	SendLogMessage(90, "Instance #%i", ID);
	return ID;
}

void TStars::UnregisterInstance(int ID)
{
	if (ID > 0 && ID < 32)
		StarsList[ID] = NULL;
}

void TStars::DestroyAll()
{
	TraceMethod trace(100, "static TStars::DestroyAll");
	for (int i = 1; i <= InstanceCount; i++)
		if (StarsList[i])
			delete StarsList[i];
	delete settings;
}

void TStars::SetDelayAll(int NewDelay)
{
	TraceMethod trace(50, "TStars::SetDelayAll()");
	if (NewDelay < 5)
		NewDelay = 5;
	if (NewDelay > 50)
		NewDelay = 50;
	for (int i = 1; i <= InstanceCount; i++)
		if (StarsList[i])
			StarsList[i]->SetDelay(NewDelay);
	DELAY = NewDelay;
}


