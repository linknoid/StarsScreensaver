#ifndef _STARS_H_
#define _STARS_H_

#include "consts.h"
#include "starsreg.h"
#include "StarsRenderer.h"

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

	TStarsRenderer *fRenderer;
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
	TStars(TStarsRenderer *renderer);
	virtual ~TStars();
	virtual void ResetDefaults();
	virtual void ChangeStarCount(int newcount);
	virtual void UpdateSettings();
	virtual void SetScreenSize(int width, int height);

	static void DrawAllStars();
	virtual bool DrawStars();
	virtual void MoveStars();

	void OnKeyDel();
};

#endif // _STARS_H_
