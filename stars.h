#ifndef _STARS_H_
#define _STARS_H_

extern const int ZMAX;
extern const int XMAX;
extern const int XZCONVERSION;
extern const int DELAY;
extern const int PriorityForLogging;


extern struct KBStateStruct
{
	bool rightarrow, leftarrow, uparrow, downarrow;
	bool pageup, pagedown;
} KBState;


class TStars
{
private:
	void ClearKBState();
protected:

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


	void OnKeyA();
	void OnKeyDel();
	void OnKeypadPlus();
	void OnKeypadMinus();

};

#endif // _STARS_H_
