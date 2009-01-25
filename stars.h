#ifndef _STARS_H_
#define _STARS_H_

#include "consts.h"
#include "starsreg.h"
#include "StarsRenderer.h"
#include "StarsSettings.h"

extern struct KBStateStruct
{
	bool rightarrow, leftarrow, uparrow, downarrow;
	bool s, pageup, pagedown, plus, minus, tab, del, home, end;
} KBState;


class TStars : public TStarsReg
{
private:
	int fDelayIterations;
	bool LastS, LastPlus, LastMinus, LastDel;
	void ClearKBState();
protected:
	static int ActiveScreen;
	static int ActiveScreenShowTime;

	TStarsRenderer *fRenderer;
	int fBiggestStarCount;
	int fWidth, fHeight;
	float fHalfWidth, fHalfHeight;
	float fFloatHeight, fFloatWidth;
	
	float *x, *y, *z;

	int zfar;
	float xspeed, zspeed;

	bool LoadSettings();
	void SaveSettings();

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

	static TStarsSettings *settings;
	static void DestroyAll();
};

#endif // _STARS_H_
