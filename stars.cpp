#include <stdlib.h>
#include <math.h>

#include "stars.h"
#include "logging/logging.h"



struct KBStateStruct KBState;

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
		KBState.a = false;
}

TStars::TStars() : TStarsReg()
{
	TraceMethod trace(100, "TStars::TStars");
	if (fCurInstance == 1)
		ClearKBState();

	fWidth = fHalfWidth = fHeight = fHalfHeight = 0;
	x = y = z = NULL;
	fDelayIterations = 0;
	fBiggestStarCount = 0;
	InitStars();
}

TStars::~TStars()
{
	TraceMethod trace(100, "TStars::~TStars");
	CleanupStars();
}

void TStars::SetScreenSize(int width, int height)
{
	TraceMethod trace(90, "TStars::SetScreenSize");
	fWidth = width;
	fHeight = height;
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

void TStars::InitStars()
{
	TraceMethod trace(90, "TStars::InitStars");
	SendLogMessage(50, "Starcount = %i", fStarCount);
	x = (float *)malloc(sizeof(float) * fStarCount); 
	y = (float *)malloc(sizeof(float) * fStarCount); 
	z = (float *)malloc(sizeof(float) * fStarCount); 

	// sin() and cos() are used here, but they're outside the loop. The only
	// time they're used is when the movement angle is changed	
	xspeed = sin(fAngle) * fSpeed * XZCONVERSION;
	zspeed = cos(fAngle) * fSpeed;

	// This is where the stars are initally created.  The ranges used for
	// x, y, and z coordinates was determined by trial and error
	// The (rand() % 20000) * 5000 instead of (rand() % 100000000) is because
	// rand() doesn't produce numbers in that range
	int i;
	for (i = 0; i < fStarCount; i++)
	{
		z[i] = rand() % ZMAX - (ZMAX / 2);
		x[i] = (rand() % 20000 - 10000) * 5000;
		y[i] = (rand() % 20000 - 10000) * 1000;
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
		x[i] = (rand() % 20000 - 10000) * 5000;
		y[i] = (rand() % 20000 - 10000) * 1000;
	}
	fStarCount = newcount;
}


void TStars::ResetDefaults()
{
	TraceMethod trace(90, "TStars::ResetDefaults");
	CleanupStars();
	fRadius = 8000;
	fSpeed = 5 * DELAY;
	fAngle = 0;
	fStarCount = 10000;
	fBiggestStarCount = 0;
	InitStars();
}


bool TStars::DrawStars()
{
	TraceMethod trace(40, "TStars::DrawStars");
	if (!BeforeDraw())
		return false;

	if (ActiveScreenShowTime > 0)
	{
		if ((ActiveScreen == fCurInstance) ||
				((fCurInstance == InstanceCount) && (ActiveScreen == 0)))
			ActiveScreenShowTime--;
		if ((ActiveScreen == fCurInstance) || (ActiveScreen == 0))
			ShowActiveScreen();
	}

	MoveStars();

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
	int i;
	for (i = 0; i < fStarCount; i++)
		if (z[i] > 1)
		{
			calcrad = fRadius / z[i];
			if (calcrad > .125)
			{
				calcx = (int) (x[i] / z[i]) + fHalfWidth;
				calcy = (int) (y[i] / z[i]) + fHalfHeight;
				if (calcx <= 0 || calcy <= 0 || calcx >= fWidth - 1 || calcy >= fHeight - 1)
					continue;
				DrawCircle(calcx, calcy, calcrad, 0xFFFFFF);
			}
		}
	


	if (!AfterDraw())
		return false;

	++fDelayIterations;
	fDelayIterations %= 10;
	if (fDelayIterations == 0)
		UpdateSettings();
}


void TStars::MoveStars()
{
	TraceMethod trace(35, "TStars::MoveStars");
	int i;
	for (i = 0; i < fStarCount; i++)
	{
		x[i] -= xspeed;
		z[i] -= zspeed;
// If it reaches the defined edge of the X axis, wrap it around
		if (x[i] < -XMAX / 2)
			x[i] += XMAX;
		else if (x[i] > XMAX / 2)
			x[i] -= XMAX;
// If it reaches the edge of visibility, wrap it around and reset X and Y axes
		if (z[i] < 0)
		{
			z[i] += ZMAX;
			x[i] = (rand() % 20000 - 10000) * 5000;
			y[i] = (rand() % 20000 - 10000) * 1000;
		} 
		else if (z[i] > ZMAX)
		{
			z[i] -= ZMAX;
			x[i] = (rand() % 20000 - 10000) * 5000;
			y[i] = (rand() % 20000 - 10000) * 1000;
		}
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
	if (KBState.rightarrow)
	{
		SendLogMessage(50, "Right arrow, rotate clockwise");
		fAngle += .01 * DELAY;
		xspeed = sin(fAngle) * fSpeed * XZCONVERSION;
		zspeed = cos(fAngle) * fSpeed;
	}
	if (KBState.leftarrow)
	{
		SendLogMessage(50, "Left arrow, rotate counterclockwise");
		fAngle -= .01 * DELAY;
		xspeed = sin(fAngle) * fSpeed * XZCONVERSION;
		zspeed = cos(fAngle) * fSpeed;
	}
	if (KBState.uparrow)
	{
		SendLogMessage(50, "Up arrow, speed up");
        fSpeed *= 1.05;
		xspeed = sin(fAngle) * fSpeed * XZCONVERSION;
		zspeed = cos(fAngle) * fSpeed;
	}
	if (KBState.downarrow)
	{
		SendLogMessage(50, "Down arrow, slow down");
        fSpeed /= 1.05;
		xspeed = sin(fAngle) * fSpeed * XZCONVERSION;
		zspeed = cos(fAngle) * fSpeed;
	}
	if (KBState.pageup)
	{
		SendLogMessage(50, "Page up, more stars");
		ChangeStarCount((int)(fStarCount * 1.1));
	}
	if (KBState.pagedown)
	{
		SendLogMessage(50, "Page down, less stars");
		ChangeStarCount((int)(fStarCount / 1.1));
	}
	if (!LastPlus && KBState.plus)
	{
		SendLogMessage(50, "Keypad plus, radius bigger");
		fRadius *= 1.03;
	}
	LastPlus = KBState.plus;
	if (!LastMinus && KBState.minus)
	{
		SendLogMessage(50, "Keypad minus, radius smaller");
		fRadius /= 1.03;
	}
	LastMinus = KBState.minus;
	if (!LastA && KBState.a)
	{
		if (fAntialias) fAntialias = false; else fAntialias = true;
		SendLogMessage(50, "Antialiasing %sabled", fAntialias ? "en" : "dis");
	}
	LastA = KBState.a;
	if (!LastDel && KBState.del)
	{
		ResetDefaults();
		SendLogMessage(50, "Keypad delete, resetting to defaults");
	}
	LastDel = KBState.del;
};


void TStars::OnKeyDel()
{
	TraceMethod trace(90, "TStars::OnKeyDel");
	ResetDefaults();
}


