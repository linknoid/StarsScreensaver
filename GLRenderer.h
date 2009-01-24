#ifndef _GLRENDERER_H_
#define _GLRENDERER_H_

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "StarsRenderer.h"


class TGLStars : public TStarsRenderer
{
private:
	GLUquadricObj *quadric;

	HDC fDC;	// Private GDI Device Context
	HGLRC fRC;	// Permanent Rendering Context
	HWND fWnd;	// Holds Our Window Handle

	float fWidthFactor;
	int fHalfHeight, fHalfWidth;

protected:
	void InitGL(HWND hWnd);
	void KillGL();
public:

	TGLStars(HWND hWnd);
	virtual ~TGLStars();
	virtual void SetScreenSize(int width, int height);
	virtual void DrawCircle(float x, float y, float radius);
	virtual void ShowActiveScreen(int TimeLeft);
	virtual bool BeforeDraw();
	virtual bool AfterDraw();

};

#endif // _GLRENDERER_H_
