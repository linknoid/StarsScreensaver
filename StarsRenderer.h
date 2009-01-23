#ifndef _STARSRENDERER_H_
#define _STARSRENDERER_H_

class TStarsRenderer
{
public:
	int ScreenWidth, ScreenHeight;
	virtual void SetScreenSize(int width, int height) = 0;
	virtual void DrawCircle(float x, float y, float radius) = 0;
	virtual void ShowActiveScreen(int TimeLeft) = 0;
	virtual bool BeforeDraw() = 0;
	virtual bool AfterDraw() = 0;
};

#endif // _STARSRENDERER_H_
