#ifndef _STARS_H_
#define _STARS_H_

#include "consts.h"
#include "starsreg.h"

extern struct KBStateStruct
{
	bool rightarrow, leftarrow, uparrow, downarrow;
	bool pageup, pagedown, a, plus, minus, tab, del;
} KBState;


class TStars : public TStarsReg
{
private:
	int fDelayIterations;
	bool LastA, LastPlus, LastMinus, LastDel;
	void ClearKBState();
protected:
	static int ActiveScreen;
	static int ActiveScreenShowTime;

	int fBiggestStarCount;
	int fWidth, fHeight, fHalfWidth, fHalfHeight;
	
	float *x, *y, *z;

	int zfar;
	int calcx, calcy, calccolor;
	float xspeed, zspeed;


	void InitStars();
	void CleanupStars();

public:
	TStars();
	virtual ~TStars();
	void ResetDefaults();
	void ChangeStarCount(int newcount);
	void UpdateSettings();
	void SetScreenSize(int width, int height);

	bool DrawStars();
	void MoveStars();

	virtual void DrawCircle(int x, int y, float radius, int c) = 0;
	virtual void ShowActiveScreen() = 0;
	virtual bool BeforeDraw() = 0;
	virtual bool AfterDraw() = 0;

	void OnKeyDel();
};

#endif // _STARS_H_
