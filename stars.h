#ifndef _STARS_H_
#define _STARS_H_

#include "consts.h"
#include "StarsRenderer.h"
#include "StarsSettings.h"

extern struct KBStateStruct
{
	bool rightarrow, leftarrow, uparrow, downarrow;
	bool s, pageup, pagedown, plus, minus, tab, del, home, end;
} KBState;


class TStars
{
private:
	float fTimeRemaining;
	bool LastS, LastPlus, LastMinus, LastDel;
	static void ClearKBState();
protected:
	static int RegisterInstance(TStars* instance);
	static void UnregisterInstance(int ID);
	static int InstanceCount;
	static TStars *StarsList[32]; // maximum of 32 displays
	int fCurInstance;

	static int ActiveScreen;
	static int ActiveScreenShowTime;

	TStarsRenderer *fRenderer;
	int fBiggestStarCount;
	int fWidth, fHeight;
	float fHalfWidth, fHalfHeight;
	float fFloatHeight, fFloatWidth;

	int fStarCount; 
	float fRadius, fSpeed, fAngle;
	bool fSubpixelPositioning;
	
	float *x, *y, *z;

	int zfar;
	float xspeed, zspeed;

	bool LoadSettings();
	void SaveSettings();

	void InitStars();
	void CleanupStars();

	void SetSpeed(float NewSpeed);
	void SetDelay(int NewDelay);
	void WrapStars();
	
public:
	TStars(TStarsRenderer *renderer);
	virtual ~TStars();
	virtual void ResetDefaults();
	virtual void ChangeStarCount(int newcount);
	virtual void UpdateSettings();
	virtual void SetScreenSize(int width, int height);

	virtual bool DrawStars();
	virtual void MoveStars(float MillisecondsElapsed);

	void OnKeyDel();

	static TStarsSettings *settings;

	static void HandleKeyEventAll();
	static void DrawStarsAll();
	static void MoveStarsAll(float MillisecondsElapsed);
	static void DestroyAll();
	static void SetDelayAll(int NewDelay);
};

#endif // _STARS_H_
