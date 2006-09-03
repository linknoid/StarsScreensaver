#include <stdlib.h>
#include <math.h>

#include "stars.h"
#include "logging/logging.h"



struct KBStateStruct KBState;

void TStars::ClearKBState()
{
	MethodTrace trace(90, "TStars::ClearKBState");
	KBState.rightarrow = 
		KBState.leftarrow = 
		KBState.uparrow = 
		KBState.downarrow = 
		KBState.pageup = 
		KBState.pagedown = false;
}

TStars::TStars()
{
	MethodTrace trace(100, "TStars::TStars");
	ClearKBState();

	fWidth = fHalfWidth = fHeight = fHalfHeight = 0;

	x = y = z = NULL;


	fRadius = 8000;
	fSpeed = 5 * DELAY;
	fAngle = 0;
	fStarCount = 10000;
	fBiggestStarCount = 0;
	if (!LoadSettings())
		return;
	InitStars();
}

TStars::~TStars()
{
	MethodTrace trace(100, "TStars::~TStars");
	SaveSettings();
	CleanupStars();
}

void TStars::SetScreenSize(int width, int height)
{
	MethodTrace trace(90, "TStars::SetScreenSize");
	fWidth = width;
	fHeight = height;
	fHalfWidth = width / 2;
	fHalfHeight = height / 2;
}

bool TStars::LoadSettings()
{
	MethodTrace trace(90, "TStars::LoadSettings");
	return true;
}

void TStars::SaveSettings()
{
	MethodTrace trace(90, "TStars::SaveSettings");
}

void TStars::CleanupStars()
{
	MethodTrace trace(90, "TStars::CleanupStars");
	if (x)
		free(x);
	if (y)
		free(y);
	if (z)
		free(z);
}

void TStars::InitStars()
{
	MethodTrace trace(90, "TStars::InitStars");
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
	MethodTrace trace(90, "TStars::ChangeStarCount");
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
	MethodTrace trace(90, "TStars::ResetDefaults");
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
	MethodTrace trace(40, "TStars::DrawStars");
	if (!BeforeDraw())
		return false;

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
	{
		calcrad = fRadius / z[i];
		if (z[i] > 1 && calcrad > .125)
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

	static int delayiterations = 0;
	++delayiterations;
	delayiterations %= 10;
	if (delayiterations == 0)
		UpdateSettings();
}


void TStars::MoveStars()
{
	MethodTrace trace(35, "TStars::MoveStars");
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
	MethodTrace trace(50, "TStars::UpdateSettings");
	if (KBState.rightarrow)
	{
		fAngle += .01 * DELAY;
		xspeed = sin(fAngle) * fSpeed * XZCONVERSION;
		zspeed = cos(fAngle) * fSpeed;
	}
	if (KBState.leftarrow)
	{
		fAngle -= .01 * DELAY;
		xspeed = sin(fAngle) * fSpeed * XZCONVERSION;
		zspeed = cos(fAngle) * fSpeed;
	}
	if (KBState.uparrow)
	{
        fSpeed *= 1.05;
		xspeed = sin(fAngle) * fSpeed * XZCONVERSION;
		zspeed = cos(fAngle) * fSpeed;
	}
	if (KBState.downarrow)
	{
        fSpeed /= 1.05;
		xspeed = sin(fAngle) * fSpeed * XZCONVERSION;
		zspeed = cos(fAngle) * fSpeed;
	}
	if (KBState.pageup)
	{
		ChangeStarCount((int)(fStarCount * 1.1));
	}
	if (KBState.pagedown)
	{
		ChangeStarCount((int)(fStarCount / 1.1));
	}
};


void TStars::OnKeyA()
{
	MethodTrace trace(90, "TStars::OnKeyA");
	if (fAntialias == true)
	{
		SendLogMessage("Antialiasing disabled");
		fAntialias = false;
	}
	else
	{
		SendLogMessage("Antialiasing enabled");
		fAntialias = true;
	}
}

void TStars::OnKeyDel()
{
	MethodTrace trace(90, "TStars::OnKeyDel");
	ResetDefaults();
}

void TStars::OnKeypadPlus()
{
	MethodTrace trace(90, "TStars::OnKeypadPlus");
	fRadius *= 1.03;
}

void TStars::OnKeypadMinus()
{
	MethodTrace trace(90, "TStars::OnKeypadMinus");
	fRadius /= 1.03;
}


