#ifndef _GLSTARS_H_
#define _GLSTARS_H_

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "stars.h"


class TGLStars : public TStars
{
private:
	GLUquadricObj *quadric;

	HDC fDC;	// Private GDI Device Context
	HGLRC fRC;	// Permanent Rendering Context
	HWND fWnd;	// Holds Our Window Handle

	float fWidthFactor;

protected:
	void InitGL(HWND hWnd);
	void KillGL();
public:

	TGLStars(HWND hWnd);
	virtual ~TGLStars();
	virtual void SetScreenSize(int width, int height);
	virtual void DrawCircle(float x, float y, float radius);
	virtual void ShowActiveScreen();
	virtual bool BeforeDraw();
	virtual bool AfterDraw();

};

#endif // _GLSTARS_H_
