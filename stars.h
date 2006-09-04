#ifndef _STARS_H_
#define _STARS_H_

#include "consts.h"

extern struct KBStateStruct
{
	bool rightarrow, leftarrow, uparrow, downarrow;
	bool pageup, pagedown, a, plus, minus, tab, del;
} KBState;


class TStars
{
private:
	int fDelayIterations;
	bool LastA, LastPlus, LastMinus, LastDel;
	void ClearKBState();
protected:
	static int InstanceCount;
	static TStars *StarsList[32]; // maximum of 32 displays
	static int ActiveScreen;
	int CurInstance;

	int fStarCount, fBiggestStarCount;
	int fWidth, fHeight, fHalfWidth, fHalfHeight;
	bool fAntialias;

	float *x, *y, *z;
	float fRadius, fSpeed, fAngle;

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

	virtual bool LoadSettings();
	virtual void SaveSettings();
	virtual void DrawCircle(int x, int y, float radius, int c) = 0;
	virtual bool BeforeDraw() = 0;
	virtual bool AfterDraw() = 0;

	void OnKeyDel();
};

#endif // _STARS_H_
