#ifndef _STARS_H_
#define _STARS_H_

#include "consts.h"
#include "starsreg.h"

extern struct KBStateStruct
{
	bool rightarrow, leftarrow, uparrow, downarrow;
	bool pageup, pagedown, a, plus, minus, tab, del, home, end;
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
	float fFloatHeight, fFloatWidth;
	
	float *x, *y, *z;

	int zfar;
	float xspeed, zspeed;


	void InitStars();
	void CleanupStars();

	virtual void SetSpeed(float NewSpeed);
	
public:
	TStars();
	virtual ~TStars();
	virtual void ResetDefaults();
	virtual void ChangeStarCount(int newcount);
	virtual void UpdateSettings();
	virtual void SetScreenSize(int width, int height);

	static void DrawAllStars();
	virtual bool DrawStars();
	virtual void MoveStars();

	virtual void DrawCircle(float x, float y, float radius) = 0;
	virtual void ShowActiveScreen() = 0;
	virtual bool BeforeDraw() = 0;
	virtual bool AfterDraw() = 0;

	void OnKeyDel();
};

#endif // _STARS_H_
